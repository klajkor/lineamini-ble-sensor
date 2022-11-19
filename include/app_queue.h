/*
 * app_queue.h
 *
 * Define and create FreeRTOS Queues
 *
 *  Created on: 2021. Apr 7.
 *      Author: Robert_Klajko
 */

#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ble_data_frame.h"
#include "main_defs.h"

#define MEASUREMENTS_Q_LEN ((UBaseType_t)60)

#define QUEUE_SEND_WAIT (pdMS_TO_TICKS(10))
#define QUEUE_REC_WAIT (pdMS_TO_TICKS(10))

extern QueueHandle_t measurements_q_handle;

extern void Create_Queues(void);
