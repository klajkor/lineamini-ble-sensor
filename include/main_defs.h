/*
 * main_defs.h
 *
 *  Created on: 2021 Apr 24
 *  Author: Robert Klajko
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <stdio.h>

#include "device_config.h"

#define DELAY_1_MilliSec (pdMS_TO_TICKS(1))
#define DELAY_2_MilliSec (pdMS_TO_TICKS(2))
#define DELAY_5_MilliSec (pdMS_TO_TICKS(5))
#define DELAY_10_MilliSec (pdMS_TO_TICKS(10))
#define DELAY_50_MilliSec (pdMS_TO_TICKS(50))
#define DELAY_100_MilliSec (pdMS_TO_TICKS(100))
#define DELAY_500_MilliSec (pdMS_TO_TICKS(500))
#define DELAY_1_Sec (pdMS_TO_TICKS(1000))
#define DELAY_2_Sec (pdMS_TO_TICKS(2000))
#define DELAY_5_Sec (pdMS_TO_TICKS(5000))
#define DELAY_10_Sec (pdMS_TO_TICKS(10000))

#define I2C_CHECK_OK (0)

#define ERROR_LED_ON HIGH  /* LED Switch on level */
#define ERROR_LED_OFF LOW  /* LED Switch off level */
#define STATUS_LED_ON HIGH /* LED Switch on level */
#define STATUS_LED_OFF LOW /* LED Switch off level */

// Uncomment the below line to get Stack usege log on Serial
// #define SERIAL_STACK_USAGE_LOG 1

void Critical_Error_Handler(void);
void GPIO_init(void);

// If address is present, then returns 0, otherwise returns an error code
uint8_t checkI2Caddress_0(int _address);
