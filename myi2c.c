/* //////////////////////////////////////////////////////////////////////////////////////
** File:      myi2c.c
 Guido Ottaviani-->guido@guiott.com<--
* \version 1.0.0
* \date 05/14
-------------------------------------------------------------------------------
\copyright 2014 Guido Ottaviani
guido@guiott.com

    myi2c is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    myi2c is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with myi2c.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------------------------

* \details
High and low level I2C procedures
 * this is developed to be compatible with a full interrupt driven program with
 * no polling or "wait until".
 * It uses i2c.h standard header just for some definitions and the opening
 * procedures.
 *
 * PLEASE READ A FULL DESCRIPTION OF THE PROCEDURES AT THE END OF THIS FILE
 *
 *
 * How to  use it:
 *  add myi2c.c and myi2c.h files to your project
 *  add the following code to your low priority ISR
 *
   void interrupt low_priority low_isr (void)
   {
    if (PIR1bits.SSPIF)     // an I2C event is over
    {
        PIR1bits.SSPIF = 0;	// reset I2c interrupt flag
        I2cEventFlag = 1;   // I2cLowService will be executed
    }

    if (PIR2bits.BCLIF)     // I2c bus collision
    {
        PIR2bits.BCLIF = 0; // interrupt flag reset
        I2cEventFlag = 1;   // execute I2cLowService
        I2cBusCollFlag = 1;	// a collision occurred
    }
   }
 *
 * in myI2c.h set:
 *  I2C_MAX_DEV with the number of I2C devices to control
 *      e.g.: #define	I2C_MAX_DEV 6
 *  I2cAdr[] array with the devices addresses
 *      e.g.: const unsigned char I2cAdr[] = {0x00,0xC0,0x50,0x40,0x42,0xE0};
 *  define the pointers into the structure for the devices
 *      e.g.:
            #define EXT_EEPROM_PTR 0
            #define CMP_PTR 1
            #define AD_EXPANDER_PTR 2
            #define LCD_PTR 3
            #define KEYPAD_PTR 4
            #define SONAR_PTR 5
 * call I2cSetting() before the main loop starts
 * call I2cService() in main loop
 * call I2cBuffChk() to check if both buffers are empty before starting a new
 *  sequence
 * 
 * after that, whenever an I2C exchange is needed, The device specific routine
 * must fill up the buffer and sets the I2cHighService flags according to the
 * istructions below
 *
*/
#include <plib/i2c.h>
#include "myi2c.h"

int i = 0;
int j = 0;

void I2cSettings(void)
{
    OpenI2C(MASTER, SLEW_OFF);	//	Master I2C, slew rate off per clock=100KHz
    SSPADD=99;                  //	100KHz baud clock @ 40MHz
                                //	SSPAD=((Fosc/BitRate)/4)-1=(400/4)-1=99

    //-------Interrupts
    IPR1bits.SSPIP=1;       //SSP int = high priority
    IPR2bits.BCLIP=1;       //BUS COLLISION int = high priority
    PIR1bits.SSPIF=0;       //Clear MSSP Interrupt flag
    PIE1bits.SSPIE=1;       //SSP (I2C events) int enabled
    PIE2bits.BCLIE=1;       //BUS COLLISION int enabled

    // ---- init I2C
    I2cBusCollFlag = 0;
    I2cEventFlag = 0;
    I2cBusyFlag = 0;
    for (i=0; i<I2C_MAX_DEV; i++)
    {
      I2c[i].Flag.Tx = 0;
      I2c[i].Flag.Rx = 0;
      I2c[i].Done = 0;
      for (j=0; j<=1; j++)
      {
        I2c[i].RxBuff[j] = 0;
      }

      for (j=0; j<=3; j++)
      {
        I2c[i].TxBuff[j] = 0;
      }
    }
}

