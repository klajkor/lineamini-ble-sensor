/*
 * ble_data_frame.h
 *
 *  Created on: 2022 Nov 19.
 *      Author: Robert_Klajko
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FRAME_START_BYTE (0xA5)
#define FRAME_STOP_BYTE (0xEE)
#define FRAME_CRC_BYTE_POS ((uint8_t)13)

typedef struct __attribute__((packed))
{
    uint8_t start_byte;
    uint8_t paddle_state;
    uint8_t timer_minutes;
    uint8_t timer_seconds;
    uint8_t timer_state;
    int32_t vcc_millivolt;
    int32_t ntc_millivolt;
    uint8_t crc_byte;
    uint8_t stop_byte;
} ble_data_frame_struct_t;

typedef uint8_t ble_data_frame_array_t[sizeof(ble_data_frame_struct_t)];

typedef union ble_data_frame_union_t {
    ble_data_frame_struct_t struct_data_frame;
    ble_data_frame_array_t  array_data_frame;
} ble_data_frame_union_t;

typedef enum ble_data_frame_ret_val_enum
{
    ble_data_frame_ret_val_NOK = 0,
    ble_data_frame_ret_val_OK,
    ble_data_frame_ret_val_End
} ble_data_frame_ret_val_enum;

typedef enum messageValidate_ret_val_enum
{
    validate_OK = 0,
    validate_MsgLenError,
    validate_StartByteError,
    validate_StopByteError,
    validate_CRCError
} messageValidate_ret_val_enum;

ble_data_frame_ret_val_enum  build_ble_frame_to_send(uint8_t paddle_state_i, uint8_t timer_minutes_i,
                                                     uint8_t timer_seconds_i, uint8_t timer_state_i,
                                                     int32_t vcc_millivolt_i, int32_t ntc_millivolt_i,
                                                     ble_data_frame_array_t *p_data_frame);
messageValidate_ret_val_enum messageValidate(ble_data_frame_union_t *pdata_frame);

