/*
 * Stick X/Y status widget - shows analog stick coordinates (ADC channels 5 and 7).
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_stick_xy_status {
	sys_snode_t node;
	lv_obj_t *obj;
};

int zmk_widget_stick_xy_status_init(struct zmk_widget_stick_xy_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_stick_xy_status_obj(struct zmk_widget_stick_xy_status *widget);
