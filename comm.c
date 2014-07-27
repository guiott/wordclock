#include <p18cxxx.h>
#include "comm.h"
#include "common.h"

char (*CommFsmState )(void) = &CommFsmIdle;

void CommSetting(void)
{
    //-------USART
    /*Baud Rate
    FOSC/[4*(BRG + 1)]
    SPBRGH:SBPRG   FOSC   des. bps  Error
    0x0056         40MHz	115200    0,22%
    0x00AD         40MHz	57600     0,22%
    0x0129         40MHz	33600     0,13%
    0x0208         40MHz	19200     0,03%
    0x0411         40MHz	9600     	0,03%
    0x0822         40MHz	4800     -0,02%
     *
     *
    SPBRGH:SBPRG   FOSC   des. bps	Error
    0x002A         20MHz	115200   -0,94%
    0x0056         20MHz	57600     0,22%
    0x0094         20MHz	33600     0,13%
    0x0103         20MHz	19200    -0,16%
    0x0208         20MHz	9600      0,03%
    0x0411         20MHz	4800      0,03%
     */
    TXSTAbits.SYNC=0;   //asynchronous
    BAUDCONbits.BRG16=1;//16 bit Baud Rate Generator
    TXSTAbits.BRGH=1;   //High baud rate

    #ifdef PLL //	40MhZ
      SPBRGH=0x04;
      SPBRG =0x11;     //	9600	@	40MHz
    #else
      SPBRGH=0x02;
      SPBRG =0x08;     //	9600	@	20MHz
    #endif

    RCSTAbits.SPEN=1;   //enable serial port pins
    RCSTAbits.CREN=1;   //enable reception
    RCSTAbits.SREN=0;   //no effect
    TXSTAbits.TX9=0;    //8-bit transmission
    RCSTAbits.RX9=0;    //8-bit reception
    TXSTAbits.TXEN=1;   //enable the transmitter
    IPR1bits.TXIP=0;    //TX interrupt low priority
    PIE1bits.TXIE=0;    //disable TX interrupts

    IPR1bits.RCIP=0;    //RX interrupt low priority
    PIE1bits.RCIE=0;    //disable RX interrupts
}

char CommFsmIdle(void)
{// do nothing
        return '0' ;
}

char CommFsmClear(void)
{// reset RX buffer
    memset(RXbuff,'\0',MAX_RX_BUFFER);
        return '0' ;
}

char CommFsmWriteRTC(void)
{// write time just read from INRIM to RTC
    RtcWriteTime();
    TimeSync=0; // time has been synchronized
        return '0' ;
}

void StartCommFsmSched(struct FsmTable * FsmStruct)
{// initialize the FSM
    FsmIndx = 0;
    CommFsmFlag = 1;        // kick off the FSM
    TxFsmFlag = 0;
    RxFsmFlag = 0;
    FsmStructActive = FsmStruct;
    CommFsmDoneFlag = 1;    // the FSM is started
}

void CommFsmSched(struct FsmTable * FsmStruct)
{//Communication Finite State Machine
    unsigned char FsmStat;

    FsmStat = FsmStruct[FsmIndx].Stat;
    
    switch (FsmStat)
	  {
        case FsmRx:
          if(RxFsmFlag==0)
          {// the first time is called
              StartRx(FsmStruct[FsmIndx].Str);
          }
          else
          {// after terminator string has been received
              // execute the callback function
              CommFsmState = (* FsmStruct[FsmIndx].pCallback);
              (* CommFsmState) () ;
              FsmIndx++;
              RxFsmFlag = 0;
          }
          break;

        case FsmTx:
          // transmit the string and then call the callback function
             
          if(TxFsmFlag==0)
          {// the first time is called
              StartTx(FsmStruct[FsmIndx].Str);
          }
          else
          {// after all the buffer is transmitted
              // execute the final function
              CommFsmState = (* FsmStruct[FsmIndx].pCallback);
              (* CommFsmState) () ;
              FsmIndx++;
              TxFsmFlag = 0;
          }
          
          break;

        case FsmDo:
          CommFsmState = (* FsmStruct[FsmIndx].pCallback);
          (* CommFsmState) () ;
          FsmIndx++;
          break;


        case FsmEnd:
          CommFsmState = (* FsmStruct[FsmIndx].pCallback);
          (* CommFsmState) () ;
          CommFsmFlag = 0;
          CommFsmDoneFlag = 0;    // the FSM is over
          break;

        default:
          CommFsmFlag = 0;
          break;
	  }
}

void StartTx(const unsigned char * TxStr )
{// Fill the TX buffer with string to transmit
    CommFsmFlag = 0;  // stop the FSM untile the full buffer has been sent
    TxBuffLen = strlen(TxStr);
    TxBuffIndx=0;
    memcpy(TxBuff, TxStr, TxBuffLen);
    // TXSTAbits.TXEN=1;   //enable the transmitter
    PIE1bits.TXIE=1;    //enable TX interrupts
}

void StartRx(const unsigned char * RxStr )
{
    CommFsmFlag = 0;  // stop the FSM untile the full buffer has been received
    RxBuffIndx=0;
    PIE1bits.RCIE=1;  // enable RX interrupts
}
