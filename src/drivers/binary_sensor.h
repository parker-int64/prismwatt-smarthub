#ifndef BINARY_SENSOR_H_
#define BINARY_SENSOR_H_

enum button_id {
    BTN_ENTER = 0,
    BTN_LEFT,
    BTN_RIGHT,
};

int gpio_keys_init(void);

#endif /* BINARY_SENSOR_H_ */