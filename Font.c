#include "Font8x5.h"
#include "matrix.h"

void WriteMatrixChar(unsigned char Ch1, unsigned char Ch2, int Pos1, int Pos2)
{//write a char in a position from 1 (most right) to 11 (most left)
 // 0,0 = top right
    
    Matrix[1]=Font8x5[Ch1][0]<<Pos1;
    Matrix[2]=Font8x5[Ch1][1]<<Pos1;
    Matrix[3]=Font8x5[Ch1][2]<<Pos1;
    Matrix[4]=Font8x5[Ch1][3]<<Pos1;
    Matrix[5]=Font8x5[Ch1][4]<<Pos1;
    Matrix[6]=Font8x5[Ch1][5]<<Pos1;
    Matrix[7]=Font8x5[Ch1][6]<<Pos1;
    Matrix[8]=Font8x5[Ch1][7]<<Pos1;

    Matrix[1]=Matrix[1] | (Font8x5[Ch2][0]<<Pos2);
    Matrix[2]=Matrix[2] | (Font8x5[Ch2][1]<<Pos2);
    Matrix[3]=Matrix[3] | (Font8x5[Ch2][2]<<Pos2);
    Matrix[4]=Matrix[4] | (Font8x5[Ch2][3]<<Pos2);
    Matrix[5]=Matrix[5] | (Font8x5[Ch2][4]<<Pos2);
    Matrix[6]=Matrix[6] | (Font8x5[Ch2][5]<<Pos2);
    Matrix[7]=Matrix[7] | (Font8x5[Ch2][6]<<Pos2);
    Matrix[8]=Matrix[8] | (Font8x5[Ch2][7]<<Pos2);
}
