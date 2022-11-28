/*
 * app_paddle_state.h
 *
 * Read reed switch to sense the state of paddle switch
 *
 *  Created on: 2022. Nov. 21.
 *      Author: Robert_Klajko
 */

#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main_defs.h"

typedef enum
{
    PADDLE_STATE_OFF = 0x00,
    PADDLE_STATE_ON = 0x01
} paddle_state_enum_t;

typedef enum
{
    REED_SWITCH_STATE_RESET = 0x00,
    REED_SWITCH_STATE_READ_PIN,
    REED_SWITCH_STATE_SET_PADDLE_STATE
} reed_switch_state_enum_t;

void    init_paddle_state_module(void);
uint8_t get_paddle_switch_state(void);
void    status_led_off(void);
void    status_led_on(void);
void    xTask_read_reed_switch(void *pvParameters);