void I2cLowService (void)
{
  I2cEventFlag=0;	// wait for next interrupt

	if (I2cBusCollFlag || SSPCON2bits.ACKSTAT)
	{	// Collision on bus or NACK ->  sequence aborted
		I2cBusCollFlag = 0; // error status reset
		I2cStat = FINE;				 // current status = FINE
	  StopI2C();						 // sends stop
	  // it doesn't reset RX & TX counter to retry on next round
	}
	
	else
	{	
	// execute scheduled sequence for each byte in the selected record
	
		switch (I2cStat)
		{
			case (STRT):	// START sequence
        if (I2c[I2cDevPtr].Flag.Tx > 0)// something to send?
			  {
 	           I2cStat = WRITE;             // next status
 	           SSPBUF = I2cAdr[I2cDevPtr];  // update flag: R/W = write
				}
        else  // if no bytes to send it has some byte to receive
				{
   	        I2cStat = READ;              // next status
   	        SSPBUF = I2cAdr[I2cDevPtr]+1;// update flag: R/W = read
   			} 
				break;
			
			
			case (WRITE):
				/* sends Nth byte
					 all bytes sents?
           all bytes received?
           otherwise stops sequences
				*/
				SSPBUF = I2c[I2cDevPtr].TxBuff[Ptr.Tx];    // TX first byte
				Ptr.Tx ++;                                 // points to next byte
				if (Ptr.Tx >= I2c[I2cDevPtr].Flag.Tx)      // all bytes sents?
				{
   				if (I2c[I2cDevPtr].Flag.Rx > 0)  // all bytes received?
   				{
   					I2cStat = RSTART;              // starts RX sequence
   				}
          else                             // nothing to receive
   				{
      				I2cStat = STP;               
     			}
    		}
        else                             // still something to TX
    		{
    				I2cStat = WRITE;          	 // TX next byte(s)
    		}
				break;
				
				
			case (READ):
				SSPCON2bits.RCEN = 1;           				// starts RX sequence
				Ptr.Rx ++;                        			// next byte Rx
				if (Ptr.Rx >= I2c[I2cDevPtr].Flag.Rx)   // all bytes received?
				{
   				I2cStat = NACK;				   			// YES, send NACK (Rx over)
 				}
				else
				{
					I2cStat = ACK;								// NO, send ACK (Rx proceed)
				}			
				break;
				
				
			case (ACK):
				I2c[I2cDevPtr].RxBuff[Ptr.Rx-1] = SSPBUF;	// store Nth byte received
				I2cStat = READ;                           // more bytes
				AckI2C();
				break;
			
				
			case (NACK):
				I2c[I2cDevPtr].RxBuff[Ptr.Rx-1] = SSPBUF;	// store Nth byte received
				I2cStat = STP;                           // last byte received
			  NotAckI2C();			  			 	
				break;
				
				
			case (RSTART):
				// reinizializza bus senza rilasciarlo
				I2cStat = ADRR;              // next status = start RX
				RestartI2C();
				break;
				
				
			case (ADRR):
				I2cStat = READ;              // next status = RX
				SSPBUF = I2cAdr[I2cDevPtr]+1;// update flag: R/W = read
  			break;
		
		
			case (STP):			
    		I2cStat = FINE;             // next status
      	I2c[I2cDevPtr].Flag.Rx = 0; // no more bytes to RX or TX
	   		I2c[I2cDevPtr].Flag.Tx = 0; // high level procedures can start again
	   		StopI2C();					        // send stop
        break;
				
				
			case (FINE):
				I2cBusyFlag = 0;	  	      // I2C comm over
        I2c[I2cDevPtr].Done = 1;    // procedure complete for this device
				break;
			
				
			default:
				break;
				
		}	// end switch
	}	// end else
					
} // I2cLowService **********

void I2cHighService (void)
{
 /*
 initialize I2C sequence resetting flags, counters and buffers
 kick start
 all the following procedures will be triggered by interrupts
 */
	Ptr.Tx = 0;				// reset TX buffer pointer
	Ptr.Rx = 0;				// reset RX buffer pointer
	I2c[I2cDevPtr].RxBuff[0] = 0; // reset RX buffer
	I2c[I2cDevPtr].RxBuff[1] = 0; // reset RX buffer
	I2cEventFlag = 0;
	I2cBusCollFlag = 0;
	I2cBusyFlag = 1;
	I2cStat = START;	// FSM pointer
	
	StartI2C();				// start with first status 
	
} // I2cHighService **********

void I2cService (void)
{
  if (I2cEventFlag)
  //SSP ISR signals that previous I2C action's over
 
  {
    I2cLowService();// next action
    // exit
  }
  else
  {
    if (I2cBusyFlag)	// I2cHighService started a new I2C sequence
                        // after this managed via ISR
                        // at the end I2cLowService will free up the bus
    {
    // exit
    }

    else
    {
      if ((I2c[I2cDevPtr].Flag.Rx > 0) || (I2c[I2cDevPtr].Flag.Tx > 0))
      // check another record into the buffer for more bytes to exchange
      {
        I2cHighService();	// Initialize a new comm sequence
                          // and get the bus
        // exit
       }

      else
      {
        if(I2cDevPtr < (I2C_MAX_DEV-1))	// more records to check
         {
          I2cDevPtr ++;			// next record
            // exit
         }

        else 							// all records in buffer checked
        {
          I2cDevPtr = 0;	// start again from first device
        }
      }
    }
   }
}

