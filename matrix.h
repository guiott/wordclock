/* 
 * File:   Matrix.h
 * Author: guiott
 *
 * Created on 19 luglio 2014, 13.48
 */

#ifndef MATRIX_H
#define	MATRIX_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

typedef union
{// to temporary store the PORTx bits
    struct
    {
        unsigned P0                  :1;
        unsigned P1                  :1;
        unsigned P2                  :1;
        unsigned P3                  :1;
        unsigned P4                  :1;
        unsigned P5                  :1;
        unsigned P6                  :1;
        unsigned P7                  :1;
    };
    struct
    {
        unsigned char Port;
    };
} PORTbits_t;
volatile PORTbits_t Dbits;
volatile PORTbits_t Ebits;
volatile PORTbits_t Cbits;

/*
     * RA1 = Row 0
     * RA2 = Row 1
     * RA3 = Row 2
     * RA4 = Row 3
     * RA5 = Row 4
     * RC0 = Row 5
     * RC1 = Row 6
     * RB1 = Row 7
     * RB2 = Row 8
     * RB3 = Row 9
*/
#define Row0  LATAbits.LATA1
#define Row1  LATAbits.LATA2
#define Row2  LATAbits.LATA3
#define Row3  LATAbits.LATA4
#define Row4  LATAbits.LATA5
#define Row5  LATCbits.LATC0
#define Row6  LATCbits.LATC1
#define Row7  LATBbits.LATB1
#define Row8  LATBbits.LATB2
#define Row9  LATBbits.LATB3

/*
     * RD0 = Column A
     * RD1 = Column B
     * RD2 = Column C
     * RD3 = Column D
     * RD4 = Column E
     * RD5 = Column F
     * RD6 = Column G
     * RD7 = Column H
     * RE0 = Column I
     * RE1 = Column J
     * RE2 = Column K
     * RC5 = Column L
 
#define ColA  LATDbits.LATD0
#define ColB  LATDbits.LATD1
#define ColC  LATDbits.LATD2
#define ColD  LATDbits.LATD3
#define ColE  LATDbits.LATD4
#define ColF  LATDbits.LATD5
#define ColG  LATDbits.LATD6
#define ColH  LATDbits.LATD7
#define ColI  LATEbits.LATE0
#define ColJ  LATEbits.LATE1
#define ColK  LATEbits.LATE2
#define ColL  LATCbits.LATC5
*/

#define ColA  Dbits.P0
#define ColB  Dbits.P1
#define ColC  Dbits.P2
#define ColD  Dbits.P3
#define ColE  Dbits.P4
#define ColF  Dbits.P5
#define ColG  Dbits.P6
#define ColH  Dbits.P7
#define ColI  Ebits.P0
#define ColJ  Ebits.P1
#define ColK  Ebits.P2
#define ColL  Cbits.P5

#define MINCOL 'A'
#define MAXCOL 'L'
const unsigned char MinCol= MINCOL -65;
const unsigned char MaxCol= MAXCOL -65;


#define MINROW 0
#define MAXROW 9

#define ON  1
#define OFF 0

void SetRow(char Row);
void SetCol(char Row);
void SetRowOff(void);
void SetColOff(void);
void MatrixSetting(void);
void SetColB(void);
void WordSetting();
void TestMatrix();
void TestCol(void);
void LongDelay(int msX10);
void WriteMatrixChar(unsigned char Ch1, unsigned char Ch2, int Pos1, int Pos2);

volatile int Matrix[MAXROW+1];
volatile int MatrixB[MAXROW+1][MAXCOL -64];    // use bytes instead of bits to save time
volatile unsigned int BitMask[MAXCOL -64];

unsigned char DutyCycle;
const int DutyTab[]=
{// pre-computed TMR1 values to obtain LED dimming
    65535,  // 0%
    65472,  // 5%
    65410,  // 10%
    65347,  // 15%
    65285,  // 20%
    65222,  // 25%
    65160,  // 30%
    65097,  // 35%
    65035,  // 40%
    64972,  // 45%
    64910,  // 50%
    64847,  // 55%
    64785,  // 60%
    64722,  // 65%
    64660,  // 70%
    64597,  // 75%
    64535,  // 80%
    64472,  // 85%
    64410,  // 90%
    64347,  // 95%
    64297// 99%  to avoid overlap with TMR3 cycle
};

extern unsigned int Sec;
extern unsigned int Min;
extern unsigned int Hour;
extern unsigned char TimeSync;

unsigned int TestTime = 250;

#endif	/* MATRIX_H */

