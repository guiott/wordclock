#include "matrix.h"
#include "common.h"
#include <math.h>

void MatrixSetting(void)
{
 
    int i;
    
    for(i=MinCol; i<=MaxCol; i++)
    {// pre-compute the bitmask in order to save time during runtime executions
        BitMask[i]=(1 << (MaxCol -i));
    }
    
    SetRow(99);  // all rows off
    SetRowOff(); // all columns off

    /* to debug
    for(i=0; i<=12; i++)
    {
        Matrix[i]=1<<(i);
    }
     to debug */
}

void ScanMatrix(void)
{
    SetRow(Row);
    SetCol(Row);
    if((++Row) > MAXROW) (Row = MINROW); // scan a column every TMR3 interrupt
}

void SetRow(unsigned char Row)
{// from 0 to 9, enable one row at a time to scan matrix
    #define ON  1
    #define OFF 0

    switch (Row)
    {
        case 0:
            Row9 = OFF;
            Row0 = ON;
            break;
            
        case 1:
            Row0 = OFF;
            Row1 = ON;
            break;
            
        case 2:
            Row1 = OFF;
            Row2 = ON;
            break;
 
        case 3:
            Row2 = OFF;
            Row3 = ON;
            break;
 
        case 4:
            Row3 = OFF;
            Row4 = ON;
            break;
 
        case 5:
            Row4 = OFF;
            Row5 = ON;
            break;
 
        case 6:
            Row5 = OFF;
            Row6 = ON;
            break;
 
        case 7:
            Row6 = OFF;
            Row7 = ON;
            break;
 
        case 8:
            Row7 = OFF;
            Row8 = ON;
            break;
 
        case 9:
            Row8 = OFF;
            Row9 = ON;
            break;
        
        default:
            SetRowOff();
            break;
    }

}

void SetColB(void)
{// pre-fill the matrix with bytes instead of bits.
 // This wastes memory but saves a lot of time during runtime matrix scan

    #define ON  1
    #define OFF 0

    char Col;
    char Row;
    unsigned int Mask;

    for(Row = MINROW; Row <= MAXROW; Row++)
    {
        for(Col = MinCol; Col <= MaxCol; Col++)
        {
            Mask=BitMask[Col] & Matrix[Row];

            if(Mask)
            {
                MatrixB[Row][MaxCol-Col] = ON;
            }
            else
            {
                MatrixB[Row][MaxCol-Col] = OFF;
            }
        }
    }
}

void SetCol(unsigned char Row)
{/* from 'A' to 'L', enable all the column to light all the LEDs interested
    the byte matrix is pre-computed when LEDs to display change (every minute)
    this saves a lot of time during 1ms column scan
  */
    ColA = MatrixB[Row][11];
    ColB = MatrixB[Row][10];
    ColC = MatrixB[Row][9];
    ColD = MatrixB[Row][8];
    ColE = MatrixB[Row][7];
    ColF = MatrixB[Row][6];
    ColG = MatrixB[Row][5];
    ColH = MatrixB[Row][4];
    ColI = MatrixB[Row][3];
    ColJ = MatrixB[Row][2];
    ColK = MatrixB[Row][1];
    ColL = MatrixB[Row][0];
}

void SetRowOff(void)
{
    Row0 = OFF;
    Row1 = OFF;
    Row2 = OFF;
    Row3 = OFF;
    Row4 = OFF;
    Row5 = OFF;
    Row6 = OFF;
    Row7 = OFF;
    Row8 = OFF;
    Row9 = OFF;
}

void WordSetting()
{// set the matrix with the words according to the new time
    int i;

    for(i=0; i<=MAXROW; i++)
    {// reset the matrix
        Matrix[i]=0;
    }

    if(Min / 30)
    {// after hal hour the word is "to" the next hour, before is "after"
        Hour++;
        Matrix[6]=0b000000011110;                   // E
    }
    else
    {
        Matrix[7]=0b100000000000;                   // MENO
    }

    switch(Min / 5)
    {
        case 1:
        case 11:
            Matrix[8] = Matrix[8] | 0b000001111110;// CINQUE
            break;

        case 2:
        case 10:
            Matrix[9] = Matrix[9] | 0b111100000000;// DIECI
            break;

        case 3:
        case 9:
            Matrix[7] = Matrix[7] | 0b001101111110;// UN QUARTO
            break;

        case 4:
        case 8:
            Matrix[8] = Matrix[8] | 0b111110000000;// VENTI
            break;

        case 5:
        case 7:
            Matrix[8] = Matrix[8] | 0b111111111110;// VENTICINQUE
            break;

        case 6:
            Matrix[9] = Matrix[9] | 0b000000111110;// MEZZA
            break;

        default:
            break;
    }

    switch(Min % 5)
    {
        case 1:
            Matrix[0] = Matrix[0] | 0b000000000001;// first point

        case 2:
            Matrix[1] = Matrix[1] | 0b000000000001;// add second point

        case 3:
            Matrix[2] = Matrix[2] | 0b000000000001;// add third point

        case 4:
            Matrix[3] = Matrix[3] | 0b000000000001;// add fourth point
            break;

        default:
            break;
    }

    Hour = Hour % 12;

    if(Hour == 1)
    {
        Matrix[1] = Matrix[1] | 0b101000000000;// E' L'
    }
    else
    {
        Matrix[0] = Matrix[0] | 0b111101101110;// SONO LE ORE
    }

    switch(Hour)
    {
        case 1:
            Matrix[1] = Matrix[1] | 0b000111000000;// UNA
            break;

        case 2:
            Matrix[1] = Matrix[1] | 0b000000011000;// DUE
            break;

        case 3:
            Matrix[2] = Matrix[2] | 0b111000000000;// TRE
            break;

        case 4:
            Matrix[5] = Matrix[5] | 0b111111100000;// QUATTRO
            break;

        case 5:
            Matrix[6] = Matrix[6] | 0b111111000000;// CINQUE
            break;

        case 6:
            Matrix[5] = Matrix[5] | 0b000000001110;// SEI
            break;

        case 7:
            Matrix[4] = Matrix[4] | 0b000000111110;// SETTE
            break;

        case 8:
            Matrix[2] = Matrix[2] | 0b000111100000;// OTTO
            break;

        case 9:
            Matrix[2] = Matrix[2] | 0b000000011110;// NOVE
            break;

        case 10:
            Matrix[3] = Matrix[3] | 0b111110000000;// DIECI
            break;

        case 11:
            Matrix[3] = Matrix[3] | 0b000001111110;// UNDICI
            break;

        case 0:
            Matrix[4] = Matrix[4] | 0b111111000000;// DODICI
            break;

        default:
            break;
    }

}