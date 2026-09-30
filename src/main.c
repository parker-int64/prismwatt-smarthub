#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include "app/app_ui.h"
#include "app/ext_io.h"
#include "drivers/adc_sensor.h"
#include "drivers/binary_sensor.h"
#include "drivers/display_mgr.h"

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("application started");

	int ret = ext_io_init();
	if (ret < 0) {
		LOG_ERR("External IO initialization failed: %d", ret);
		return ret;
	}

	ret = ext_io_led_test();
	if (ret < 0) {
		LOG_ERR("LED test failed: %d", ret);
		return ret;
	}

	ret = adc_sensor_init();

	if (ret < 0) {
		LOG_ERR("ADC channel initialization failed: %d", ret);
		return ret;
	}

	ret = gpio_keys_init();

	if (ret < 0) {
		LOG_ERR("GPIO keys initialization failed: %d", ret);
		return ret;
	}

	ret = display_mgr_init();

	if (ret < 0) {
		LOG_ERR("Failed to initialize the display: %d", ret);
		return ret;
	}

	ret = app_ui_init();

	if (ret < 0) {
		LOG_ERR("Failed to initialize the UI: %d", ret);
		return ret;
	}

	while (true) {
		adc_sensor_update();

		if (adc_sensor_get_state() == ADC_SENSOR_ACTIVE) {
			uint16_t sample;

			adc_sensor_get_sample(&sample);

			LOG_INF("Current sample: %u", sample);
		}

		k_sleep(K_MSEC(1000));
	}

	return 0;
}
