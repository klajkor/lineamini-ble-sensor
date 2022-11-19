/*
 * device_config.h
 *
 *  Created on: 2021. Nov 14.
 *      Author: Robert Klajko
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>

// Define status LED pins
#define GPIO_ERROR_PIN (2U) // ESP32 built-in LED

// INA3221 module I2C address
#define INA3221_I2C_ADDR (0x40)

// INA3221 module GPIO pins
#define GPIO_INA3221_SDA (21U)
#define GPIO_INA3221_SCL (22U)

// Reed switch GPIO pin
#define GPIO_REED_PIN 25 // Digital pin connected to the reed switch

