/*
 * app_timer.h
 *
 * Timer functions
 *
 *  Created on: 2022. Nov. 30.
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
    SHOT_TIMER_RESET = 0,
    SHOT_TIMER_RUNNING,
    SHOT_TIMER_STOPPED
} shot_timer_state_t;

typedef struct shot_timer_t
{
    uint8_t            minutes;
    uint8_t            seconds;
    shot_timer_state_t state;
} shot_timer_t;

void         init_timers(void);
void         vSecondsTimerCallback(TimerHandle_t xTimer);
void         start_shot_timer(void);
void         stop_shot_timer(void);
void         reset_shot_timer(void);
void         vCooldownTimerCallback(TimerHandle_t xTimer);
void         start_cooldown_timer(void);
void         stop_cooldown_timer(void);
shot_timer_t get_shot_timer_values(void);
