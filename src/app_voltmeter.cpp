/*
 * app_voltmeter.c
 *
 *  Created on: 2022. Nov. 19.
 *      Author: Robert Klajko
 */

#include "app_voltmeter.h"

INA3221 ina3221(INA3221_ADDR40_GND);

voltmeter_init_result_t          voltmeter_init_result;
static measured_voltage_struct_t last_measured_voltage;

void voltmeter_init(void)
{
    uint8_t i2c_check_result = 1;
    last_measured_voltage = {-1, -1};
    vTaskDelay(DELAY_10_MilliSec);
    Serial.println("voltmeter_init() started");
    i2c_check_result = checkI2Caddress_0(INA3221_I2C_ADDR);
    if (i2c_check_result == I2C_CHECK_OK)
    {
        Serial.println("INA3221 sensor detected on I2C bus.");
        voltmeter_init_result = VOLTMETER_BEGIN_FAILED;
        ina3221.begin();
        ina3221.reset();
        // Set shunt resistors to 10 mOhm for all channels
        ina3221.setShuntRes(10, 10, 10);
        // Set series filter resistors to 10 Ohm for all channels.
        // Series filter resistors introduce error to the current measurement.
        // The error can be estimated and depends on the resitor values and the bus
        // voltage.
        ina3221.setFilterRes(10, 10, 10);
        ina3221.setShuntMeasDisable();
        ina3221.setAveragingMode(INA3221_REG_CONF_AVG_128);
        voltmeter_init_result = VOLTMETER_INIT_OK;
        Serial.println("INA3221 init succesful.");
        last_measured_voltage = {0, 0};
    }
    else
    {
        voltmeter_init_result = VOLTMETER_NOT_DETECTED;
        Serial.println("INA3221 sensor NOT detected on I2C bus.");
    }
}

measured_voltage_struct_t voltmeter_get_last_reading(void)
{
    return last_measured_voltage;
}

measured_voltage_struct_t read_voltmeter(void)
{
    measured_voltage_struct_t measurements;
    switch (voltmeter_init_result)
    {
    case VOLTMETER_INIT_OK:
        measurements.vcc_millivolt = (int32_t)(1000 * ina3221.getVoltage(INA3221_CH1));
        measurements.ntc_millivolt = (int32_t)(1000 * ina3221.getVoltage(INA3221_CH2));
        break;
    case VOLTMETER_NOT_DETECTED:
        measurements = {-1, -1};
        break;
    case VOLTMETER_BEGIN_FAILED:
        measurements = {-2, -2};
        break;
    }
    last_measured_voltage.vcc_millivolt = measurements.vcc_millivolt;
    last_measured_voltage.ntc_millivolt = measurements.ntc_millivolt;
    return measurements;
}

/*
void voltmeter_convert_reading_to_str(int32_t light_level, char *voltmeter_str_o, uint32_t str_size)
{
    memset(voltmeter_str_o, 0, str_size);
    snprintf(voltmeter_str_o, str_size - 1, "%5d lx", light_level);
}

void voltmeter_convert_reading_to_long_str(int32_t light_level, char *voltmeter_str_o, uint32_t str_size)
{
    memset(voltmeter_str_o, 0, str_size);
    snprintf(voltmeter_str_o, str_size - 1, "Light level: %d lux", light_level);
}
*/
