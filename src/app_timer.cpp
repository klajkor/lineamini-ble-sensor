/*
 * app_timer.cpp
 *
 * Timer functions
 *
 *  Created on: 2022. Nov. 30.
 *      Author: Robert_Klajko
 */

#include "app_timer.h"

#define SECONDS_TIMER_PERIOD (pdMS_TO_TICKS(1000U))
#define COOLDOWN_TIMER_PERIOD (pdMS_TO_TICKS(5000U))

static TimerHandle_t xSecondsTimer;
static BaseType_t    xSecondsTimerStarted;
static TimerHandle_t xCooldownTimer;
static BaseType_t    xCooldownTimerStarted;

static shot_timer_t espresso_shot_timer;

void init_timers(void)
{
    reset_shot_timer();
    xSecondsTimer = xTimerCreate("SecondsTimer", SECONDS_TIMER_PERIOD, pdTRUE, 0, vSecondsTimerCallback);
    vTaskDelay(DELAY_1_Sec);
    if (xSecondsTimer != NULL)
    {
        Serial.println("Seconds timer created.");
    }
    else
    {
        Serial.println("!> Failed to create seconds timer");
    }
    xCooldownTimer = xTimerCreate("CooldownTimer", COOLDOWN_TIMER_PERIOD, pdFALSE, 0, vCooldownTimerCallback);
    if (xCooldownTimer != NULL)
    {
        Serial.println("Cooldown timer created.");
    }
    else
    {
        Serial.println("!> Failed to create cooldown timer");
    }
}

static void vSecondsTimerCallback(TimerHandle_t xTimer)
{
    espresso_shot_timer.seconds++;
    if (espresso_shot_timer.seconds >= 60)
    {
        espresso_shot_timer.seconds = 0;
        espresso_shot_timer.minutes++;
        if (espresso_shot_timer.minutes >= 60)
        {
            espresso_shot_timer.minutes = 0;
        }
    }
}

void start_shot_timer(void)
{
    reset_shot_timer();
    stop_cooldown_timer();
    xSecondsTimerStarted = xTimerStart(xSecondsTimer, 0);
    if (xSecondsTimerStarted != pdPASS)
    {
        Serial.println("!> Failed to activate timer");
    }
    espresso_shot_timer.state = SHOT_TIMER_RUNNING;
}

void stop_shot_timer(void)
{
    xSecondsTimerStarted = xTimerStop(xSecondsTimer, 0);
    if (xSecondsTimerStarted != pdPASS)
    {
        Serial.println("!> Failed to deactivate timer");
    }
    espresso_shot_timer.state = SHOT_TIMER_STOPPED;
    start_cooldown_timer();
}

void reset_shot_timer(void)
{
    espresso_shot_timer = {0, 0, SHOT_TIMER_RESET};
}

static void vCooldownTimerCallback(TimerHandle_t xTimer)
{
    reset_shot_timer();
}

void start_cooldown_timer(void)
{
    xCooldownTimerStarted = xTimerStart(xCooldownTimer, 0);
    if (xCooldownTimerStarted != pdPASS)
    {
        Serial.println("!> Failed to activate cooldown timer");
    }
    espresso_shot_timer.state = SHOT_TIMER_HOLD;
}

void stop_cooldown_timer(void)
{
    xCooldownTimerStarted = xTimerStop(xCooldownTimer, 0);
    if (xCooldownTimerStarted != pdPASS)
    {
        Serial.println("!> Failed to deactivate cooldown timer");
    }
}

shot_timer_t get_shot_timer_values(void)
{
    return espresso_shot_timer;
}
