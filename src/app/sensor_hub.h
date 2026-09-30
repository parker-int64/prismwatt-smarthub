#ifndef SENSOR_HUB_H_
#define SENSOR_HUB_H_

/**
 * @brief poll function in the main loop 
 */
void sensor_hub_poll(void);

/**
 * @brief read the new data in a thread
 */
void sensor_hub_get_data();


#endif // SENSOR_HUB_H_