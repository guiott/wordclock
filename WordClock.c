/* ////////////////////////////////////////////////////////////////////////////
** File:      WordClock.c
*/                                  
#define  Ver "WordClock v.0.0.8\r       by Guiott"

/**
* \mainpage WordClock.c
* \author    Guido Ottaviani-->guido@guiott.com<--
* \version 0.0.8
* \date 07/14
* \details this is a study for the realization of a  somehow unusual word clock.
 * It's more a study for some special HW and SW techniques than a real
 * application.
 * It uses a 10 x 11 matrix led display and not fixed words leds.
 * The matrix is refreshed by the PIC18F4620 I/O pins through a series of high
 * and low side power drivers.
 * The time is maintained by a self powered I2C RTC.
 * The time is syncronized with INRIM via a WiFly internet connection.
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
#include <plib/usart.h>
#include <string.h>

#include "common.h"
#include "definition.h"
#include "prototypes.h"
#include "statusBits.h"
#include "comm.h"

void main (void)	
{	
INTCONbits.GIEH=0;	// disable interrupt high
INTCONbits.GIEL=0;	// disable interrupt low

Settings();    // set ports and peripherals
I2cSettings(); // set I2C ports and peripherals

// ==== enable interrupts
INTCONbits.GIEH=1;	// enable interrupt high
INTCONbits.GIEL=1;	// enable interrupt low

SetTimer3(Timer3_ms);
T3CONbits.TMR3ON=1; 
SQW_FLAG=0;

// RtcInit();           //?????????????????????????????????????????????????????????????????
CommSetting();
MatrixSetting();
InterruptSettings();
DutyCycle = 10;     // preset LED PWM to 50%
    
while (1)  // main loop
{    
    if(SQW_FLAG)
    {// 1Hz interrupt from RTC
        OneHzTick ++;
        if(!(OneHzTick % 900))
        {//every 60*15 = 900 seconds read Time Server via WiFly and update RTC
            StartCommFsmSched(ReadTimeFsm); //read time from http://www.inrim.it
            RtcWriteTime();
        }
        else if(I2cBuffChk(RTC_PTR))
        {// if previous operations are over, start a new one
            I2cData(RTC_PTR,1,0,0,0,0,3); //read RTC,3 bytes,starting from reg 0
        }

        SQW_FLAG=0; // I2C reading procedure from RTC has been started
    }
    else if(I2c[RTC_PTR].Done == 1)
    {// I2C operation terminated. Data have been read from RTC
        RtcReadTime();
        I2c[RTC_PTR].Done = 0;      // data used. Wait for next reading

        if(Min != PrevMin)
        {// if time changed set a new Matrix
            WordSetting();
            PrevMin = Min;
        }
    }
    
    if(CommFsmFlag)
    {// execute the Communication Finite State Machine
        CommFsmSched(FsmStructActive);
    }

    I2cService(); // controls if I2C needs service

   // add A/D peripheral to read ambient light and set DutyCycle ???????????????????????????????????????????

}  // main loop
} // main ==========



/* Functions *****************************************************************/

void SetTimer0(int Count)
{
/**
 *\brief set both High and Low registers for Timer0
 *
 */

TMR0H = Count >> 8; // byte High
TMR0L = Count;      // byte Low
}

void SetTimer1(unsigned char DutyCicle)
{
//set both High and Low registers for Timer1
int Count = DutyTab[DutyCycle];
TMR1H = Count >> 8; // byte High
TMR1L = Count;      // byte Low
T1CONbits.TMR1ON=1;
}

void SetTimer3(int Count)
{
/**
 *\brief set both High and Low registers for Timer3
 *
 */

TMR3H = Count >> 8; // byte High
TMR3L = Count;      // byte Low
}


