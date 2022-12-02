/*
 * app_voltmeter.h
 *
 *  Created on: 2022. Nov. 19.
 *      Author: Robert Klajko
 */

#pragma once

#include <Arduino.h>
#include <INA3221.h>
#include <Wire.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ble_data_frame.h"
#include "device_config.h"
#include "main_defs.h"

typedef struct measured_voltage_struct_t
{
    int32_t vcc_millivolt;
    int32_t ntc_millivolt;
} measured_voltage_struct_t;

typedef enum voltmeter_init_result_t
{
    VOLTMETER_INIT_OK,
    VOLTMETER_NOT_DETECTED,
    VOLTMETER_BEGIN_FAILED
} voltmeter_init_result_t;

extern voltmeter_init_result_t voltmeter_init_result;

void                      voltmeter_init(void);
measured_voltage_struct_t voltmeter_get_last_reading(void);
measured_voltage_struct_t read_voltmeter(void);
