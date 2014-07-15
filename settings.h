/*
 * File:   settings.h
 * Author: guiott
 *
 * Created on 12 maggio 2014
 */

#ifndef settings_H
#define settings_H

extern unsigned int Timer0_ms;
extern unsigned int Timer3_ms;

extern struct Bits
{
        unsigned bit0:1;
        unsigned bit1:1;
        unsigned bit2:1;
        unsigned bit3:1;
        unsigned bit4:1;
        unsigned bit5:1;
        unsigned bit6:1;
        unsigned bit7:1;
};
extern struct Bits VARbits1;

#endif
