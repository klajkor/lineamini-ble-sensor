/*
    INA3221 I2C voltmeter:

    GPIO21 - SDA
    GPIO22 - SCL

*/

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>
#include <stdio.h>
#include <stdlib.h>

#include "app_nvs.h"
#include "app_queue.h"
#include "app_tasks.h"
#include "app_timer.h"
#include "app_voltmeter.h"
#include "ble_data_frame.h"
#include "device_config.h"
#include "main_defs.h"

#define SENSOR_SERVICE_UUID (BLEUUID((uint16_t)0x183B))        // Binary Sensor service, Bluetooth Core Specification
#define SENSOR_CHARACTERISTIC_UUID (BLEUUID((uint16_t)0x2AC4)) // Object Properties, Bluetooth Core Specification

uint32_t boot_count = 0;

const char tx_node_version[] = "Linea Mini BLE Sensor v0.1";

BLEServer             *pServer;
BLEService            *pService;
BLECharacteristic     *pCharacteristic;
BLEAdvertising        *pAdvertising;
ble_data_frame_union_t message_from_queue;
bool                   deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

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
    Serial.print("Boot count: ");
    Serial.println(boot_count);
    nvs_usage_info();

    BLEDevice::init(tx_node_version);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    pService = pServer->createService(SENSOR_SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        SENSOR_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SENSOR_SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
    BLEAdvertisementData adv;
    uint8_t              md[9] = {0xDE, 0xDE};
    BLEAddress           mac_addr = BLEDevice::getAddress();
    memcpy(md + 2, *mac_addr.getNative(), 6);
    md[8] = 0; // string need \x00 at the end
    std::string mds = (char *)md;
    adv.setManufacturerData(mds);
    adv.setAppearance(0x440);
    pAdvertising->setAdvertisementData(adv);
    pAdvertising->start();
    BLEDevice::startAdvertising();
    vTaskDelay(DELAY_10_MilliSec);
    init_timers();
    vTaskDelay(DELAY_50_MilliSec);
    GPIO_init();
    vTaskDelay(DELAY_10_MilliSec);
    voltmeter_init();
    vTaskDelay(DELAY_10_MilliSec);
    Create_Queues();
    Create_App_Tasks();
    init_paddle_state_module();
    Serial.println("BLE device active, waiting for connections...");
}

void loop()
{
    uint8_t tmp_array[sizeof(message_from_queue)];

    if (measurements_q_handle != NULL)
    {
        if (xQueueReceive(measurements_q_handle, &message_from_queue, QUEUE_REC_WAIT) == pdPASS)
        {
            if (messageValidate(&message_from_queue) == validate_OK)
            {
                Serial.printf("Paddle state: %d, ", message_from_queue.struct_data_frame.paddle_state);
                Serial.printf("%02d:%02d ", message_from_queue.struct_data_frame.timer_minutes,
                              message_from_queue.struct_data_frame.timer_seconds);
                Serial.printf("Timer state: %d, ", message_from_queue.struct_data_frame.timer_state);
                Serial.printf("VCC mV: %4i, ", message_from_queue.struct_data_frame.vcc_millivolt);
                Serial.printf("NTC mV: %4i", message_from_queue.struct_data_frame.ntc_millivolt);
                if (deviceConnected)
                {
                    pCharacteristic->setValue((uint8_t *)&message_from_queue, sizeof(message_from_queue));
                    pCharacteristic->notify(true);
                    Serial.printf(" BLE notified");
                }
                Serial.printf("\r\n");
            }
            else
            {
                Serial.println("Invalid data frame!");
            }
        }
    }
    vTaskDelay(DELAY_50_MilliSec);
}

void Critical_Error_Handler(void)
{
    Serial.println("Crital Error Occured, Program Halted");
    while (1)
    {
#ifdef GPIO_ERROR_PIN
        digitalWrite(GPIO_ERROR_PIN, ERROR_LED_ON);
        vTaskDelay(200);
        digitalWrite(GPIO_ERROR_PIN, ERROR_LED_OFF);
#endif
        vTaskDelay(200);
    }
}

void GPIO_init(void)
{
    pinMode(GPIO_REED_PIN, INPUT_PULLUP);
    pinMode(GPIO_STATUS_PIN, OUTPUT);
#ifdef GPIO_ERROR_PIN
    pinMode(GPIO_ERROR_PIN, OUTPUT);
    digitalWrite(GPIO_ERROR_PIN, ERROR_LED_ON);
    delay(300);
    digitalWrite(GPIO_ERROR_PIN, ERROR_LED_OFF);
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