/*===========================================================================*/
// Low priority interrupt vector
void interrupt low_priority low_isr (void)
{  
    if (PIR1bits.RCIF)                  // RX buffer full
    {//cleared when RCREG is read
        if (RxBuffIndx >= MAX_RX_BUFFER)
        {// buffer overflow
          RXerr = 3;
          RxFlag	= 1;                // RX over
          INTCONbits.TMR0IE=0;          // timeout interrupt TMR0 OFF
          T0CONbits.TMR0ON=0;           //TMR0 off
        }
        else
        {
            RXbuff[RxBuffIndx] = ReadUSART();
            if(RCSTAbits.OERR || RCSTAbits.FERR)
            {// Overrun or Frame error
                RXerr = 1;
                RxFlag	= 1;            // RX over
                INTCONbits.TMR0IE=0;    // timeout interrupt TMR0 OFF
                T0CONbits.TMR0ON=0;     //TMR0 off
            }
            else
            {// one correct char received
                if(strstr(RXbuff, FsmStructActive[FsmIndx].Str))
                {// terminator received
                    PIE1bits.RCIE=0; //disable RX interrupts
                    RxFsmFlag=1;     // call the second phase of active FSM step
                    CommFsmFlag=1;
                }
                else
                {
                    RxBuffIndx ++;
                    SetTimer0(Timer0_ms);   // 1,5s timeout
                    INTCONbits.TMR0IF = 0;  // reset interrupt flag
                    INTCONbits.TMR0IE=1;    // RX timeout  ON
                    T0CONbits.TMR0ON=1;     //TMR0 on
                }
            }
        }
    }

    if (PIR1bits.TXIF) // TX buffer empty. Cleared when TXREG is written
    {// load TXREG with the next byte to send
        TXREG = TxBuff[TxBuffIndx];
        TxBuffIndx++;
        if(TxBuffIndx>=TxBuffLen)
        {
            TXSTAbits.TXEN=0; //disable the transmitter
            PIE1bits.TXIE=0; //disable TX interrupts
            TxFsmFlag=1;     // call the second phase of active FSM step
            CommFsmFlag=1;
        }
    }

    if (INTCONbits.TMR0IF)          
    {// TMR0 overflow = RX timeout
         INTCONbits.TMR0IE=0;           // timeout interrupt TMR0 OFF
         INTCONbits.TMR0IF=0;           // reset flag di interrupt
         RXerr = 2;                     // RX timeout error
         RxFlag	= 1;                    // RX over
    }
}

/*===========================================================================*/
// High priority interrupt vector
void interrupt high_isr (void)
{
 if (PIR2bits.TMR3IF)   // timer 3 overflow?
 {// used for LED matrix scan
    PIR2bits.TMR3IF = 0;    // reset of interrupt flag
    SetTimer3(Timer3_ms);
    SetTimer1(DutyCycle);   // set Timer 1 in order to dimm LED light
    ScanMatrix();
    ClrWdt();
 }

 if (PIR1bits.TMR1IF)       // timer 1 overflow?
 {// used for PWM LED light dimming
    SetRowOff();            // switch off all rows
    T1CONbits.TMR1ON=0;     //TMR1 off. It will be enabled at next cycle
    PIR1bits.TMR1IF = 0;    // reset of interrupt flag
 }

 if (INTCONbits.INT0IF)     // External interrupt on iNT0 -> RTC 1Hz clock
 {
    LED ^= 1;               // blink yellow led at 0.5Hz
    SQW_FLAG=1;
    INTCONbits.INT0IF = 0;  // reset of interrupt flag
 }

if (PIR1bits.SSPIF)         // an I2C event is over
{
    PIR1bits.SSPIF = 0;     // reset I2c interrupt flag
    I2cEventFlag = 1;       // I2cLowService will be executed
}

if (PIR2bits.BCLIF)         // I2c bus collision
{
    PIR2bits.BCLIF = 0;     // interrupt flag reset
    I2cEventFlag = 1;       // execute I2cLowService
    I2cBusCollFlag = 1;     // a collision occurred
}

}   