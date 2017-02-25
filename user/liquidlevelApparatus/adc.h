/*
 * adc.h
 *
 *  Created on: 2017/2/23
 *      Author: kk
 */

#ifndef USER_LIQUIDLEVEL_ADC_H_
#define USER_LIQUIDLEVEL_ADC_H_

void adc_storeVoltage(u32 adc_data);
double adc_calculateVoltage(void);

#endif /* USER_LIQUIDLEVEL_ADC_H_ */

