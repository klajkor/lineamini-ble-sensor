/*
    INA3221 I2C voltmeter:

    GPIO21 - SDA
    GPIO22 - SCL

*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>

#include "app_nvs.h"
#include "app_queue.h"
#include "app_tasks.h"
#include "app_voltmeter.h"
#include "ble_data_frame.h"
#include "device_config.h"
#include "main_defs.h"

uint32_t boot_count = 0;

const char tx_node_version[] = "Linea Mini BLE Sensor v0.1";

void setup(void)
{
    Serial.begin(115200);
    vTaskDelay(DELAY_1_Sec);
    nvs_init_memory();
    nvs_get_boot_counter();
    Serial.println("ESP32 setup() started.");
    vTaskDelay(DELAY_500_MilliSec);
    Wire.begin();
    Serial.println("I2C_0 bus begin completed");
    GPIO_init();
    vTaskDelay(DELAY_10_MilliSec);
    Serial.print("Boot count: ");
    Serial.println(boot_count);
    nvs_usage_info();
    voltmeter_init();
    vTaskDelay(DELAY_10_MilliSec);
    // BLE_init();
    Create_Queues();
    Create_App_Tasks();
}

void loop()
{
    ble_data_frame_struct_t message_from_queue;
    if (measurements_q_handle != NULL)
    {
        if (xQueueReceive(measurements_q_handle, &message_from_queue, QUEUE_REC_WAIT) == pdPASS)
        {
            Serial.print("VCC mV: ");
            Serial.print(message_from_queue.vcc_millivolt);
            Serial.print(" NTC mV: ");
            Serial.println(message_from_queue.ntc_millivolt);
        }
    }
    vTaskDelay(DELAY_1_MilliSec);
}

void Critical_Error_Handler(void)
{
    Serial.println("Crital Error Occured, Program Halted");
#ifdef GPIO_ERROR_PIN
    digitalWrite(GPIO_ERROR_PIN, HIGH);
#endif
    while (1)
    {
#ifdef GPIO_ERROR_PIN
        digitalWrite(GPIO_ERROR_PIN, HIGH);
        vTaskDelay(200);
        digitalWrite(GPIO_ERROR_PIN, LOW);
#endif
        vTaskDelay(200);
    }
}

void GPIO_init(void)
{
    pinMode(GPIO_REED_PIN, INPUT_PULLUP);
#ifdef GPIO_ERROR_PIN
    pinMode(GPIO_ERROR_PIN, OUTPUT);
    digitalWrite(GPIO_ERROR_PIN, HIGH);
    delay(300);
    digitalWrite(GPIO_ERROR_PIN, LOW);
#endif
    yield();
    delay(50);
}

// If address is present, then returns 0, otherwise returns an error code
uint8_t checkI2Caddress_0(int _address)
{
    Wire.beginTransmission(_address);
    return Wire.endTransmission();
}
