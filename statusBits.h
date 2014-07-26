/*
 * File:   config.h
 * Author: guiott
 *
 * Created on 12 maggio 2014
 */

#ifndef config_H
#define config_H

//----- Status bits
#ifdef PLL  // 40MHz
	#pragma config OSC = HSPLL  //	10 x 4 = 40MHZ
#else
	#pragma config OSC = HS     //	20Mhz
#endif

#pragma config WDT = ON
#pragma config WDTPS = 256 //1s#pragma config LVP = OFF
#pragma config LVP = OFF
#pragma config BOREN = SBORDIS
#pragma config BORV = 1
// #pragma config CCP2MX = PORTC   // CCP2 capture on pin RC1

#endif
