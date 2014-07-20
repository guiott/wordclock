#include "timeProc.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>


void RtcReadTime(void)
{/*convert the BCD time read from RTC registers to decimal values
    Hour is in 12H mode without AM/PM
  */
    unsigned char Units;
    unsigned char Tens;

    Units=I2c[RTC_PTR].RxBuff[0]&0b00001111;
    Tens=((I2c[RTC_PTR].RxBuff[0]&0b01110000)>>4)*10;
    Sec=Units + Tens;

    Units=I2c[RTC_PTR].RxBuff[1]&0b00001111;
    Tens=((I2c[RTC_PTR].RxBuff[1]&0b01110000)>>4)*10;
    Min=Units + Tens;

    Units=I2c[RTC_PTR].RxBuff[2]&0b00001111;
    Tens=((I2c[RTC_PTR].RxBuff[2]&0b00010000)>>4)*10;
    Hour=Units + Tens;
}

void RtcWriteTime(void)
{
    unsigned char Units;
    unsigned char Tens;

    Tens = NtpSec / 10;
    Units = NtpSec - (Tens * 10);
    Sec = (Tens << 4) | Units;


    Tens = NtpMin / 10;
    Units = NtpMin - (Tens * 10);
    Min = (Tens << 4) | Units;

    Tens = NtpHour / 10;
    Units = NtpHour - (Tens * 10);
    Hour = (Tens<<4) | Units;

    if(I2cBuffChk(RTC_PTR))
    {// if previous operations are over, start a new one
        I2cData(RTC_PTR, 4, 0, Sec, Min, Hour, 0);
    }
}

char TimeDecode(void)
{// decode the current time from http://www.inrim.it
 //Example: *OPEN*15 OCT 2012 22:44:39 CEST\n*CLOS*

    int TimePoint;

    TimePoint = strcspn(RXbuff, ":" );

    Hour = atoi(RXbuff+TimePoint-2);
    Min  = atoi(RXbuff+TimePoint+1);
    Sec  = atoi(RXbuff+TimePoint+4);
    return '0';
}

