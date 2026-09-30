cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(prism_watt_smart_hub)

target_sources(app PRIVATE
	src/main.c
	src/app/app_ui.c
	src/app/ext_io.c
	src/drivers/adc_sensor.c
	src/drivers/binary_sensor.c
	src/drivers/display_mgr.c
)