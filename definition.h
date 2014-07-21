/*
 * File:   definition.h
 * Author: guiott
 *
 * Created on 12 maggio 2014
 */

#ifndef definition_H
#define definition_H

struct Bits
{// to use as flags
    unsigned bit0:1;
    unsigned bit1:1;
    unsigned bit2:1;
    unsigned bit3:1;
    unsigned bit4:1;
    unsigned bit5:1;
    unsigned bit6:1;
    unsigned bit7:1;
};

struct Bits VARbits1;

//----- Variables
int i = 0; // contatore generico
int j = 0; // contatore generico
char TimerStatus = 0;

unsigned int Timer0_ms;
unsigned int Timer3_ms;
extern const int DutyTab[];
extern unsigned char DutyCycle;

unsigned int OneHzTick;

extern unsigned int RXerr;
#define MAX_RX_BUFFER 64                    // RX buffer size
extern unsigned char RXbuff[MAX_RX_BUFFER]; // RX buffer
extern unsigned char RxBuffIndx;            // RX buffer pointer
extern unsigned char RxFlag;                // RX procedure status
extern unsigned char TxBuff[];
extern unsigned char TxBuffLen;
extern unsigned char TxBuffIndx;
extern unsigned int Min;

#define  LED LATBbits.LATB5

unsigned int PrevMin;       // to compare the time elapsed

#define MAX_LIGHT  255
#define MIN_LIGHT  0

unsigned int LightIndx;//duty cycle multiplier

#endif