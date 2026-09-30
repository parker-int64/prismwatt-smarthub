#include "binary_sensor.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(gpio_keys, LOG_LEVEL_INF);

#define ENTER_KEY_NODE DT_ALIAS(enter_key)
#define LEFT_KEY_NODE DT_ALIAS(left_key)
#define RIGHT_KEY_NODE DT_ALIAS(right_key)

static const struct gpio_dt_spec gpio_buttons[] = {
    GPIO_DT_SPEC_GET(ENTER_KEY_NODE, gpios),
    GPIO_DT_SPEC_GET(LEFT_KEY_NODE,  gpios),
    GPIO_DT_SPEC_GET(RIGHT_KEY_NODE, gpios),
};


static struct gpio_callback button_cb_data[ARRAY_SIZE(gpio_buttons)];


static void buttons_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    for (size_t i = 0; i < ARRAY_SIZE(gpio_buttons); i++) {
        if (dev == gpio_buttons[i].port && (pins & BIT(gpio_buttons[i].pin))) {
            switch (i) {
            case BTN_ENTER:
                LOG_INF("ENTER key pressed!");
                break;
            case BTN_LEFT:
                LOG_INF("LEFT key pressed!");
                break;
            case BTN_RIGHT:
                LOG_INF("RIGHT key pressed!");
                break;
            default:
                break;
            }
        }
    }
}


int gpio_keys_init(void) {
        int ret;

        for (size_t i = 0; i < ARRAY_SIZE(gpio_buttons); i++) {
                if(!gpio_is_ready_dt(&gpio_buttons[i])) {
                        LOG_ERR("GPIO device %s is not ready.", gpio_buttons[i].port->name);
                        return -1;
                }

                ret = gpio_pin_configure_dt(&gpio_buttons[i], GPIO_INPUT);

                if (ret < 0) {
                        LOG_ERR("Error %d: failed to configure pin %d", ret, gpio_buttons[i].pin);
                        return ret;
                }

                ret = gpio_pin_interrupt_configure_dt(&gpio_buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
                if (ret < 0) {
                        LOG_ERR("Error %d: failed to configure interrupt on pin %d", ret, gpio_buttons[i].pin);
                        return ret;
                }

                /* Initialize and bind teh unified callbacks  */
                gpio_init_callback(&button_cb_data[i], buttons_callback, BIT(gpio_buttons[i].pin));
                gpio_add_callback(gpio_buttons[i].port, &button_cb_data[i]);

        }

        LOG_INF("GPIO buttons initialized successfully.");

        return 0;
}

