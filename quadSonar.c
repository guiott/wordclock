/* //////////////////////////////////////////////////////////////////////////////////////
** File:      QuadSonar.c
*/                                  
#define  Ver "WordClock v.1.0.0\r       by Guiott"

/**
* \mainpage WordClock.c
* \author    Guido Ottaviani-->guido@guiott.com<--
* \version 1.0.0
* \date 05/14
* \details ------------------
-------------------------------------------------------------------------------
\copyright 2014 Guido Ottaviani
guido@guiott.com

    WordClock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WordClock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WordClock.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------------------------
*/

// include standard
#include <stdlib.h>
#include <math.h>
#include <./plib/pwm.h>
#include <./plib/i2c.h>

#include "common.h"
#include "definition.h"
#include "prototypes.h"
#include "statusBits.h"


void main (void)	// ==========
{	
  
INTCONbits.GIEH=0;	// disable interrupt high
INTCONbits.GIEL=0;	// disable interrupt low


Settings(); // set ports and peripherals



// ==== enable interrupts
INTCONbits.GIEH=1;	// enable interrupt high
INTCONbits.GIEL=1;	// enable interrupt low

SetTimer0(Timer0_49ms);
T0CONbits.TMR0ON=1;     //TMR0 on
SetMux(UsIndx);
TX_FLAG=0;
I2C_POINTER_FLAG = 0;   // reset State 1B

while (1)  // main loop
{	
    if(TIMER0_FLAG)
    {
        Cycle();
        ClrWdt();
    }

    if(CAPTURE_FLAG)
    {
        Echo();
    }

    if(TX_FLAG)
    {// there is a string to transmit
        if(PIR1bits.TXIF)
        {// the TX buffer TXREG is empty
            TxChar();
        }
    }

}  // main loop
} // main ==========



/* Functions *****************************************************************/

void Uchar2Ascii(unsigned char Indx)
{/**
 *\brief compose the TXbuffer string
  *     converting the unsigned char in 3 ASCII chars
  *
 */

  unsigned char u,d,c,tmp,pos,L1,L2,Sep;
  const unsigned char Pos = 7;

	c=I2cRegTx[Indx]/100;
	tmp=(I2cRegTx[Indx]-100*c);
	d=tmp/10;
	u=tmp-10*d;

  switch(Indx)
  {
      case 0:
          L1='L';    // Left
          L2='L';    // Left
          Sep=' ';
          break;

      case 1:
          L1='L';    // Left
          L2='C';    // Center
          Sep=' ';
          break;

      case 2:
          L1='C';    // Center
          L2='L';    // Left
          Sep=' ';
          break;

      case 3:
          L1='C';    // Center
          L2='C';    // Center
          Sep=' ';
          break;

      case 4:
          L1='C';     // Center
          L2='R';     // Right
          Sep=' ';
          break;

      case 5:
          L1='R';    // Right
          L2='C';    // Center
          Sep=' ';
          break;

      case 6:
          L1='R';     // Right
          L2='R';     // Right
          Sep='\r';
          break;
  }

  pos=Indx*Pos;                     // position inside the string

  TxBuff[pos+6]=Sep;                // separator
  TxBuff[pos+5]="0123456789"[u];    // units
  TxBuff[pos+4]="0123456789"[d];    // tens
  TxBuff[pos+3]=" 123456789"[c];    // hundreds
  TxBuff[pos+2]=':';
  TxBuff[pos+1]=L2;                 // description
  TxBuff[pos+0]=L1;
}

void TxChar(void)
{/**
 *\brief load TXREG with the next byte to send
  *
 */
    TXREG = TxBuff[TxBuffIndx];
    TxBuffIndx++;
    if(TxBuffIndx>TxBuffLen)
    {
        TX_FLAG=0; // transmission is over until the next cycle
    }
}

void SetTxBuffer(void)
{/**
 *\brief fill the TX buffer with the ASCII values
  *
 */
    for(i=0;i<7;i++)
    {// fill the TXbuff with the characters to display
        Uchar2Ascii(i);
    }
    TxBuffLen=48;
    TxBuffIndx=0;
    TX_FLAG=1;
}

