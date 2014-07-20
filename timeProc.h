/* 
 * File:   TimeProc.h
 * Author: guiott
 *
 * Created on 19 luglio 2014, 12.51
 */

#ifndef TIMEPROC_H
#define	TIMEPROC_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#include "common.h"

//RTC time
unsigned int Sec;
unsigned int Min;
unsigned int Hour;

//Time server time
unsigned char NtpSec;
unsigned char NtpMin;
unsigned char NtpHour;

extern unsigned char RXbuff[MAX_RX_BUFFER];	// RX buffer

void I2cHighService (void);
void I2cLowService (void);
void I2cSettings(void);
void I2cService (void);
unsigned char I2cBuffChk (unsigned char I2cDevice);
void I2cData (unsigned char DevPtr, unsigned char TxBytes,
                unsigned char TX1, unsigned char TX2, unsigned char TX3,
                unsigned char TX4, unsigned char RxBytes);
void RtcReadTime(void);
void RtcWriteTime(void);
void RtcInit(void);

#endif	/* TIMEPROC_H */
