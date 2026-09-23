#ifndef ADC_SENSOR_H_
#define ADC_SENSOR_H_

#include <stdint.h>

int adc_drv_init_all(void);

int adc_drv_read_channel(uint8_t index, int32_t *val);


#endif // ADC_SENSOR_H_