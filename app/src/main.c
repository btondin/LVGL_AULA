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

#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);

#define ACCEL_SCALE      100   /* chart unit = m/s² × ACCEL_SCALE  → ±2G ≈ ±1962 */
#define G_MS2            9.81  /* 1 G in m/s² */
#define CHART_TOP_MARGIN  50   /* pixels reserved for button + G labels */

static lv_obj_t *chart1;
static lv_chart_series_t *ser_x;
static lv_chart_series_t *ser_y;
static lv_chart_series_t *ser_z;
static lv_obj_t *lbl_x;
static lv_obj_t *lbl_y;
static lv_obj_t *lbl_z;
static lv_timer_t *sensor_timer;

const struct device *accel_sensor;

/* Timer handler: fetches sensor data, updates chart and G-value labels */
static void sensor_timer_cb(lv_timer_t *timer)
{
	struct sensor_value accel[3];
	int rc = sensor_sample_fetch(accel_sensor);

	if (rc == 0) {
		rc = sensor_channel_get(accel_sensor, SENSOR_CHAN_ACCEL_XYZ, accel);
	}
	if (rc < 0) {
		LOG_ERR("Sensor update failed: %d", rc);
		return;
	}

	double ax = sensor_value_to_double(&accel[0]);
	double ay = sensor_value_to_double(&accel[1]);
	double az = sensor_value_to_double(&accel[2]);

	lv_chart_set_next_value(chart1, ser_x, (int32_t)(ax * ACCEL_SCALE));
	lv_chart_set_next_value(chart1, ser_y, (int32_t)(ay * ACCEL_SCALE));
	lv_chart_set_next_value(chart1, ser_z, (int32_t)(az * ACCEL_SCALE));

	/* Update G-value labels next to the button */
	lv_label_set_text_fmt(lbl_x, "X:%.2fG", ax / G_MS2);
	lv_label_set_text_fmt(lbl_y, "Y:%.2fG", ay / G_MS2);
	lv_label_set_text_fmt(lbl_z, "Z:%.2fG", az / G_MS2);
}

/* Button click counter */
static void btn_event_cb(lv_event_t *e)
{
	static int count = 0;
	lv_obj_t *btn = lv_event_get_target(e);
	lv_obj_t *label = lv_obj_get_child(btn, 0);

	count++;
	lv_label_set_text_fmt(label, "Touch: %d", count);
	LOG_INF("Button pressed %d times", count);
}

/*
 * Top UI row (CHART_TOP_MARGIN px tall):
 *   [X:0.00G]  ← stacked G labels (montserrat_12, colored)
 *   [Y:0.00G]                                    [Touch: N] ← button (top-right)
 *   [Z:0.00G]
 */
static void create_top_ui(lv_obj_t *parent)
{
	/* Touch test button — position kept at top-right */
	lv_obj_t *btn = lv_btn_create(parent);
	lv_obj_set_size(btn, 100, 40);
	lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, -5, 5);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t *btn_lbl = lv_label_create(btn);
	lv_label_set_text(btn_lbl, "Touch: 0");
	lv_obj_center(btn_lbl);

	/* G-value labels stacked vertically to the left of the button */
	static const struct {
		const char   *init;
		lv_palette_t  color;
		lv_obj_t    **lbl;
		int           y_off; /* pixel offset from top */
	} items[] = {
		{"X:--G", LV_PALETTE_RED,   &lbl_x,  4},
		{"Y:--G", LV_PALETTE_BLUE,  &lbl_y, 19},
		{"Z:--G", LV_PALETTE_GREEN, &lbl_z, 34},
	};

	for (int i = 0; i < 3; i++) {
		lv_obj_t *lbl = lv_label_create(parent);
		lv_label_set_text(lbl, items[i].init);
		lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
		lv_obj_set_style_text_color(lbl, lv_palette_main(items[i].color), 0);
		lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 5, items[i].y_off);
		*items[i].lbl = lbl;
	}
}

static void create_accelerometer_chart(lv_obj_t *parent)
{
	chart1 = lv_chart_create(parent);
	lv_obj_set_size(chart1, LV_HOR_RES, LV_VER_RES - CHART_TOP_MARGIN);
	lv_obj_align(chart1, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_chart_set_type(chart1, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(chart1, 5, 8);
	/* ±2G scaled by ACCEL_SCALE: 2 × 9.81 × 100 ≈ 1962, use ±2000 for headroom */
	lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_Y, -2000, 2000);
	lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_SHIFT);

	ser_x = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED),
				    LV_CHART_AXIS_PRIMARY_Y);
	ser_y = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_BLUE),
				    LV_CHART_AXIS_PRIMARY_Y);
	ser_z = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_GREEN),
				    LV_CHART_AXIS_PRIMARY_Y);

	lv_chart_set_point_count(chart1, CONFIG_SAMPLE_CHART_POINTS_PER_SERIES);

	/* Hide point markers */
	lv_obj_set_style_size(chart1, 0, 0, LV_PART_INDICATOR);
}

int main(void)
{
	const struct device *display_dev;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display not ready");
		return -ENODEV;
	}

	accel_sensor = DEVICE_DT_GET(DT_ALIAS(accel0));
	if (!device_is_ready(accel_sensor)) {
		LOG_ERR("Sensor %s not ready", accel_sensor->name);
		return -ENODEV;
	}

	lv_obj_t *scr = lv_screen_active();
	create_top_ui(scr);
	create_accelerometer_chart(scr);

	sensor_timer = lv_timer_create(sensor_timer_cb,
				       200 / CONFIG_SAMPLE_ACCEL_SAMPLING_RATE,
				       NULL);
	lv_timer_handler();
	display_blanking_off(display_dev);

	while (1) {
		uint32_t sleep_ms = lv_timer_handler();

		k_msleep(MIN(sleep_ms, INT32_MAX));
	}

	return 0;
}
