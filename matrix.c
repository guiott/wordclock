#include "matrix.h"
#include "common.h"
#include <math.h>

void MatrixSetting(void)
{
 
    int i;
    
    for(i=MinCol; i<=MaxCol; i++)
    {// pre-compute the bitmask in order to save time during runtime executions
        BitMask[i]=(1 << (MaxCol - i));
    }
    /* to debug
    for(i=0; i<=12; i++)
    {
        Matrix[i]=1<<(i);
    }
     to debug */
}

void ScanMatrix(void)
{
    static unsigned char Row = MINROW;   // current row ON
    SetColOff();
    SetRowOff();
    SetRow(Row);
    SetCol(Row);
    if((++Row) > MAXROW) (Row = MINROW); // scan a column every TMR3 interrupt
}

void SetRow(unsigned char Row)
{// from 0 to 9, enable one row at a time to scan matrix
    switch (Row)
    {
        case 0:
            Row0 = ON;
            break;
            
        case 1:
            Row1 = ON;
            break;
            
        case 2:
            Row2 = ON;
            break;
 
        case 3:
            Row3 = ON;
            break;
 
        case 4:
            Row4 = ON;
            break;
 
        case 5:
            Row5 = ON;
            break;
 
        case 6:
            Row6 = ON;
            break;
 
        case 7:
            Row7 = ON;
            break;
 
        case 8:
            Row8 = ON;
            break;
 
        case 9:
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

     * RD0 = Column A
     * RD1 = Column B
     * RD2 = Column C
     * RD3 = Column D
     * RD4 = Column E
     * RD5 = Column F
     * RD6 = Column G
     * RD7 = Column H
  *
     * RE0 = Column I
     * RE1 = Column J
     * RE2 = Column K
  *
     * RC5 = Column L
  */

    ColA = MatrixB[Row][11];
    ColB = MatrixB[Row][10];
    ColC = MatrixB[Row][9];
    ColD = MatrixB[Row][8];
    ColE = MatrixB[Row][7];
    ColF = MatrixB[Row][6];
    ColG = MatrixB[Row][5];
    ColH = MatrixB[Row][4];

    LATD = Dbits.Port;

    ColI = MatrixB[Row][3];
    ColJ = MatrixB[Row][2];
    ColK = MatrixB[Row][1];

    LATE = Ebits.Port;

    ColL = MatrixB[Row][0];

    LATC = PORTC | Cbits.Port;
}

void SetColOff(void)
{
    LATD = 0;
    LATE = 0;
    LATC = PORTC & 0b11011111;
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

    if(Min >= 5)    // nothing if "o'clock"
    {
        if(Min < 35)
        {// after half hour the word is "to" the next hour, before is "after"
            Matrix[7]=0b100000000000;               // E
        }
        else
        {
            Hour++;
            Matrix[6]=0b000000011110;               // MENO
        }
    }
    else
    {
    Matrix[6]=0b000000000000;
    Matrix[7]=0b000000000000;
    }

    switch(Min / 5)
    {
        case 1:
        case 11:
            Matrix[8] = Matrix[8] | 0b000001111110;// CINQUE
            break;

        case 2:
        case 10:
            Matrix[9] = Matrix[9] | 0b111110000000;// DIECI
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

    if(!TimeSync)// light up points only if RTC time is in sync with INRIM
    {
        switch(Min % 5)
        {
            case 1:
                Matrix[0] = Matrix[0] | 0b000000000001;// first point
                break;

            case 2:
                Matrix[0] = Matrix[0] | 0b000000000001;// first point
                Matrix[1] = Matrix[1] | 0b000000000001;// add second point
                break;

            case 3:
                Matrix[0] = Matrix[0] | 0b000000000001;// first point
                Matrix[1] = Matrix[1] | 0b000000000001;// add second point
                Matrix[2] = Matrix[2] | 0b000000000001;// add third point
                break;

            case 4:
                Matrix[0] = Matrix[0] | 0b000000000001;// first point
                Matrix[1] = Matrix[1] | 0b000000000001;// add second point
                Matrix[2] = Matrix[2] | 0b000000000001;// add third point
                Matrix[3] = Matrix[3] | 0b000000000001;// add fourth point
                break;

            default:
                break;
        }
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
            Matrix[1] = Matrix[1] | 0b000000011100;// DUE
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

    SetColB();  
}

void TestMatrix()
{
    static unsigned char Xcol;
    static unsigned char Yrow;
    int i;
    static int j = 0;
    static int k = 0;

   
    for(i=MINROW; i<=MAXROW; i++)
    {// reset the matrix
        Matrix[i]=0;
    }
  
    switch(j)
    {
        case 0: // char display
            WriteMatrixChar('G', 'O', 7, 1);
            TestTime = 5000;
            j++;
            break;

        case 1: // dot by dot
            Matrix[Yrow]=0b0000100000000000 >> Xcol;
            if((++Xcol)>MaxCol)
            {
                Xcol = MinCol;
                if((++Yrow)>MAXROW)
                {
                    Yrow=MINROW;
                    j++;
                }
            }
            TestTime = 100;
            break;

        case 2: // row by row
            Matrix[Yrow]=0b0000111111111111;
            if((++Yrow)>MAXROW)
            {
                Yrow=MINROW;
                j++;
            }
            TestTime = 300;
            break;

        case 3: //column by column
            Matrix[0]=0b0000100000000000 >> Xcol;
            Matrix[1]=0b0000100000000000 >> Xcol;
            Matrix[2]=0b0000100000000000 >> Xcol;
            Matrix[3]=0b0000100000000000 >> Xcol;
            Matrix[4]=0b0000100000000000 >> Xcol;
            Matrix[5]=0b0000100000000000 >> Xcol;
            Matrix[6]=0b0000100000000000 >> Xcol;
            Matrix[7]=0b0000100000000000 >> Xcol;
            Matrix[8]=0b0000100000000000 >> Xcol;
            Matrix[9]=0b0000100000000000 >> Xcol;
            if((++Xcol)>MaxCol)
            {
                Xcol = MinCol;
                j++;
            }
            TestTime = 300;
            break;

        case 4: //diagonal left
            Matrix[0]=0b0000100000000000 >> Xcol;
            Matrix[1]=0b0000010000000000 >> Xcol;
            Matrix[2]=0b0000001000000000 >> Xcol;
            Matrix[3]=0b0000000100000000 >> Xcol;
            Matrix[4]=0b0000000010000000 >> Xcol;
            Matrix[5]=0b0000000001000000 >> Xcol;
            Matrix[6]=0b0000000000100000 >> Xcol;
            Matrix[7]=0b0000000000010000 >> Xcol;
            Matrix[8]=0b1000000000001000 >> Xcol;
            Matrix[9]=0b0100000000000100 >> Xcol;
            if((++Xcol)>MaxCol)
            {
                Xcol = MinCol;
                j++;
            }
            TestTime = 300;
            break;

        case 5: //diagonal right
            Matrix[0]=0b0000000000000100 << Xcol;
            Matrix[1]=0b0000000000001000 << Xcol;
            Matrix[2]=0b0000000000010000 << Xcol;
            Matrix[3]=0b0000000000100000 << Xcol;
            Matrix[4]=0b0000000001000000 << Xcol;
            Matrix[5]=0b0000000010000000 << Xcol;
            Matrix[6]=0b0000000100000000 << Xcol;
            Matrix[7]=0b0000001000000000 << Xcol;
            Matrix[8]=0b0000010000000001 << Xcol;
            Matrix[9]=0b0000100000000010 << Xcol;
            if((++Xcol)>MaxCol)
            {
                Xcol = MinCol;
                j++;
            }
            TestTime = 300;
            break;

       case 6: //all LEDs on
           if(k%2)
           {
                Matrix[0]=0XFFFF;
                Matrix[1]=0XFFFF;
                Matrix[2]=0XFFFF;
                Matrix[3]=0XFFFF;
                Matrix[4]=0XFFFF;
                Matrix[5]=0XFFFF;
                Matrix[6]=0XFFFF;
                Matrix[7]=0XFFFF;
                Matrix[8]=0XFFFF;
                Matrix[9]=0XFFFF;
           }
           else
           {
                Matrix[0]=0;
                Matrix[1]=0;
                Matrix[2]=0;
                Matrix[3]=0;
                Matrix[4]=0;
                Matrix[5]=0;
                Matrix[6]=0;
                Matrix[7]=0;
                Matrix[8]=0;
                Matrix[9]=0;
           }

            if((++k)>8)
            {
                k = 0;
                j++;
            }
            TestTime = 1000;
            break;

        default:
            j=0;
            TestTime = 300;
            break;
    }

    /*
    TestTime = 1000;
    //              ABCDEFGHIJKL
    Matrix[0]=0b0000010101010101;
    Matrix[1]=0b0000010101010101;
    Matrix[2]=0b0000010101010101;
    Matrix[3]=0b0000010101010101;
    Matrix[4]=0b0000010101010101;
    Matrix[5]=0b0000010101010101;
    Matrix[6]=0b0000010101010101;
    Matrix[7]=0b0000010101010101;
    Matrix[8]=0b0000010101010101;
    Matrix[9]=0b0000010101010101;
    */

    SetColB();
}

void TestLed(void)
{
    Row9=0;
    Row0=1;
    TestCol();
    Row0=0;
    Row1=1;
    TestCol();
    Row1=0;
    Row2=1;
    TestCol();
    Row2=0;
    Row3=1;
    TestCol();
    Row3=0;
    Row4=1;
    TestCol();
    Row4=0;
    Row5=1;
    TestCol();
    Row5=0;
    Row6=1;
    TestCol();
    Row6=0;
    Row7=1;
    TestCol();
    Row7=0;
    Row8=1;
    TestCol();
    Row8=0;
    Row9=1;
    TestCol();
}

void TestCol(void)
{
    int Delay=5;
    ColL=0;
    ColA=1;
    LongDelay(Delay);
    ColA=0;
    ColB=1;
    LongDelay(Delay);
    ColB=0;
    ColC=1;
    LongDelay(Delay);
    ColC=0;
    ColD=1;
    LongDelay(Delay);
    ColD=0;
    ColE=1;
    LongDelay(Delay);
    ColE=0;
    ColF=1;
    LongDelay(Delay);
    ColF=0;
    ColG=1;
    LongDelay(Delay);
    ColG=0;
    ColH=1;
    LongDelay(Delay);
    ColH=0;
    ColI=1;
    LongDelay(Delay);
    ColI=0;
    ColJ=1;
    LongDelay(Delay);
    ColJ=0;
    ColK=1;
    LongDelay(Delay);
    ColK=0;
    ColL=1;
    LongDelay(Delay);
}

void LongDelay(int msX10)
{
    int i;
    for(i=0;i<msX10;i++)
    {
        __delay_ms(10);
    }
}