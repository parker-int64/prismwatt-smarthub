#ifndef DISPLAY_MGR_H_
#define DISPLAY_MGR_H_


#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

int display_mgr_init(void);

const struct device *display_mgr_get_device(void);

int display_mgr_set_backlight(uint8_t brightness);


#ifdef __cplusplus
}
#endif


#endif // DISPLAY_MGR_H_