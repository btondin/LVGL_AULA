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
#include "lv_font_tiny5.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);

#define ACCEL_SCALE 100
#define G_MS2       9.81

static lv_obj_t *chart1;
static lv_chart_series_t *ser_x;
static lv_chart_series_t *ser_y;
static lv_chart_series_t *ser_z;
static lv_obj_t *lbl_x;
static lv_obj_t *lbl_y;
static lv_obj_t *lbl_z;
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

	double ax = sensor_value_to_double(&accel[0]);
	double ay = sensor_value_to_double(&accel[1]);
	double az = sensor_value_to_double(&accel[2]);

	lv_chart_set_next_value(chart1, ser_x, (int32_t)(ax * ACCEL_SCALE));
	lv_chart_set_next_value(chart1, ser_y, (int32_t)(ay * ACCEL_SCALE));
	lv_chart_set_next_value(chart1, ser_z, (int32_t)(az * ACCEL_SCALE));

	/* Update legend with current values in G */
	lv_label_set_text_fmt(lbl_x, "X:%.1f", ax / G_MS2);
	lv_label_set_text_fmt(lbl_y, "Y:%.1f", ay / G_MS2);
	lv_label_set_text_fmt(lbl_z, "Z:%.1f", az / G_MS2);
}

static void create_legend(lv_obj_t *parent)
{
	lv_obj_t *legend = lv_obj_create(parent);
	lv_obj_remove_style_all(legend);
	lv_obj_set_size(legend, LV_HOR_RES, 8);
	lv_obj_align(legend, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_style_bg_color(legend, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(legend, LV_OPA_COVER, 0);
	lv_obj_set_flex_flow(legend, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(legend, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER,
			      LV_FLEX_ALIGN_CENTER);

	static const struct {
		const char *init;
		lv_color_t color;
		lv_obj_t **lbl;
	} items[] = {
		{"X:--", LV_COLOR_MAKE(0, 0, 0), &lbl_x},
		{"Y:--", LV_COLOR_MAKE(0, 0, 0), &lbl_y},
		{"Z:--", LV_COLOR_MAKE(0, 0, 0), &lbl_z},
	};

	for (int i = 0; i < 3; i++) {
		lv_obj_t *lbl = lv_label_create(legend);
		lv_label_set_text(lbl, items[i].init);
		lv_obj_set_style_text_font(lbl, &lv_font_tiny5, 0);
		lv_obj_set_style_text_color(lbl, items[i].color, 0);
		*items[i].lbl = lbl;
	}
}

#define CHART_TOP_MARGIN 8

/* Draw event: set line style per series (X=solid, Y=dotted, Z=dashed) */
static void chart_draw_event_cb(lv_event_t *e)
{
	lv_draw_task_t *task = lv_event_get_draw_task(e);

	if (lv_draw_task_get_type(task) != LV_DRAW_TASK_TYPE_LINE) {
		return;
	}

	lv_draw_line_dsc_t *line_dsc = lv_draw_task_get_line_dsc(task);
	if (line_dsc == NULL || line_dsc->base.part != LV_PART_ITEMS) {
		return;
	}

	/* id1: series index (0=X first added, 1=Y, 2=Z) */
	if (line_dsc->base.id1 == 1) {
		/* Y: dotted */
		line_dsc->dash_width = 1;
		line_dsc->dash_gap = 2;
	} else if (line_dsc->base.id1 == 2) {
		/* Z: dashed */
		line_dsc->dash_width = 4;
		line_dsc->dash_gap = 3;
	}
	/* X (id1==0): solid (default, no change needed) */
}

static void create_accelerometer_chart(lv_obj_t *parent)
{
	create_legend(parent);

	/* Fundo escuro na tela (mono: white=pixel OFF=escuro no OLED) */
	lv_obj_set_style_bg_color(parent, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

	chart1 = lv_chart_create(parent);
	lv_obj_set_size(chart1, LV_HOR_RES, LV_VER_RES - CHART_TOP_MARGIN);
	lv_obj_align(chart1, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_bg_color(chart1, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(chart1, LV_OPA_COVER, 0);
	lv_obj_set_style_border_width(chart1, 0, 0);
	lv_chart_set_type(chart1, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(chart1, 4, 5);
	lv_obj_set_style_line_width(chart1, 1, LV_PART_MAIN);
	lv_chart_set_range(chart1, LV_CHART_AXIS_PRIMARY_Y, -2000, 2000); /* -/+ 2G scaled x100 */
	lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_SHIFT);

	ser_x = lv_chart_add_series(chart1, lv_color_black(),
				    LV_CHART_AXIS_PRIMARY_Y);
	ser_y = lv_chart_add_series(chart1, lv_color_black(),
				    LV_CHART_AXIS_PRIMARY_Y);
	ser_z = lv_chart_add_series(chart1, lv_color_black(),
				    LV_CHART_AXIS_PRIMARY_Y);

	lv_chart_set_point_count(chart1, CONFIG_SAMPLE_CHART_POINTS_PER_SERIES);

	/* Do not display point markers on the data */
	lv_obj_set_style_size(chart1, 0, 0, LV_PART_INDICATOR);

	/* Enable draw task events and register callback for per-series line styles */
	lv_obj_add_flag(chart1, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
	lv_obj_add_event_cb(chart1, chart_draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
}

int main(void)
{
	const struct device *display_dev;

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

	create_accelerometer_chart(lv_screen_active());
	sensor_timer = lv_timer_create(sensor_timer_cb,
					200/ CONFIG_SAMPLE_ACCEL_SAMPLING_RATE,
					NULL);
	lv_timer_handler();
	display_blanking_off(display_dev);

	while (1) {
		uint32_t sleep_ms = lv_timer_handler();

		k_msleep(MIN(sleep_ms, INT32_MAX));
	}

	return 0;
}
