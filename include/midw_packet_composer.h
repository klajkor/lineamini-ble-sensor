/*
 * midw_packet_composer.h
 *
 *  Created on: 2024 Feb 25.
 *      Author: Robert_Klajko
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PACKET_PREAMBLE (0x5A)
#define OVERHEAD_TO_DATA (17)

typedef enum
{
    PACKET_TYPE_SHOT_TIMER = 0,
    PACKET_TYPE_VOLTAGE,
    PACKET_TYPE_COMMAND,
    PACKET_TYPE_LAST
} packet_type_t;

void init_packet_composer(void);
uint16_t general_packet_composer(uint8_t packet_type, uint8_t *p_data_payload, uint8_t data_payload_size, uint8_t *p_packet_buffer);
