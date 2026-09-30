#ifndef EXT_IO_H_
#define EXT_IO_H_

#include <zephyr/drivers/gpio.h>

typedef enum HubChannel {
        HUB_CHANNEL_1 = 1,
        HUB_CHANNEL_2,
        HUB_CHANNEL_3,
        HUB_CHANNEL_4,
        HUB_CHANNEL_5,
        HUB_CHANNEL_6,
        HUB_CHANNEL_7,
        HUB_CHANNEL_MAX
} hub_channel_t;


typedef enum StatusLed {
        GREEN_LED_1 = 1,
        GREEN_LED_2,
        GREEN_LED_3,
        GREEN_LED_4,
        GREEN_LED_5,
        GREEN_LED_6,
        GREEN_LED_7,
        GREEN_LED_MAX
} status_led_t;

/**
 * @brief Initialize the external IO expander
 * @return int 0 on success, -1 on failure
 */
int ext_io_init(void);


/**
 * @brief Enable the 5V power supply on designated channel
 * @param channel VUSB channel number (1 - 7)
 * @param enable true enable，false disable
 * @return int 0 success
 */
int ext_io_set_vusb_power(hub_channel_t channel, bool enable);

/**
 * @brief Enable the 5V power supply by mask provided
 * @param mask channel mask (CHANNEL_N)
 * @param enable true enable，false disable
 * @return int 0 success
 */
int ext_io_set_vusb_power_mask(uint8_t mask, bool enable);


int ext_io_set_led_power(status_led_t led_num, bool enable);


int ext_io_set_led_power_mask(uint8_t mask, bool enable);


int ext_io_led_test(void);

#endif /* EXT_IO_H_ */