unsigned char I2cBuffChk (unsigned char I2cDevice)
{// Test if both RX and TX buffer are empty before performing other actions
    return(!I2c[I2cDevice].Flag.Tx && !I2c[I2cDevice].Flag.Rx);
}

void I2cData (unsigned char DevPtr, unsigned char TxBytes, 
                unsigned char TX1, unsigned char TX2, unsigned char TX3,
                unsigned char TX4, unsigned char RxBytes)
{// Exchange data with I2C device
    I2c[DevPtr].Flag.Tx = TxBytes;	// n bytes to TX
		I2c[DevPtr].TxBuff[0] = TX1 ;   // first byte
    I2c[DevPtr].TxBuff[1] = TX2 ;   // second byte
		I2c[DevPtr].TxBuff[2] = TX3 ;   // third byte
		I2c[DevPtr].TxBuff[3] = TX4 ;   // fourth byte
	  I2c[DevPtr].Flag.Rx = RxBytes;	// n bytes to RX
    I2c[DevPtr].Done = 0;           // reset flag, wait for execution
}

/*
 ------------------------------------------------------------------------------
Description of the I2C communication procedures

 * Analyzing the I2C library source code it's clear that some of that uses
 * waiting loops for the execution of some SSP operations, therefore they are
 * not compatible with a pseudo-RTOS program structure
 * i.e.:

    ReadI2C		wait until byte	received
    getcI2C		idem
    getsI2C		perform	getcI2C() for 'length' number of bytes
              check that receive sequence	is over
              wait until ACK sequence	is over
    IdleI2C		Test and wait until	I2C	module is idle
    WriteI2C	wait until write cycle is complete
    putI2C		idem
    putsI2C		transmit data until	null character
              test for idle condition
              wait until ninth clock pulse received

 * some of them can instead be used
    AckI2C		ok
    CloseI2C	ok
    DataRdyI2C	ok
    NotAckI2C	ok
    OpenI2C		ok
    RestartI2C	ok
    StopI2C		ok
    StartI2C	ok

 * All of the EEPROM procedures contains wait on idle as well as of LCD contains
 * many delays

 * All of the critical procedures has been rewritten in a time-safe mode using
 * SSP interrupts, status flags and global variables in a Finite State Machine
 * way.
 * A usefull trace to use this kind of technique is the Microchip AN736.


 * The I2C communication is performed in several layers

 * -Application layer:
 * each I2C device has its own properties.
 * each I2C device has its own buffer (4 byte for RX & 4 byte for TX)
 * the device specific routine fills up the buffer with the bytes to exchange
 * and sets the flags according to the actions to perform:
	I2c[I2cDevPtr].Flag.Rx
	I2c[I2cDevPtr].Flag.Tx .

 * -I2cHighLevel layer:
 * it's executed if the deviceXX has something to exchange
 * I2c[I2cDevPtr].Flag.Rx !=0 || I2c[I2cDevPtr].Flag.Tx!=0
 * and if the lower level routine is available:	I2cBusyFlag	= 0
 * exchanges the bytes through the lower level routine and set the I2cBusyFlag

 * -I2cLowLevel layer:
 * it's performed if the ISR has set	I2cEventFlag

	START->ADRW->	if there is something to TX, otherwise Rx
	WRITE byte[0]->...WRITE	byte[n]->
	RSTART->ADRR->
	READ byte[0]->ACK...READ byte[n]NACK->
	STOP
	FINE

 * in the main loop the different flags are checked starting from below
 * the whole cycle is repeated for each device scnning th buffers:

1-I2c event? YES -> starts I2cLowService -> EXIT
  |
  V
  NO
2-I2cBusyFlag ?	YES	-> EXIT
  |
  V
  NO
3-is there some byte TX and/or RX ? ->	YES	-> I2cHighService -> EXIT
  |
  V
  NO
4-are there more records in the I2C buffer?-> YES -> Increment I2cDevPtr-> EXIT
  |
  V
  NO
5-reset I2cDevPtr counter ->	EXIT

 Description of the FSM
1-the I2c event is communicated from SSP	through	ISR
	I2cLowService executes the I2C sequence to exchange a single byte
	reset I2C event or bus collision flag
 once exchanged all the buffer resets I2cBusyFlag	to enable upper level routine

2-I2cBusyFlag is set from I2cHighService and reset from	I2cLowService

3-The device specific routine has filled up the buffer, set the I2cHighService
    flags, initialized the I2cLowService seqEunce. This one exchanges each byte
		at each byte exchanged the counter is decreased
		when the TX counter is 0 it starts the RX
		when both counters are 0 it can start with the following device

4-All the buffer has been used, at next cycle it restarts form the first device

5-It's ready to control the next device at next cycle
 
 ------------------------------------------------------------------------------
 */