/*
 * midw_crc.h
 *
 *  Created on: 2024 Feb 25.
 *      Author: Robert_Klajko
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t gencrc8(uint8_t *data, uint8_t len);
