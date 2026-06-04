#include "tsl2561.h"

//****************************************************************************
//
// Copyright  2004−2005 TAOS, Inc.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Module Name:
// lux.cpp
//
//****************************************************************************

#define LUX_SCALE 14
#define RATIO_SCALE 9

// Integration time scaling factors
#define CH_SCALE 10
#define CHSCALE_TINT0 0x7517
#define CHSCALE_TINT1 0x0fe7

// T Package coefficients
#define K1T 0x0040
#define B1T 0x01f2
#define M1T 0x01be
#define K2T 0x0080
#define B2T 0x0214
#define M2T 0x02d1
#define K3T 0x00c0
#define B3T 0x023f
#define M3T 0x037b
#define K4T 0x0100
#define B4T 0x0270
#define M4T 0x03fe
#define K5T 0x0138
#define B5T 0x016f
#define M5T 0x01fc
#define K6T 0x019a
#define B6T 0x00d2
#define M6T 0x00fb
#define K7T 0x029a
#define B7T 0x0018
#define M7T 0x0012
#define K8T 0x029a
#define B8T 0x0000
#define M8T 0x0000

// CS package coefficients
#define K1C 0x0043
#define B1C 0x0204
#define M1C 0x01ad
#define K2C 0x0085
#define B2C 0x0228
#define M2C 0x02c1
#define K3C 0x00c8
#define B3C 0x0253
#define M3C 0x0363
#define K4C 0x010a
#define B4C 0x0282
#define M4C 0x03df
#define K5C 0x014d
#define B5C 0x0177
#define M5C 0x01dd
#define K6C 0x019a
#define B6C 0x0101
#define M6C 0x0127
#define K7C 0x029a
#define B7C 0x0037
#define M7C 0x002b
#define K8C 0x029a
#define B8C 0x0000
#define M8C 0x0000

unsigned int CalculateLux(
    unsigned int iGain,
    unsigned int tInt,
    unsigned int ch0,
    unsigned int ch1,
    int iType
)
{
    unsigned long chScale;
    unsigned long channel1;
    unsigned long channel0;

    switch (tInt)
    {
        case 0: // 13.7 msec
            chScale = CHSCALE_TINT0;
            break;

        case 1: // 101 msec
            chScale = CHSCALE_TINT1;
            break;

        default: // assume no scaling
            chScale = (1 << CH_SCALE);
            break;
    }

    // Scale if gain is NOT 16X
    if (!iGain) {
        chScale = chScale << 4;
    }

    // Scale the channel values
    channel0 = (ch0 * chScale) >> CH_SCALE;
    channel1 = (ch1 * chScale) >> CH_SCALE;

    // Find the ratio of the channel values: Channel1 / Channel0
    unsigned long ratio1 = 0;

    if (channel0 != 0) {
        ratio1 = (channel1 << (RATIO_SCALE + 1)) / channel0;
    }

    // Round the ratio value
    unsigned long ratio = (ratio1 + 1) >> 1;

    unsigned int b = 0;
    unsigned int m = 0;

    switch (iType)
    {
        case 0: // T package
            if ((ratio >= 0) && (ratio <= K1T)) {
                b = B1T;
                m = M1T;
            }
            else if (ratio <= K2T) {
                b = B2T;
                m = M2T;
            }
            else if (ratio <= K3T) {
                b = B3T;
                m = M3T;
            }
            else if (ratio <= K4T) {
                b = B4T;
                m = M4T;
            }
            else if (ratio <= K5T) {
                b = B5T;
                m = M5T;
            }
            else if (ratio <= K6T) {
                b = B6T;
                m = M6T;
            }
            else if (ratio <= K7T) {
                b = B7T;
                m = M7T;
            }
            else if (ratio > K8T) {
                b = B8T;
                m = M8T;
            }
            break;

        case 1: // CS package
            if ((ratio >= 0) && (ratio <= K1C)) {
                b = B1C;
                m = M1C;
            }
            else if (ratio <= K2C) {
                b = B2C;
                m = M2C;
            }
            else if (ratio <= K3C) {
                b = B3C;
                m = M3C;
            }
            else if (ratio <= K4C) {
                b = B4C;
                m = M4C;
            }
            else if (ratio <= K5C) {
                b = B5C;
                m = M5C;
            }
            else if (ratio <= K6C) {
                b = B6C;
                m = M6C;
            }
            else if (ratio <= K7C) {
                b = B7C;
                m = M7C;
            }
            else if (ratio > K8C) {
                b = B8C;
                m = M8C;
            }
            break;

        default:
            b = 0;
            m = 0;
            break;
    }

    /*
     * Important C fix:
     * In the copied datasheet code, temp was unsigned long.
     * But then it checks if temp < 0, which cannot work with unsigned values.
     * So we use signed long here.
     */
    long temp;

    temp = ((long)channel0 * b) - ((long)channel1 * m);

    // Do not allow negative lux value
    if (temp < 0) {
        temp = 0;
    }

    // Round lsb: 2^(LUX_SCALE - 1)
    temp += (1 << (LUX_SCALE - 1));

    // Strip off fractional portion
    unsigned long lux = ((unsigned long)temp) >> LUX_SCALE;

    return (unsigned int)lux;
}
