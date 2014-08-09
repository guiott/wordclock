/*
 * File:   prototypes.h
 * Author: guiott
 *
 * Created on 12 maggio 2014
 */

#ifndef prototypes_H
#define prototypes_H

/*Prototypes*******************************************************************/

void Settings(void);
void InterruptHandlerHigh (void);
void InterruptHandlerLow (void);
void SetTimer0(int Count);
void SetTimer1(unsigned char Count);
void SetTimer3(int Count);
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
void InterruptSettings(void);
char TimeDecode(void);
void ScanMatrix(void);
void MatrixSetting(void);
void SetColB(void);
void SetColOff(void);
void SetRowOff(void);
void WordSetting();
void TestMatrix();
void TestLed(void);

#endif
