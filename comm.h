/* 
 * File:   comm.h
 * Author: guiott
 *
 * Created on 2 luglio 2014, 15.22
 */

#ifndef COMM_H
#define	COMM_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#include "prototypes.h"
#include <string.h>
#include "common.h"

unsigned char RxFlag;         // RX procedure status
unsigned char CommFsmFlag=0;  // FSM active flag
unsigned char TxFsmFlag;      // Tx FSM phase flag
unsigned char RxFsmFlag = 0;  // Rx FSM phase flag

typedef char (*FsmCallbackFunc)(void);   // pointer to function
char CommFsmIdle(void);
char CommFsmClear(void);
char CommFsmWriteRTC(void);
void CommSetting(void);
void StartTx(const unsigned char * TxStr );
void StartRx(const unsigned char * RxStr );
void CommFsmSched(struct FsmTable * FsmStruct);
void StartCommFsmSched(struct FsmTable * FsmStruct);

unsigned char TxBuff[32];
unsigned char TxBuffLen;
unsigned char TxBuffIndx;
unsigned int RXerr;
unsigned char RXbuff[MAX_RX_BUFFER];	// RX buffer
unsigned char RxBuffIndx;               // RX buffer pointer
unsigned char FsmIndx;
struct FsmTable * FsmStructActive;

struct FsmTable
{// Scheduler for Finite State Machine
    unsigned char Stat;         // status index
    const unsigned char * Str;  // string to send or receive
    FsmCallbackFunc pCallback;  // function to call after RX or TX is over
};

enum Fsm{FsmRx, FsmTx, FsmDo, FsmEnd};

struct FsmTable ReadTimeFsm[] =
{/* scheduler used for comm protocol with WiFLy
  this reads time from http://www.inrim.it
 */
    /*
    {FsmTx, "exit\r\n", (FsmCallbackFunc)CommFsmIdle},  ????????????????????????????? add procedure to exit from command mode ??????????
    {FsmRx, "EXIT", (FsmCallbackFunc)CommFsmIdle},
    {FsmEnd, "", (FsmCallbackFunc)CommFsmIdle}
    */

    {FsmTx, "$$$", (FsmCallbackFunc)CommFsmIdle},
    {FsmRx, "CMD", (FsmCallbackFunc)CommFsmIdle},
    {FsmTx, "open\r\n", (FsmCallbackFunc)CommFsmIdle},
    {FsmRx, "*CLOS*", (FsmCallbackFunc)TimeDecode},
    {FsmDo, "", (FsmCallbackFunc)CommFsmWriteRTC},
    {FsmEnd, "", (FsmCallbackFunc)CommFsmClear}
};

#endif	/* COMM_H */
