/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/display/widgets/key_position_status.h>
#include <zmk/display/widgets/modifiers_status.h>
#include <zmk/display/widgets/stick_xy_status.h>
#include <zmk/display/status_screen.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_KEY_POSITION_STATUS)
static struct zmk_widget_key_position_status key_position_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_MODIFIERS_STATUS)
static struct zmk_widget_modifiers_status modifiers_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_STICK_XY_STATUS)
static struct zmk_widget_stick_xy_status stick_xy_status_widget;
#endif

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *status_screen;
    status_screen = lv_obj_create(NULL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, status_screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_status_widget, status_screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
    zmk_widget_peripheral_status_init(&peripheral_status_widget, status_screen);
    lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_LEFT, 0,
                 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_status_widget, status_screen);
    lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
                               lv_theme_get_font_small(status_screen), LV_PART_MAIN);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
    zmk_widget_wpm_status_init(&wpm_status_widget, status_screen);
    lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_KEY_POSITION_STATUS)
    zmk_widget_key_position_status_init(&key_position_status_widget, status_screen);
    lv_obj_set_style_text_font(zmk_widget_key_position_status_obj(&key_position_status_widget),
                              lv_theme_get_font_small(status_screen), LV_PART_MAIN);
    lv_obj_align(zmk_widget_key_position_status_obj(&key_position_status_widget),
                 LV_ALIGN_BOTTOM_MID, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_MODIFIERS_STATUS)
    zmk_widget_modifiers_status_init(&modifiers_status_widget, status_screen);
    lv_obj_set_style_text_font(zmk_widget_modifiers_status_obj(&modifiers_status_widget),
                               lv_theme_get_font_small(status_screen), LV_PART_MAIN);
    lv_obj_align(zmk_widget_modifiers_status_obj(&modifiers_status_widget),
                 LV_ALIGN_LEFT_MID, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_STICK_XY_STATUS)
    zmk_widget_stick_xy_status_init(&stick_xy_status_widget, status_screen);
    lv_obj_set_style_text_font(zmk_widget_stick_xy_status_obj(&stick_xy_status_widget),
                              lv_theme_get_font_small(status_screen), LV_PART_MAIN);
    lv_obj_align(zmk_widget_stick_xy_status_obj(&stick_xy_status_widget),
                 LV_ALIGN_RIGHT_MID, 0, 0);
#endif

    return status_screen;
}
