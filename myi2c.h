/* 
 * File:   myi2c.h
 * Author: guiott
 *
 * Created on 18 maggio 2014, 9.38
 */

#ifndef MYI2C_H
#define	MYI2C_H

#endif	/* MYI2C_H */

//I2C definitions

unsigned char I2cEventFlag;   // SSP port interrupt flag
unsigned char I2cBusCollFlag; // SSP collision flag
unsigned char I2cBusyFlag;    // I2C communication process flag

#define	I2C_MAX_DEV 1         // how many I2C devices present on bus

unsigned char I2cDevPtr = 0;  // I2C used device pointer

unsigned char I2cAdr[] = {0x00};  //I2C addresses

struct Nibble
{
    unsigned char Rx:4;		// status
    unsigned char Tx:4;
};

struct I2cS
{
    struct Nibble Flag;	// Rx 0=idle, n=n Byte to receive (if n!=0)
			// Tx 0=idle, n=n Byte to transmit (if n!=0)
    unsigned char Done; // I2C procedure executed
    unsigned char RxBuff[4];	// RX buffer Byte(s)
    unsigned char TxBuff[4];	// TX buffer Byte(s)
};

struct I2cS I2c[I2C_MAX_DEV];
struct Nibble Ptr;		// I2C buffer pointer (Rx & TX)
char I2cStat;			// status pointer for I2C sequence

// status
enum I2cBusStates{STRT, WRITE, ADRR, READ, ACK, NACK, RSTART, STP, FINE};

// I2c device pointer
#define RTC_PTR 0