void Echo(void)
{/**
 *\brief an echo returned. Read the capture registers to compute echo time
 *
 */
    CAPTURE_FLAG=0;
    ECHO_FLAG = 1; // at least one echo arrived
    // Timer1 * HowManyCmPerEachTmr1Tick = Distance from 0 to 255 cm
    Dist[UsIndx] = (int)(((Ccpr2HBuff<<8) + Ccpr2LBuff) * CmTick);
    SelectUS();
}

void SelectUS(void)
{/**
 *\brief set the UsIndx in order to cycle among the four transducers
  * US Left Left:   UsIndx = 0b01
  * US Left Center: UsIndx = 0b11
  * US Right Center:UsIndx = 0b10
  * US Right Right: UsIndx = 0b00
  * http://www.guiott.com/QuadSonar/HR/LinoSonar.png
 */

    #define MIN_DIFF 3 // min dist in cm to be considered the same object
    int DistDiff;

    UsIndx++;
    if (UsIndx>3)
    {// a full cycle is over.
        UsIndx=0;

        // extrapolation of intermediate values
        if(ABS(Dist[1] - Dist[3]) < MIN_DIFF)
        {// LL - CL
            I2cRegTx[0] = 255;                  // LL
            I2cRegTx[1] = MIN(Dist[1],Dist[3]); // LC
            I2cRegTx[2] = 255;                  // CL
        }
        else
        {
            I2cRegTx[0] = Dist[1];              // LL
            I2cRegTx[1] = 255;                  // LC
            I2cRegTx[2] = Dist[3];              // CL
        }


        if(ABS(Dist[3] - Dist[2]) < MIN_DIFF)
        {// CL - CR
            I2cRegTx[2] = 255;                  // CL
            I2cRegTx[3] = MIN(Dist[3],Dist[2]); // CC
            I2cRegTx[4] = 255;                  // CR
        }
        else
        {
            I2cRegTx[2] = Dist[3];              // CL
            I2cRegTx[3] = 255;                  // CC
            I2cRegTx[4] = Dist[2];              // CR
        }


        if(ABS(Dist[2] - Dist[0]) < MIN_DIFF)
        {// CR - RR
            I2cRegTx[4] = 255;                  // CR
            I2cRegTx[5] = MIN(Dist[2],Dist[0]); // RC
            I2cRegTx[6] = 255;                  // RR
        }
        else
        {
            I2cRegTx[4] = Dist[2];              // CR
            I2cRegTx[5] = 255;                  // RC
            I2cRegTx[6] = Dist[0];              // RR
        }

        // The values can be transmitted
        SetTxBuffer();
    }
    
    SetMux(UsIndx);
}

void SetMux(int MuxPort)
{
/**
 *\brief Set the selected port on analog multiplexer
 *
*/
    MUX_DISABLE = 0;
    MUX_SELECT_LOW  = MuxPort;
    MUX_SELECT_HIGH = MuxPort >> 1;
}

void Cycle()
{/**
 *\brief set the sequence for 40KHz burst and perform the operation to measure
  *      the echo flying time
 *
 */
       TIMER0_FLAG=0;
        switch (TimerStatus)
        {
            case 0:
                PIE2bits.CCP2IE=0; //CCP2 int enabled only when needed
                if (ECHO_FLAG==0)
                {
                    Dist[UsIndx]=255; // if no echo back Dist = maximum
                    SelectUS();
                }
                else
                {
                    ECHO_FLAG = 0;
                }
                PIE2bits.CCP2IE=0;        // CCP2 int enabled only when needed
                ResetTimer1(); // reset the timer to measure the eco delay
                SetTimer0(Timer0_150us);  // 40KHz burst duration
                SetPWM(1);                // switch on the 40KHz pulse
                break;

            case 1:
                SetTimer0(Timer0_850us); // wait for the pulse decay time
                SetPWM(0);               // switch off the 40KHz pulse
                break;

            case 2:
                PIR2bits.CCP2IF = 0;      // reset of CCP2 int flag
                PIE2bits.CCP2IE=1;        // CCP2 int enabled only when needed
                SetTimer0(Timer0_49ms);   // wait for the echo before
                                          // starting a new pulse
                SetPWM(0);
                break;

            default:
                SetTimer0(Timer0_49ms);
                SetPWM(0);
                break;
        }

        TimerStatus ++;
        if(TimerStatus > 2)
        {
            TimerStatus=0;
        }
}

void SetTimer0(int Count)
{
/**
 *\brief set both High and Low registers for Timer0
 *
 */

TMR0H = Count >> 8; // byte High
TMR0L = Count;      // byte Low
}

