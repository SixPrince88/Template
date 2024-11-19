#ifndef __HW_ADC_H__
#define __HW_ADC_H__
#include "user_gpio.h"
#include "user_adc.h"
#include "user_dma.h"

void HW_ADC_Init(void);
void sensor1_get(void);
void sensor2_get(void);
void sensor3_get(void);
#endif
