#ifndef ADC_SENSOR_H_
#define ADC_SENSOR_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  ADC sensor state */
enum adc_sensor_state {
    ADC_SENSOR_IDLE,
    ADC_SENSOR_ACTIVE,
};

/**
 * Initialize the ADC sensor.
 */
int adc_sensor_init(void);

/**
 * Update ADC sensor state.
 *
 * In IDLE state, performs low-rate current detection.
 * In ACTIVE state, performs continuous sampling.
 */
int adc_sensor_update(void);

/**
 * Get current sensor state.
 */
enum adc_sensor_state adc_sensor_get_state(void);

/**
 * Read one current sample.
 *
 * This is intended for low-rate idle polling.
 *
 * @param sample Raw ADC sample.
 */
int adc_sensor_read_current(uint16_t *sample);

/**
 * Process one idle detection sample.
 *
 * The function updates the internal load detection state
 * according to the configured threshold and hysteresis.
 *
 * @return Current sensor state.
 */
enum adc_sensor_state adc_sensor_process_idle(void);


/**
 * Start high-rate continuous sampling.
 */
int adc_sensor_start(void);


/**
 * Stop high-rate continuous sampling.
 */
int adc_sensor_stop(void);

/**
 * Get the latest sample from active sampling.
 */
int adc_sensor_get_sample(uint16_t *sample);

/**
 * Check whether continuous sampling is active.
 */
// bool adc_sensor_sampling_active(void);


void adc_sensor_debug_all_channels(void);


#ifdef __cplusplus
}
#endif
#endif // ADC_SENSOR_H_