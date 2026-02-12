/*
 * LVGL usage example on Zephyr RTOS
 *
 * Copyright (c) 2023 Benjamin Cabé <benjamin@zephyrproject.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);

static lv_obj_t *chart1;
static lv_chart_series_t *ser_x;
static lv_chart_series_t *ser_y;
static lv_chart_series_t *ser_z;
static lv_timer_t *sensor_timer;

const struct device *accel_sensor;

/* Timer handler: fetches sensor data and appends it to the chart */
static void sensor_timer_cb(lv_timer_t *timer)
{
	struct sensor_value accel[3];
	int rc = sensor_sample_fetch(accel_sensor);

	if (rc == 0) {
		rc = sensor_channel_get(accel_sensor, SENSOR_CHAN_ACCEL_XYZ, accel);
	}
	if (rc < 0) {
		LOG_ERR("ERROR: Update failed: %d\n", rc);
		return;
	}
	lv_chart_set_next_value(chart1, ser_x, sensor_value_to_double(&accel[0]));
	lv_chart_set_next_value(chart1, ser_y, sensor_value_to_double(&accel[1]));
	lv_chart_set_next_value(chart1, ser_z, sensor_value_to_double(&accel[2]));
}

/* Callback do botao de teste do touch */
static void btn_event_cb(lv_event_t *e)
{
	static int count = 0;
	lv_obj_t *btn = lv_event_get_target(e);
	lv_obj_t *label = lv_obj_get_child(btn, 0);

	count++;
	lv_label_set_text_fmt(label, "Touch: %d", count);
	LOG_INF("Button pressed %d times", count);
}

static void create_touch_test_btn(lv_obj_t *parent)
{
	lv_obj_t *btn = lv_btn_create(parent);
	lv_obj_set_size(btn, 100, 40);
	lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, -5, 5);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t *label = lv_label_create(btn);
	lv_label_set_text(label, "Touch: 0");
	lv_obj_center(label);
}

static void create_accelerometer_chart(lv_obj_t *parent)
{
	chart1 = lv_chart_create(parent);
	lv_obj_set_size(chart1, LV_HOR_RES, LV_VER_RES - 50);
	lv_obj_align(chart1, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_chart_set_type(chart1, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(chart1, 5, 8);
	lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_Y, -20, 20); /* roughly -/+ 2G */
	lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_SHIFT);

	ser_x = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED),
				    LV_CHART_AXIS_PRIMARY_Y);
	ser_y = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_BLUE),
				    LV_CHART_AXIS_PRIMARY_Y);
	ser_z = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_GREEN),
				    LV_CHART_AXIS_PRIMARY_Y);

	lv_chart_set_point_count(chart1, CONFIG_SAMPLE_CHART_POINTS_PER_SERIES);

	/* Do not display point markers on the data */
	lv_obj_set_style_size(chart1, 0, 0, LV_PART_INDICATOR);
}

/* Scan do barramento I2C - lista todos os enderecos que respondem */
static void i2c_scan(const struct device *i2c_dev)
{
	uint8_t dummy;

	LOG_INF("I2C scan on %s...", i2c_dev->name);
	for (uint8_t addr = 0x08; addr < 0x78; addr++) {
		int ret = i2c_read(i2c_dev, &dummy, 1, addr);
		if (ret == 0) {
			LOG_INF("  Found device at 0x%02X", addr);
		}
	}
	LOG_INF("I2C scan done.");
}

int main(void)
{
	const struct device *display_dev;

	/* I2C scan para debug - remover depois */
	const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
	if (device_is_ready(i2c_dev)) {
		i2c_scan(i2c_dev);
	} else {
		LOG_ERR("I2C0 not ready");
	}

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return -ENODEV;
	}

	accel_sensor = DEVICE_DT_GET(DT_ALIAS(accel0));
	if (!device_is_ready(accel_sensor)) {
		LOG_ERR("Device %s is not ready\n", accel_sensor->name);
		return -ENODEV;
	}

	create_touch_test_btn(lv_screen_active());
	create_accelerometer_chart(lv_screen_active());
	sensor_timer = lv_timer_create(sensor_timer_cb,
					1000 / CONFIG_SAMPLE_ACCEL_SAMPLING_RATE,
					NULL);
	lv_timer_handler();
	display_blanking_off(display_dev);

	while (1) {
		uint32_t sleep_ms = lv_timer_handler();

		k_msleep(MIN(sleep_ms, INT32_MAX));
	}

	return 0;
}
