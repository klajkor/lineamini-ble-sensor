/*
 * app_nvs.h
 *
 *  Created on: 2022. Nov 19.
 *      Author: Robert Klajko
 */

#pragma once

#include "esp_system.h"
#include "nvs.h"
#include "nvs_flash.h"

/**
 * Opaque pointer type representing non-volatile storage handle
 */
typedef uint32_t nvs_handle_t;

extern uint32_t boot_count;

void nvs_init_memory(void);
void nvs_get_boot_counter(void);
void nvs_usage_info(void);
