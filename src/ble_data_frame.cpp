/*
 * ble_data_frame.c
 *
 *  Created on: 2021 March 13.
 *      Author: Robert_Klajko
 */
#include "ble_data_frame.h"

ble_data_frame_ret_val_enum build_ble_frame_to_send(uint8_t paddle_state_i, uint8_t timer_minutes_i,
                                                    uint8_t timer_seconds_i, uint8_t timer_state_i,
                                                    int32_t vcc_millivolt_i, int32_t ntc_millivolt_i,
                                                    ble_data_frame_array_t *p_data_frame)
{
    uint16_t                    i;
    uint8_t                     crc_byte;
    ble_data_frame_ret_val_enum retval;
    ble_data_frame_union_t      data_frame;

    if (p_data_frame != NULL)
    {
        memset((char *)p_data_frame, 0, sizeof(ble_data_frame_array_t) * sizeof(uint8_t));
        data_frame.struct_data_frame.start_byte = FRAME_START_BYTE;
        data_frame.struct_data_frame.paddle_state = paddle_state_i;
        data_frame.struct_data_frame.timer_minutes = timer_minutes_i;
        data_frame.struct_data_frame.timer_seconds = timer_seconds_i;
        data_frame.struct_data_frame.timer_state = timer_state_i;
        data_frame.struct_data_frame.vcc_millivolt = vcc_millivolt_i;
        data_frame.struct_data_frame.ntc_millivolt = ntc_millivolt_i;
        crc_byte = gencrc8(data_frame.array_data_frame, FRAME_CRC_BYTE_POS);
        data_frame.struct_data_frame.crc_byte = crc_byte;
        data_frame.struct_data_frame.stop_byte = FRAME_STOP_BYTE;
        memcpy(p_data_frame, &data_frame, sizeof(ble_data_frame_array_t) * sizeof(uint8_t));
        retval = ble_data_frame_ret_val_OK;
    }
    else
    {
        retval = ble_data_frame_ret_val_NOK;
    }

    return retval;
}

uint8_t gencrc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = CRC_DEFAULT_VALUE;
    uint8_t i, j;
    for (i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ CRC_POLYNOM);
            else
                crc <<= 1;
        }
    }
    return crc;
}

messageValidate_ret_val_enum messageValidate(ble_data_frame_union_t *pdata_frame)
{
    messageValidate_ret_val_enum retval;
    uint8_t                      msgValid;
    msgValid = 1;
    if ((msgValid == 1) && (pdata_frame->struct_data_frame.start_byte == FRAME_START_BYTE))
    {
        msgValid = 1;
    }
    else
    {
        msgValid = 0;
        retval = validate_StartByteError;
    }
    if ((msgValid == 1) && (pdata_frame->struct_data_frame.stop_byte == FRAME_STOP_BYTE))
    {
        msgValid = 1;
    }
    else
    {
        msgValid = 0;
        retval = validate_StartByteError;
    }
    if ((msgValid == 1) &&
        (pdata_frame->struct_data_frame.crc_byte == gencrc8((uint8_t *)pdata_frame, FRAME_CRC_BYTE_POS)))
    {
        msgValid = 1;
    }
    else
    {
        msgValid = 0;
        retval = validate_CRCError;
    }
    if (msgValid == 1)
    {
        retval = validate_OK;
    }
    return retval;
}
