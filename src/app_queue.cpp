/*
 * app_queue.c
 *
 * Define and create FreeRTOS Queues
 *
 *  Created on: 2021. Apr 7.
 *      Author: Robert_Klajko
 */

#include "app_queue.h"

QueueHandle_t measurements_q_handle;

void Create_Queues(void)
{
    /* Create queue for measurements */
    measurements_q_handle = xQueueCreate(MEASUREMENTS_Q_LEN, sizeof(ble_data_frame_array_t));
    if (measurements_q_handle == NULL)
    {
        Critical_Error_Handler();
    }
}
