/*
 * app_nvs.cpp
 *
 *  Created on: 2022. Nov 19.
 *      Author: Robert Klajko
 */

#include "app_nvs.h"

void nvs_init_memory(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        nvs_flash_erase();
        err = nvs_flash_init();
    }
}

void nvs_get_boot_counter(void)
{
    esp_err_t    err;
    nvs_handle_t my_handle;
    err = nvs_open("boot_params", NVS_READWRITE, &my_handle);
    if (err == ESP_OK)
    {
        err = nvs_get_u32(my_handle, "boot_count", &boot_count);
        switch (err)
        {
        case ESP_OK:
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        boot_count++;
        printf("NVS set boot_count value ");
        err = nvs_set_u32(my_handle, "boot_count", boot_count);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("NVS commit ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
}

void nvs_usage_info(void)
{
    nvs_stats_t nvs_stats;
    nvs_get_stats(NULL, &nvs_stats);
    printf("NVS: Used %d/%d entries, NameSpaces %d\n", nvs_stats.used_entries, nvs_stats.total_entries,
           nvs_stats.namespace_count);
}