void ResetTimer1(void)
{
/**
 *\brief Timer1 start from 0
 *
 */

TMR1H = 0; // byte High
TMR1L = 0; // byte Low
}

void SetPWM(int On)
{
    if(On==0)
    {
        SetDCPWM1(0);  // duty cycle 0%
    }
    else
    {
        SetDCPWM1(500); // duty cycle 50%
    }
}

/*===========================================================================*/

// Low priority interrupt vector

/* OLD C18 interrupt sintax >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

 #pragma code LowVector = 0x18
void InterruptVectorLow (void)
{
  _asm
    goto InterruptHandlerLow //jump to interrupt routine
  _endasm
}

//----------------------------------------------------------------------------
// Low priority interrupt routine

#pragma code
#pragma interruptlow InterruptHandlerLow



===========================================================================

IntServiceRoutine**********************************************************
void InterruptHandlerLow (void)

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

void interrupt low_priority low_isr (void)
{  
 if (PIR2bits.CCP2IF)     // CCP2 capture interrupt occurred
 {
    Ccpr2HBuff=CCPR2H;  // to avoid overwrite
    Ccpr2LBuff=CCPR2L;

    CAPTURE_FLAG = 1;
    PIR2bits.CCP2IF = 0;// reset of interrupt flag

    // only the first echo is captured.
    PIE2bits.CCP2IE=0;//The interrupt will be enabled again at the next cycle
 }
}   // Low Priority IntServiceRoutine
/*****************************************************************************/


/*===========================================================================*/
// High priority interrupt vector

/* OLD C18 interrupt sintax >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#pragma code HighVector = 0x08
void
InterruptVectorHigh (void)
{
  _asm
    goto InterruptHandlerHigh //jump to interrupt routine
  _endasm
}

//----------------------------------------------------------------------------
// High priority interrupt routine

#pragma code
#pragma interrupt InterruptHandlerHigh

===========================================================================


IntServiceRoutine**********************************************************
void InterruptHandlerHigh (void)
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

void interrupt high_isr (void)
{
 /*----The I2c ISR is a porting of assembly code for Microchip AN734-----------
 * over the I2C protocol it was added a 24Cxx EEPROM like data exchange mode.
 * The procedure is executed within the High Priority ISR in order to speed up
 * the response on I2C bus and avoid interruption, so an high care must be
 * taken on execution time and on external calls. If there are less strict
 * requirements the entire procedure can be executed in the main part after
 * the setting of a flag from the ISR.

/* SMP CKE D/A -P- -S- R/W U-A B-F
 *
 *  0   0   0   0   1   0   0   1  State 1: I2C write operation,
 *                                          last byte was an address byte*/
#define STATE1 0X09

/*  0   0   1   0   1   0   0   1  State 2: I2C write operation,
 *                                          last byte was a data byte*/
#define STATE2 0X29

/*  0   0   0   0   1   1   0   X  State 3: I2C read operation,
 *                                          last byte was an address byte*/
#define STATE3 0X0C

/*  0   0   1   0   1   1   0   0  State 4: I2C read operation,
 *                                          last byte was a data byte*/
#define STATE4 0X2C

/*  0   0   1   0   1   X   0   0  State 5: Slave I2C logic reset
 *                       CKP = 1            by NACK from master*/
#define STATE5 0X28

