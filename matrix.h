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
 */
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

#define MINCOL 'A'
#define MAXCOL 'L'
const unsigned char MinCol= MINCOL -65;
const unsigned char MaxCol= MAXCOL -65;


#define MINROW 0
#define MAXROW 9

void SetRow(char Row);
void SetCol(char Row);
void SetRowOff(void);
void MatrixSetting(void);
void SetColB(void);
int Matrix[MAXROW+1];
int MatrixB[MAXROW+1][MAXCOL -64];    // use bytes instead of bits to save time
unsigned int BitMask[MAXCOL -64];
unsigned char Row = MINROW;           // current row ON

unsigned char DutyCycle;
const int DutyTab[]=
{// pre-computed TMR1 values to obtain LED dimming with duty cycles from
    65535,          // 0%
    65035,          // 5%
    64535,          // 10%
    64035,          // 15%
    63535,          // 20%
    63035,          // 25%
    62535,          // 30%
    62035,          // 35%
    61535,          // 40%
    61035,          // 45%
    60535,          // 50%
    60035,          // 55%
    59535,          // 60%
    59035,          // 65%
    58535,          // 70%
    58035,          // 75%
    57535,          // 80%
    57035,          // 85%
    56535,          // 90%
    56035,          // 95%
    55635           // 99%  to avoid overlap with TMR3 cycle
};
#endif	/* MATRIX_H */

