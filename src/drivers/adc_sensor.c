#include "adc_sensor.h"
#include "zephyr/logging/log.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>


#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};


LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

int adc_drv_init_all() {
	int err;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	/* Configure channels individually. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			LOG_ERR("ADC controller device %s is not ready", adc_channels[i].dev->name);
			return -1;
		}
		err = adc_channel_setup_dt(&adc_channels[i]);

		if ( err < 0 ) {
			LOG_ERR("Could not setup channel #%d (%d)\n", i, err);
			return err;
		}
	}

	return 0;
}


int adc_drv_read_all() {
	for (size_t i = 0U; i< ARRAY_SIZE(adc_channels); i++) {
		int32_t val;
	
		LOG_INF("- %s, channel %d: ",
			adc_channels[i].dev->name,
			adc_channels[i].channel_id);
		(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

		err = adc_read_dt(&adc_channels[i], &sequence);	

		if (err < 0) {
			LOG_WRN("Could not read (%d)", err);
			continue;
		}
		
		/*
		 * If using differential mode, the 16 bit value
		 * in the ADC sample buffer should be a signed 2's
		 * complement value.
		 */
		if (adc_channels[i].channel_cfg.differential) {
			val = (int32_t)((int16_t)buf);
		} else {
			val = (int32_t)buf;
		}
		LOG_DBG("%"PRId32, val);
	
		err = adc_raw_to_microvolts_dt(&adc_channels[i], &val);
	
		if (err < 0) {
			LOG_ERR("ADC value is not available");
			return err;
		} else {
			LOG_DBG(" = %"PRId32" mV\n", val_mv);
		}
	}
	return 0;
}
