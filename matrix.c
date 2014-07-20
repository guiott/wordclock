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