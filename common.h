/*
 * File:   Common.h
 * Author: guiott
 *
 * Created on 12 maggio 2014
 */

#ifndef Common_H
    #define Common_H

    //#include <p18f4620.h>
    #include <p18cxxx.h>
    #define PLL 1

    #ifdef PLL  // 40MHz
        #define XTAL_FREQ  40000000    //crystal frequency
        #define PRESCALER0 256          //TIMER0 selected prescaler
        #define PRESCALER1 8            //TIMER1 selected prescaler
        #define PRESCALER3 8            //TIMER3 selected prescaler

    #else
        #define XTAL_FREQ  20000000    //crystal frequency
        #define PRESCALER0 4           //TIMER0 selected prescaler
        #define PRESCALER1 4           //TIMER1 selected prescaler
        #define PRESCALER3 4           //TIMER3 selected prescaler

    #endif

    #define FCY   (XTAL_FREQ / 4)       //crystal divided by four
    #define TCY   1 / (float)FCY        //single instruction period

    #define TMR1_TICK TCY*PRESCALER1    //TIMER1 tick duration

    //----- Flags
    #define TIMER0_FLAG VARbits1.bit0	// Timer0 Interrupt occurred
    #define SQW_FLAG VARbits1.bit1      // it's time to transmit
    #define TIMER3_FLAG VARbits1.bit2	// Timer3 Interrupt occurred

    //----- Macro
    #if !defined(MIN)
        #define MAX(a,b) (((a) > (b)) ? (a) : (b))
        #define MIN(a,b) (((a) < (b)) ? (a) : (b))
    #endif

    #if !defined(ABS)
        #define ABS(x) ((x) > 0 ? (x) : -(x)) // ABS() for int, long, float...
    #endif

    // I2c device
    #define RTC_PTR 0
    #define I2C_MAX_DEV 1                // how many I2C devices present on bus

    #define MAX_RX_BUFFER 64                // RX buffer size

    extern unsigned char I2cEventFlag;   // SSP port interrupt flag
    extern unsigned char I2cBusCollFlag; // SSP collision flag
    extern unsigned char I2cBusyFlag;    // I2C communication process flag

    extern struct Nibble
    {
        unsigned char Rx:4;		// status
        unsigned char Tx:4;
    };

    extern struct I2cS
    {
        struct Nibble Flag; // Rx 0=idle, n=n Byte to receive (if n!=0)
                            // Tx 0=idle, n=n Byte to transmit (if n!=0)
        unsigned char Done; // I2C procedure executed
        unsigned char RxBuff[4];	// RX buffer Byte(s)
        unsigned char TxBuff[4];	// TX buffer Byte(s)
    };

    extern struct I2cS I2c[I2C_MAX_DEV];
    extern unsigned char I2cAdr[];  //I2C addresses

#endif
