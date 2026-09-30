#include "display_mgr.h"
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(display, LOG_LEVEL_INF);


static const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

static const struct pwm_dt_spec backlight =
    PWM_DT_SPEC_GET(DT_NODELABEL(lcd_backlight));

int display_mgr_init(void) {
        int ret;

        if (!device_is_ready(display_dev)) {
                LOG_ERR("Failed to retrieve the display node.");
                return -ENODEV;
        }

        if (!pwm_is_ready_dt(&backlight)) {
                LOG_ERR("Failed to retrieve the LCD backlight node.");
                return -ENODEV;
        }

        /* set backlight to 80% on boot */
        ret = display_mgr_set_backlight(80);
        if (ret < 0) {
                LOG_ERR("Failed to set the backlight during initialization");
                return ret;
        }
        
        return 0;
}


int display_mgr_set_backlight(uint8_t brightness) {
        if (brightness > 100)
                brightness = 100;

        uint32_t pulse = (backlight.period * brightness) / 100U;

        LOG_INF("Backlight: %u%%, period=%u ns, pulse=%u ns",
            brightness, backlight.period, pulse);

        return pwm_set_pulse_dt(&backlight, pulse);
}

const struct device *display_mgr_get_device(void) {
    return display_dev;
}




