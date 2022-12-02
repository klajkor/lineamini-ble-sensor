/*
 * app_tasks.h
 *
 *  Created on: 2021. Apr 24.
 *      Author: Robert Klajko
 */

#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "app_paddle_state.h"
#include "app_queue.h"
#include "app_timer.h"
#include "app_voltmeter.h"
#include "ble_data_frame.h"
#include "device_config.h"
#include "main_defs.h"

void Create_App_Tasks(void);
static void xTaskVoltMeterRead(void *pvParameters);
static void xTaskDataFrameBuild(void *pvParameters);
