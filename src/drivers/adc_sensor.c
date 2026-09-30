#include "adc_sensor.h"
#include "zephyr/logging/log.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>


LOG_MODULE_REGISTER(adc_sensor, LOG_LEVEL_INF);

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), 
			     io_channels,
			     DT_SPEC_AND_COMMA)
};

#define ADC_CHANNEL_COUNT       ARRAY_SIZE(adc_channels)

#define CURRENT_ON_THRESHOLD   5
#define CURRENT_OFF_THRESHOLD  2

static enum adc_sensor_state state = ADC_SENSOR_IDLE;

static uint16_t current_sample;

static uint16_t idle_max;

static uint8_t active_channel;

static int adc_sensor_read_channel(uint8_t index, uint16_t *sample) {
	int err;
	struct adc_sequence sequence = {
		.buffer = sample,
		.buffer_size = sizeof(*sample)
	};

	if (index >= ARRAY_SIZE(adc_channels)) 
		return -EINVAL;

	if (sample == NULL)
		return -EINVAL;

	err = adc_sequence_init_dt(&adc_channels[index], &sequence);


	if (err < 0) {
		LOG_ERR("Failed to initialize ADC sequence: %d", err);
		return err;
	}
	err = adc_read_dt(&adc_channels[index], &sequence);

	if (err < 0) {
		LOG_ERR("ADC channel %u read failed: %d", index, err);
		return err;
	}

	return 0;
}

void adc_sensor_debug_all_channels(void)
{
        uint16_t sample;

        for (int i = 0; i < 7; i++) {
                if (adc_sensor_read_channel(i, &sample) == 0) {
                        LOG_INF("ADC%d: %u", i, sample);
                }
        }
}

int adc_sensor_init(void)
{
        int err;

        for (size_t i = 0; i < ARRAY_SIZE(adc_channels); i++) {
                if (!adc_is_ready_dt(&adc_channels[i])) {
                        LOG_ERR("ADC device %s is not ready",
                                adc_channels[i].dev->name);
                        return -ENODEV;
                }

                err = adc_channel_setup_dt(&adc_channels[i]);
                if (err < 0) {
                        LOG_ERR("Failed to setup ADC channel %u: %d",
                                i, err);
                        return err;
                }
        }

	state = ADC_SENSOR_IDLE;
	current_sample = 0;
	active_channel = 0;
	idle_max = 0;

        LOG_INF("ADC sensor initialized");

        return 0;
}

static void adc_sensor_update_idle(void)
{
        uint16_t sample;

        for (uint8_t i = 0; i < ADC_CHANNEL_COUNT; i++) {
                if (adc_sensor_read_channel(i, &sample) < 0) {
                        continue;
                }

                if (sample > idle_max) {
                        idle_max = sample;
                }

                LOG_DBG("Idle ADC%u: %u, max: %u",
                        i, sample, idle_max);

                if (sample >= CURRENT_ON_THRESHOLD) {
                        active_channel = i;
                        current_sample = sample;

                        LOG_INF("Load detected on ADC%u, sample=%u",
                                active_channel, current_sample);

                        state = ADC_SENSOR_ACTIVE;
                        idle_max = 0;

                        return;
                }
        }
}

static void adc_sensor_update_active(void)
{
        int err;

        err = adc_sensor_read_channel(active_channel,
                                      &current_sample);

        if (err < 0) {
                return;
        }

        LOG_DBG("Active ADC%u sample: %u",
                active_channel, current_sample);

        if (current_sample <= CURRENT_OFF_THRESHOLD) {
                LOG_INF("Channel ADC%u load removed, entering IDLE mode",
                        active_channel);

                state = ADC_SENSOR_IDLE;
                active_channel = 0;
                idle_max = 0;
        }
}


int adc_sensor_update(void) {
	switch (state) {
		case ADC_SENSOR_IDLE:
			adc_sensor_update_idle();
			break;
		
		case ADC_SENSOR_ACTIVE:
			adc_sensor_update_active();
			break;
		
		default:
			state = ADC_SENSOR_IDLE;
			break;
	}

	return 0;
}


enum adc_sensor_state adc_sensor_get_state(void) {
	return state;
}


int adc_sensor_get_sample(uint16_t *sample) {
	if (sample == NULL)
		return -EINVAL;

	*sample = current_sample;

	return 0;
}

int adc_sensor_start(void) {
	if (state == ADC_SENSOR_ACTIVE)
		return 0;

	LOG_INF("Starting active sampling");

	state = ADC_SENSOR_ACTIVE;

	return 0;
} 


int adc_sensor_stop(void) {
	if (state == ADC_SENSOR_IDLE)
		return 0;

	LOG_INF("Starting active sampling");

	state = ADC_SENSOR_IDLE;

	return 0;
}
