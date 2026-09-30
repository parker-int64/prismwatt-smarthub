#include "ext_io.h"
#include <zephyr/logging/log.h>
#include <zephyr/drivers/regulator.h>

LOG_MODULE_REGISTER(ext_io, CONFIG_LOG_DEFAULT_LEVEL);

#define EXPANDER_NODE DT_NODELABEL(tca9535)

#if !DT_NODE_HAS_STATUS_OKAY(EXPANDER_NODE)
#error "tca9535 device tree node is not enabled or present!"
#endif


static const struct device *const gpio_expander_dev = DEVICE_DT_GET(EXPANDER_NODE);

static const struct device *const vusb_regulators[HUB_CHANNEL_MAX] = {
        [HUB_CHANNEL_1] = DEVICE_DT_GET(DT_ALIAS(vusb1_pwr)),
        [HUB_CHANNEL_2] = DEVICE_DT_GET(DT_ALIAS(vusb2_pwr)),
        [HUB_CHANNEL_3] = DEVICE_DT_GET(DT_ALIAS(vusb3_pwr)),
        [HUB_CHANNEL_4] = DEVICE_DT_GET(DT_ALIAS(vusb4_pwr)),
        [HUB_CHANNEL_5] = DEVICE_DT_GET(DT_ALIAS(vusb5_pwr)),
        [HUB_CHANNEL_6] = DEVICE_DT_GET(DT_ALIAS(vusb6_pwr)),
        [HUB_CHANNEL_7] = DEVICE_DT_GET(DT_ALIAS(vusb7_pwr)),
};

static const struct gpio_dt_spec status_leds[GREEN_LED_MAX] = {
        [GREEN_LED_1] = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green_led1), gpios, {0}),
        [GREEN_LED_2] = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green_led2), gpios, {0}),
        [GREEN_LED_3] = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green_led3), gpios, {0}),
        [GREEN_LED_4] = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green_led4), gpios, {0}),
        [GREEN_LED_5] = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green_led5), gpios, {0}),
        [GREEN_LED_6] = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green_led6), gpios, {0}),
        [GREEN_LED_7] = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green_led7), gpios, {0})
};


int ext_io_init(void) {
        if (!device_is_ready(gpio_expander_dev)) {
                LOG_ERR("TCA9535 device not ready");
                return -ENODEV;
        }

        int ret;
        /* USB power regulators */
        for (hub_channel_t channel = HUB_CHANNEL_1;
                channel < HUB_CHANNEL_MAX;
                channel++) {

                if (!device_is_ready(vusb_regulators[channel])) {
                        LOG_ERR("VUSB regulator %d not ready", channel);
                        return -ENODEV;
                }
        }

        // Green leds
	for (status_led_t led = GREEN_LED_1; led < GREEN_LED_MAX; led++) {
		if (!gpio_is_ready_dt(&status_leds[led])) {
			LOG_ERR("GPIO status led %d not ready", led);
                        return -ENODEV;
                }

                ret = gpio_pin_configure_dt(&status_leds[led], GPIO_OUTPUT_INACTIVE);

                if (ret < 0) {
                        LOG_ERR("Failed to configure GPIO for LED %d: %d", led, ret);
                        return ret;
                }
        }

        LOG_INF("Ext IO driver initialized.");
        return 0;
}


int ext_io_set_vusb_power(hub_channel_t channel, bool enable) {
        if (channel >= HUB_CHANNEL_MAX) {
                return -EINVAL;
        }

        if (enable) {
                return regulator_enable(vusb_regulators[channel]);
        }

        return regulator_disable(vusb_regulators[channel]);
}


int ext_io_set_vusb_power_mask(uint8_t mask, bool enable)
{
        for (hub_channel_t channel = HUB_CHANNEL_1;
             channel < HUB_CHANNEL_MAX;
             channel++) {

                uint8_t bit = channel - HUB_CHANNEL_1;

                if (!(mask & BIT(bit))) {
                        continue;
                }

                int ret = ext_io_set_vusb_power(channel, enable);
                if (ret < 0) {
                        LOG_ERR("Failed to %s VUSB channel %d: %d",
                                enable ? "enable" : "disable",
                                channel,
                                ret);
                        return ret;
                }
        }

        return 0;
}


int ext_io_set_led_power(status_led_t led_num, bool enable) {

        if (led_num < GREEN_LED_1 || led_num >= GREEN_LED_MAX) {
                return -EINVAL;
        }
	return gpio_pin_set_dt(&status_leds[led_num], enable ? 1 : 0);
}


int ext_io_set_led_power_mask(uint8_t mask, bool enable) {

        int ret;
        size_t mask_bit = 0;
        for (status_led_t led = GREEN_LED_1; led < GREEN_LED_MAX; led++, mask_bit++) {
                if (mask & BIT(mask_bit)) {
                        ret = gpio_pin_set_dt(&status_leds[led], enable ? 1 : 0);
                        if (ret < 0) {
                                return ret;
                        }
                }
        }
        return 0;
}

int ext_io_led_test(void)
{
	int ret;

	/* LED1 -> LED7: turn on one by one */
	for (status_led_t led = GREEN_LED_1;
	     led < GREEN_LED_MAX;
	     led++) {

		ret = ext_io_set_led_power(led, true);
		if (ret < 0) {
			LOG_ERR("Failed to turn on LED %d: %d", led, ret);
			return ret;
		}

		k_msleep(100);
	}

	/* LED7 -> LED1: turn off one by one */
	for (status_led_t led = GREEN_LED_MAX - 1;
	     led >= GREEN_LED_1;
	     led--) {

		ret = ext_io_set_led_power(led, false);
		if (ret < 0) {
			LOG_ERR("Failed to turn off LED %d: %d", led, ret);
			return ret;
		}

		k_msleep(100);
	}

	/* Turn all LEDs on */
	for (status_led_t led = GREEN_LED_1;
	     led < GREEN_LED_MAX;
	     led++) {

		ret = ext_io_set_led_power(led, true);
		if (ret < 0) {
			LOG_ERR("Failed to turn on LED %d: %d", led, ret);
			return ret;
		}
	}

	/* Keep all LEDs on for 300 ms */
	k_msleep(300);

	/* Turn all LEDs off */
	for (status_led_t led = GREEN_LED_1;
	     led < GREEN_LED_MAX;
	     led++) {

		ret = ext_io_set_led_power(led, false);
		if (ret < 0) {
			LOG_ERR("Failed to turn off LED %d: %d", led, ret);
			return ret;
		}
	}

	return 0;
}