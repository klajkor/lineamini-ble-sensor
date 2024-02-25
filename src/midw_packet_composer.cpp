/*
 *  midw_packet_composer.cpp
 *
 *  Created on: 2024 Feb 24.
 *      Author: Robert_Klajko
 */

#include "midw_packet_composer.h"
#include "midw_crc.h"

static uint16_t packet_counter[PACKET_TYPE_LAST];

void init_packet_composer(void)
{
    uint8_t i;
    for (i=0; i < PACKET_TYPE_LAST; i++)
    {
        packet_counter[i] = 0;
    }
}
