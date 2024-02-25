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

#define ESPRESSIF_MANUFACTURER_ID 0x02E5 // Manufacturer ID 0x02E5 (Espressif Inc)

#define DEVICE_INFORMATION_SERVICE_UUID                                                                               \
    (BLEUUID((uint16_t)0x180A)) // Device Information service, Bluetooth Core Specification

#define SENSOR_SERVICE_UUID "018ddfb8-9527-7c42-ad5e-68938c63f8da"
#define SHOT_TIMER_CHARACTERISTIC_UUID "018ddfb8-9527-7d1a-83ae-3fc50bcba896"
#define VOLTAGE_SENSOR_CHARACTERISTIC_UUID "018ddfb8-9527-7d30-9787-1398271d58e6"
#define SENSOR_CONTROL_CHARACTERISTIC_UUID "018ddfb8-9527-7ed0-bded-146e85beef00" // TBD

uint32_t boot_count = 0;

const char  tx_node_version[] = "Shot Timer BLE Sensor v0.2";
std::string node_name = (char *)tx_node_version;

BLEServer         *pServer;
BLEService        *pService;
BLECharacteristic *pCharVoltage;
BLECharacteristic *pCharShotTimer;
// BLECharacteristic     *pCharTime;
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
        pServer->startAdvertising(); // restart advertising
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
    pCharShotTimer = pService->createCharacteristic(
        SHOT_TIMER_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pCharShotTimer->addDescriptor(new BLE2902());
    pCharVoltage = pService->createCharacteristic(
        VOLTAGE_SENSOR_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pCharVoltage->addDescriptor(new BLE2902());
    // pCharTime = pService->createCharacteristic(TIME_SECOND_16_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ|
    // BLECharacteristic::PROPERTY_NOTIFY);
    // pCharTime->addDescriptor(new BLE2902());

    pService->start();
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(DEVICE_INFORMATION_SERVICE_UUID);
    // pAdvertising->setScanResponse(false);
    // pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not
    // advertise this parameter
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // help with iPhone connections issue
    pAdvertising->setMaxPreferred(0x12);
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
    uint8_t  tmp_array[sizeof(message_from_queue)];
    uint32_t voltage_packet[2];
    uint8_t  shot_timer_packet[4];
    uint32_t ntc_millivolt = 0;
    uint8_t  paddle_state = 0;
    uint16_t total_seconds = 0;

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
                ntc_millivolt = message_from_queue.struct_data_frame.ntc_millivolt;
                paddle_state = message_from_queue.struct_data_frame.paddle_state;
                total_seconds = (message_from_queue.struct_data_frame.timer_minutes * 60) +
                                message_from_queue.struct_data_frame.timer_seconds;
                if (deviceConnected)
                {
                    // pCharVoltage->setValue((uint8_t *)&message_from_queue, sizeof(message_from_queue));
                    voltage_packet[0] = message_from_queue.struct_data_frame.vcc_millivolt;
                    voltage_packet[1] = message_from_queue.struct_data_frame.ntc_millivolt;
                    pCharVoltage->setValue((uint8_t *)&voltage_packet, sizeof(voltage_packet));
                    pCharVoltage->notify(true);
                    shot_timer_packet[0] = message_from_queue.struct_data_frame.paddle_state;
                    shot_timer_packet[1] = message_from_queue.struct_data_frame.timer_minutes;
                    shot_timer_packet[2] = message_from_queue.struct_data_frame.timer_seconds;
                    shot_timer_packet[3] = message_from_queue.struct_data_frame.timer_state;
                    pCharShotTimer->setValue((uint8_t *)&shot_timer_packet, sizeof(shot_timer_packet));
                    pCharShotTimer->notify(true);
                    // pCharTime->setValue(total_seconds);
                    // pCharTime->notify(true);
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
