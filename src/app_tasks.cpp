/*
 * app_tasks.c
 *
 *  Created on: 2021. ápr. 8.
 *      Author: Robert_Klajko
 */

#include "app_tasks.h"

#define MAX_FAILED_SENSOR_READS (40U)
#define FAILED_SENSOR_READS_DURATION_IN_MSEC ((int64_t)(24 * 60 * 60 * 1000)) // 1 day in millisec

#define STANDARD_TASK_STACK_SIZE ((uint32_t)(2 * 1024))  // 2k
#define VOLTMETER_TASK_STACK_SIZE ((uint32_t)(4 * 1024)) // 4k

#define PRIO_VoltMeterRead ((UBaseType_t)7)

#define DELAY_VoltMeterRead_MilliSec (250U)
#define DELAY_VoltMeterRead ((TickType_t)((DELAY_VoltMeterRead_MilliSec) / portTICK_PERIOD_MS))
#define CYCLES_VoltMeterRead ((uint32_t)(1 * 1 * (1000 / DELAY_VoltMeterRead_MilliSec)))

TaskHandle_t xHandle_VoltMeterRead = NULL;

void Create_App_Tasks(void)
{
    BaseType_t xReturned;

    xReturned = xTaskCreatePinnedToCore(xTaskVoltMeterRead,        /* Function that implements the task. */
                                        "VoltMeterRead",           /* Text name for the task. */
                                        VOLTMETER_TASK_STACK_SIZE, /* Stack size in words, not bytes. */
                                        (void *)1,                 /* Parameter passed into the task. */
                                        PRIO_VoltMeterRead,        /* Priority at which the task is created. */
                                        &xHandle_VoltMeterRead,    /* Used to pass out the created task's handle. */
                                        APP_CPU_NUM);
    if (xReturned != pdPASS)
    {
        Critical_Error_Handler();
    }
}

void xTaskVoltMeterRead(void *pvParameters)
{
    uint32_t task_delay;
    int8_t   not_used = 0;
    uint32_t read_cycles = 0;
    uint32_t failed_reads = 0;
    int64_t  task_timer_started = esp_timer_get_time();
    int64_t  failed_sensor_reads_duration = FAILED_SENSOR_READS_DURATION_IN_MSEC * 1000;
    uint8_t  status_to_be_sent = 0;

    measured_voltage_struct_t measured_voltages;

    ble_data_frame_array_t ble_data_frame;

    measured_voltages.vcc_millivolt = 0;
    measured_voltages.ntc_millivolt = 0;
    task_delay = DELAY_VoltMeterRead;

    vTaskDelay(DELAY_10_MilliSec);
    while (voltmeter_init_result == VOLTMETER_INIT_OK &&
           (failed_reads < (MAX_FAILED_SENSOR_READS * CYCLES_VoltMeterRead)))
    {
        read_cycles++;
        measured_voltages = read_voltmeter();
        if (measured_voltages.vcc_millivolt >= 0 && measured_voltages.ntc_millivolt >= 0)
        {
            if (read_cycles >= CYCLES_VoltMeterRead)
            {
                read_cycles = 0;
                build_ble_frame_to_send(ePADDLE_STATE_OFF, 0, 0, measured_voltages.vcc_millivolt,
                                        measured_voltages.ntc_millivolt, &ble_data_frame);
                if (measurements_q_handle != NULL)
                {
                    if (xQueueSend(measurements_q_handle, &ble_data_frame, QUEUE_SEND_WAIT) == pdTRUE)
                    {
                        taskYIELD();
                    }
                    else
                    {
                        Serial.println("xQueueSend Error");
                    }
                }
                else
                {
                    Serial.println("measurements_q_handle is NULL!");
                }
            }
        }
        else
        {
            failed_reads++;
            if (failed_reads % 50 == 0)
            {
                Serial.print("!> Failed to read voltmeter, error code: ");
                Serial.println(measured_voltages.vcc_millivolt);
                Serial.print("!> Failed reads of voltmeter: ");
                Serial.println(failed_reads);
            }
            if (failed_reads > (MAX_FAILED_SENSOR_READS - 5) &&
                (esp_timer_get_time() >= (task_timer_started + failed_sensor_reads_duration)))
            {
                failed_reads = 0;
                task_timer_started = esp_timer_get_time();
                Serial.println("__ Failed reads of voltmeter counter reset");
            }
        }

#ifdef SERIAL_STACK_USAGE_LOG
        Serial.print("xTaskVoltMeterRead:");
        Serial.print(uxTaskGetStackHighWaterMark(NULL)); // stack size used
        Serial.println();
        Serial.flush();
#endif
        vTaskDelay(task_delay);
    }
    Serial.println("!> Too many failed reads of voltmeter!");
    Serial.println("!> Deleting xTaskVoltMeterRead() task.");
    vTaskDelete(xHandle_VoltMeterRead);
}
