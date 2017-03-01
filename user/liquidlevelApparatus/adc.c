/*
 * adc.c
 *
 *  Created on: 2017/2/23
 *      Author: kk
 */

#include <eat_type.h>

static int adc_data_count;
static double adc_data_sum;

void adc_storeVoltage(u32 adc_data)
{
    adc_data_count++;
    adc_data_sum += adc_data;
}

double adc_calculatePressure(void)
{
    double adc_data_avr, current, pressure;
    adc_data_avr = adc_data_sum / adc_data_count;
    current = adc_data_avr / 140.0;
    pressure = (current - 4.0) / 16.0 * 10.0;
    adc_data_sum = 0;
    adc_data_count = 0;
    return pressure;
}



