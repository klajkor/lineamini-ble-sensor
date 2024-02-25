/*
 *  midw_crc.cpp
 *
 *  Created on: 2024 Feb 24.
 *      Author: Robert_Klajko
 */

#include "midw_crc.h"

#define CRC_DEFAULT_VALUE (0xFF)
#define CRC_POLYNOM (0x31)

uint8_t gencrc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = CRC_DEFAULT_VALUE;
    uint8_t i, j;
    for (i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ CRC_POLYNOM);
            else
                crc <<= 1;
        }
    }
    return crc;
}
