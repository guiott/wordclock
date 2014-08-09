#include "settings.h"
#include "common.h"
#include "prototypes.h"

/*Settings********************************************************************/
void Settings(void)
{

    /*-------Port A
     * RA0 = LDR Analog IN
     * RA1 = Row 0
     * RA2 = Row 1
     * RA3 = Row 2
     * RA4 = Row 3
     * RA5 = Row 4
     */
    TRISA = 0b11000001;

    /*-------Port B
     * RB0 = INT0 SQW -> RTC 1s interrupt
     * RB1 = Row 7
     * RB2 = Row 8
     * RB3 = Row 9
     * RB4 =
     * RB5 = LED yellow
    */
    TRISB = 0b11000001;

    /*-------Port	C
     * RC0 = Row 5
     * RC1 = Row 6
     * RC2 =
     * RC3 = I2C SCL
     * RC4 = I2C SDA
     * RC5 = Column L
     * RC6 = TX
     * RC7 = RX
    */
    TRISC = 0b11011100;

    /*-------Port	D
     * RD0 = Column A
     * RD1 = Column B
     * RD2 = Column C
     * RD3 = Column D
     * RD4 = Column E
     * RD5 = Column F
     * RD6 = Column G
     * RD7 = Column H
    */
    TRISD = 0b00000000;

    /*-------Port	E
     * RE0 = Column I
     * RE1 = Column J
     * RE2 = Column K
     *
    */
    TRISE = 0b00000000;

    //-------A/D converter
    ADCON1bits.PCFG=0b1110; // only AN0 is an analog ports
    ADCON1bits.VCFG1=0;     // VREF- source = Vss
    ADCON1bits.VCFG0=0;     // VREF+ source = Vdd
    ADCON2bits.ADFM=0;      // left justified. Only the most significant 8 bits
                            // in ADRESH are going to be used
    ADCON2bits.ADCS=0b110;	// A/D Conversion Clock, TAD = TOSC*64 = 1.6us
    ADCON2bits.ACQT=0b111;	// A/D Acquisition Time = 20 TAD = 1.6*20 = 32us
    ADCON0bits.CHS=0;       // AN0 is used
    ADCON0bits.ADON=1;      // A/D on


    //-------TIMER0 used for RX timeout
    T0CONbits.TMR0ON=0;     //TMR0 off
    T0CONbits.T08BIT=0;     //16 bit Timer
    T0CONbits.T0CS=0;       //Timer on internal clock
    T0CONbits.T0SE=0;       //increment on low-to-high transition
    T0CONbits.PSA=0;        //Prescaler assigned
    #ifdef PLL //	40MhZ
        T0CONbits.T0PS=7;   //Prescaler 1:256 clock = 40MHz
    #else
        T0CONbits.T0PS=6;   //Prescaler 1:128 clock = 20MHz
    #endif
    // period = CLKOUT * prescaler * TMR0
    Timer0_ms  = (int)(65535 - (1.5   * FCY / PRESCALER0)); // 1.5s
    INTCONbits.TMR0IF = 0;// reset of interrupt flag

    //-------TIMER1 used for PWM LED light dimming
    T1CONbits.TMR1ON=0;     //TMR1 off
    T1CONbits.TMR1CS=0;     //Internal clock (Fosc/4)
    T1CONbits.RD16=1;       //16 bit Timer
    T1CONbits.T1OSCEN=0;    //Timer1 osc off
    T1CONbits.T1RUN=0;      //Not Timer1 clock
    T1CONbits.T1SYNC=1;     //Do not synchronize external clock input

    #ifdef PLL //	40MhZ
        T1CONbits.T1CKPS=3; //Prescaler 1:8 clock = 40MHz
    #else
        T1CONbits.T1CKPS=2; //Prescaler 1:4 clock = 20MHz
    #endif
    PIR1bits.TMR1IF = 0;    // reset of interrupt flag

     //-------TIMER3 used for LED matrix scan
    T3CONbits.TMR3ON=0;     //TMR0 off
    T3CONbits.RD16=1;       //16 bit Timer
    T3CONbits.TMR3CS=0;     //Timer on internal clock
    #ifdef PLL //	40MhZ
        T3CONbits.T3CKPS=3; //Prescaler 1:8 clock = 40MHz
    #else
        T3CONbits.T3CKPS=2;   //Prescaler 1:4 clock = 20MHz
    #endif
    // period = CLKOUT * prescaler * TMR3
    Timer3_ms  = (int)(65535 - (0.001   * FCY / PRESCALER3)); // 1ms
    PIR2bits.TMR3IF = 0;// reset of interrupt flag
}

void InterruptSettings(void)
{
    RCONbits.IPEN=1;        //interrupt priority enabled

    INTCON2bits.TMR0IP=0;   //TMR0 interrupt low priority
    INTCONbits.TMR0IF=0;    //reset of interrupt flag
    INTCONbits.TMR0IE=1;    //interrupt on TMR0 overflow enabled

    IPR1bits.TMR1IP=1;      //TMR1 interrupt high priority
    PIR1bits.TMR1IF=0;      //reset of interrupt flag
    PIE1bits.TMR1IE=1;      //interrupt on TMR1 overflow enabled

    IPR2bits.TMR3IP=1;      //TMR3 interrupt high priority
    PIR2bits.TMR3IF=0;      //reset of interrupt flag
    PIE2bits.TMR3IE=1;      //interrupt on TMR3 overflow enabled

    INTCON2bits.INTEDG0=0;  //INTO interrupt on falling edge
    INTCONbits.INT0IF = 0;  //reset of interrupt flag
    INTCONbits.INT0IE=1;    //INT0 External Interrupt Enable

    IPR1bits.ADIP=0;        //AD interrupt low priority
    PIR1bits.ADIF=0;        //reset of interrupt flag
    PIE1bits.ADIE=1;        //AD interrupt enabled
}

void RtcInit(void)
{/*Initial configuration for RTC
  * SQW enabled @ 1Hz
  * 12h mode enabled
  */

   unsigned char Tmp;
   I2cAdr[RTC_PTR] = 0xD0;
    /* Reg 07 is the CONTROL REGISTER
    * BIT7    BIT6    BIT5    BIT4    BIT3    BIT2    BIT1    BIT0
    * OUT     0       0       SQWE    0       0       RS1     RS0
    * 0b00010000 to have SQW output at 1Hz
    */
    I2cData(RTC_PTR, 2, 7, 0b00010000, 0, 0, 0); //write init word on reg 7
    while(I2c[RTC_PTR].Done != 1)
    {// wait for sequence completion
        I2cService(); // controls if I2C needs service
    }

    I2cData(RTC_PTR, 1, 0, 0, 0, 0, 1); //request Reg 0 content (CH + second)
    while(I2c[RTC_PTR].Done != 1)
    {// wait for sequence completion
        I2cService(); // controls if I2C needs service
    }

    Tmp = I2c[RTC_PTR].RxBuff[0] & 0b01111111; // enable oscillator

    I2cData(RTC_PTR, 1, 2, 0, 0, 0, 1); //request Reg 2 content (hour)
    while(I2c[RTC_PTR].Done != 1)
    {// wait for sequence completion
        I2cService(); // controls if I2C needs service
    }

    Tmp = I2c[RTC_PTR].RxBuff[0] | 0b01000000; //set 12h mode

    I2cData(RTC_PTR, 2, 2, Tmp, 0, 0, 0); //write back hour with flag set
    while(I2c[RTC_PTR].Done != 1)
    {// wait for sequence completion
        I2cService(); // controls if I2C needs service
    }
}
/*settings*********************************************************************/