#define STATE_MASK 0X2D        // to mask out unimportant bits in SSPSTAT
#define BF_MASK 0X2C           // to mask out BF bit in SSPSTAT
#define RW_MASK 0X28           // to mask out R/W bit in SSPSTAT
#define MAX_TRY 100            // maximum number of retries for transmission
//----------------------------------------------------------------------------*/
unsigned char SspstatMsk;
unsigned char I2cAddr; //to perform dummy reading of the SSPBUFF

 if (INTCONbits.TMR0IF)   // timer 0 overflow?
 {
    TIMER0_FLAG = 1;
    INTCONbits.TMR0IF = 0;// reset of interrupt flag
 }

 if (PIR1bits.SSPIF)   // I2C interrupt?
 {
    SspstatMsk=SSPSTAT & STATE_MASK; //mask out unimportant bits

//State 1------------------
    if(SspstatMsk == STATE1)
    {/* After State 1 the State Machine goes in State 1B, the next received
      * byte will be the register pointer*/
        I2C_POINTER_FLAG = 1;   // go into the State 1B
        I2cAddr = SSPBUF;  //dummy reading of the buffer to empty it
        SSPCON1bits.CKP = 1; //release clock immediately to free up the bus
    }

//State 2------------------
    else if(SspstatMsk == STATE2)
    {
        if(I2C_POINTER_FLAG)
        {//come from State 1B, the received data is the register pointer
            I2cRegPtr = SSPBUF;
            SSPCON1bits.CKP = 1; //release clock immediately to free up the bus
        }
        else
        {//the received data is a valid byte to store
            I2cRegRx[I2cRegPtr] = SSPBUF;
            SSPCON1bits.CKP = 1; //release clock immediately to free up the bus
            I2cRegPtr ++;
        }

        if(I2cRegPtr >= I2C_BUFF_SIZE_RX)
        {
            I2cRegPtr = I2C_BUFF_SIZE_RX - 1;
        }
        I2C_POINTER_FLAG = 0;
    }

//State 3------------------
    else if((SspstatMsk & BF_MASK) == STATE3)
    {//first reading from master after it sends address
        I2cAddr = SSPBUF;  //dummy reading of the buffer to empty it
        for(i=0; i<MAX_TRY; i++)
        {//check MAX_TRY times if buffer is empty
            if(!SSPSTATbits.BF)
            {//if buffer is empty try to send a byte
                for(j=0; j<MAX_TRY; j++)
                {//check MAX_TRY times to avoid collisions
                    SSPCON1bits.WCOL=0; //reset collision flag
                    SSPBUF = I2cRegTx[I2cRegPtr]; //send requested byte
                    if(!SSPCON1bits.WCOL)
                    {/*if no collision, sending was OK, point to the next byte*/
                        SSPCON1bits.CKP = 1; //free up the bus
                        I2cRegPtr ++;
                        if(I2cRegPtr >= I2C_BUFF_SIZE_TX)
                        {
                            I2cRegPtr = I2C_BUFF_SIZE_TX - 1;
                        }
                        // insert here an OK flag if needed
                        goto State3End; //everything's fine. Procedure over
                    }
                }
                // insert here a fault flag if needed
                goto State3End; //too many collisions. Exit.
            }
        }
        // insert here a fault flag if needed
        // if here the buffer was never empty
        Nop();

        State3End:
        I2C_POINTER_FLAG = 0;
    }

//State 4------------------
    else if((!SSPCON1bits.CKP) && (SspstatMsk == STATE4))
    {//subsequent readings. Usually it does the same as State 3

        for(i=0; i<MAX_TRY; i++)
        {//check MAX_TRY times if buffer is empty
            if(!SSPSTATbits.BF)
            {//if buffer is empty try to send a byte
                for(j=0; j<MAX_TRY; j++)
                {//check MAX_TRY times to avoid collisions
                    SSPCON1bits.WCOL=0; //reset collision flag
                    SSPBUF = I2cRegTx[I2cRegPtr]; //send requested byte
                    if(!SSPCON1bits.WCOL)
                    {/*if no collision, sending was OK, point to the next byte*/
                        SSPCON1bits.CKP = 1; //free up the bus
                        I2cRegPtr ++;
                        if(I2cRegPtr >= I2C_BUFF_SIZE_TX)
                        {
                            I2cRegPtr = I2C_BUFF_SIZE_TX - 1;
                        }
                        // insert here an OK flag if needed
                        goto State4End; //everything's fine. Procedure over
                    }
                }
                // insert here a fault flag if needed
                goto State4End; //too many collisions. Exit.
            }
        }
        // insert here a fault flag if needed
        // if here the buffer was never empty

        State4End:
        I2C_POINTER_FLAG = 0;
    }

//State 5------------------
    else if((SspstatMsk & RW_MASK) == STATE5)
    {//end cycle
        SSPCON1bits.CKP = 1; //release clock
        I2C_POINTER_FLAG = 0;
    }

//Default------------------
    else
    {//not in a known state. A different safety action could be performed here
        SSPCON1bits.CKP = 1; //release clock
        I2C_POINTER_FLAG = 0;
    }

    SSPCON1bits.CKP = 1; //release clock
    PIR1bits.SSPIF = 0;  //Clear MSSP Interrupt flag
}

}   // High Priority IntServiceRoutine

