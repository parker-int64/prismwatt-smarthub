#ifndef APP_UI_H_
#define APP_UI_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Draw the initial UI: a white screen with a centered "Hello World" text.
 * Uses Zephyr's display write API; the display device and backlight must
 * already be initialized (see display_mgr_init()).
 */
int app_ui_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_UI_H_ */
