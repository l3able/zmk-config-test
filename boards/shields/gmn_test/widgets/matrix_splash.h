/*
 * Matrix rain splash screen - full-screen animation, then switch to next screen.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>

/** Create the matrix splash screen and start animation + 5s timer.
 *  After 5 seconds, lv_scr_load(next_screen) is called and animation stops.
 *  @param next_screen  Screen to show after splash (e.g. status screen)
 *  @return The splash screen (load this first with lv_scr_load)
 */
lv_obj_t *matrix_splash_screen_create(lv_obj_t *next_screen);
