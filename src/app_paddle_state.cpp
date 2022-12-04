/*
 * app_paddle_state.cpp
 *
 * Read reed switch to sense the state of paddle switch
 *
 *  Created on: 2022. Nov. 21.
 *      Author: Robert_Klajko
 */

#include "app_paddle_state.h"

#define REED_SWITCH_ON_LEVEL LOW   /* Reed switch on level */
#define REED_SWITCH_OFF_LEVEL HIGH /* Reed switch off level */

#define READ_REED_SWITCH_TASK_STACK_SIZE ((uint32_t)(4 * 1024)) // 4k

#define PRIO_READ_REED_SWITCH ((UBaseType_t)7)

#define DELAY_READ_REED_SWITCH_MS (pdMS_TO_TICKS(10U)) // 100Hz

static TaskHandle_t        xHandle_ReadReedSwitch = NULL;
static paddle_state_enum_t current_paddle_state = PADDLE_STATE_OFF;

void init_paddle_state_module(void)
{
    BaseType_t xReturned;

    xReturned = xTaskCreatePinnedToCore(xTask_read_reed_switch,           /* Function that implements the task. */
                                        "ReadReedSwitch",                 /* Text name for the task. */
                                        READ_REED_SWITCH_TASK_STACK_SIZE, /* Stack size in words, not bytes. */
                                        (void *)1,                        /* Parameter passed into the task. */
                                        PRIO_READ_REED_SWITCH,            /* Priority at which the task is created. */
                                        &xHandle_ReadReedSwitch, /* Used to pass out the created task's handle. */
                                        APP_CPU_NUM);
    if (xReturned != pdPASS)
    {
        Critical_Error_Handler();
    }
}

uint8_t get_paddle_switch_state(void)
{
    return current_paddle_state;
}

void status_led_off(void)
{
    digitalWrite(GPIO_STATUS_PIN, STATUS_LED_OFF);
}

void status_led_on(void)
{
    digitalWrite(GPIO_STATUS_PIN, STATUS_LED_ON);
}

static void xTask_read_reed_switch(void *pvParameters)
{
    uint32_t            task_delay;
    uint16_t            binary_counter = 0;
    paddle_state_enum_t previous_paddle_state;

    task_delay = DELAY_READ_REED_SWITCH_MS;

    vTaskDelay(DELAY_10_MilliSec);
    while (1)
    {
        binary_counter = binary_counter << 1;
        if (digitalRead(GPIO_REED_PIN) == REED_SWITCH_ON_LEVEL)
        {
            binary_counter++;
        }
        previous_paddle_state = current_paddle_state;
        if (binary_counter > 0)
        {
            if (previous_paddle_state != PADDLE_STATE_ON)
            {
                current_paddle_state = PADDLE_STATE_ON;
                status_led_on();
                start_shot_timer();
            }
        }
        else
        {
            if (previous_paddle_state != PADDLE_STATE_OFF)
            {
                current_paddle_state = PADDLE_STATE_OFF;
                status_led_off();
                stop_shot_timer();
            }
        }
#ifdef SERIAL_STACK_USAGE_LOG
        Serial.print("xTask_read_reed_switch:");
        Serial.print(uxTaskGetStackHighWaterMark(NULL)); // stack size used
        Serial.println();
        Serial.flush();
#endif
        vTaskDelay(task_delay);
    }
    Serial.println("!> Deleting xTask_read_reed_switch() task.");
    vTaskDelete(xHandle_ReadReedSwitch);
}
