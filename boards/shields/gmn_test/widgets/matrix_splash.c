/*
 * Matrix rain splash screen - real-time falling columns, 5s then switch.
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zmk/display.h>
#include "matrix_splash.h"
#include <lvgl.h>
#include <string.h>

#define MATRIX_W  128
#define MATRIX_H  64
#define COL_W    8
#define ROW_H    8
#define COLS     (MATRIX_W / COL_W)
#define ROWS     (MATRIX_H / ROW_H)

#define SPLASH_DURATION_MS  (5 * 1000)
#define FRAME_INTERVAL_MS   80

#if (LV_COLOR_DEPTH != 1)
#error "matrix_splash expects LV_COLOR_DEPTH_1 (SSD1306)"
#endif
#define CANVAS_BUF_SIZE ((MATRIX_W * MATRIX_H * LV_COLOR_DEPTH + 7) / 8)

static const char matrix_chars[] = "0123456789ABCDEF";
#define NUM_CHARS (sizeof(matrix_chars) - 1)

static lv_obj_t *splash_screen;
static lv_obj_t *canvas;
static lv_obj_t *next_screen_to_load;
static uint8_t canvas_buf[CANVAS_BUF_SIZE];

/* Per-column state */
static int16_t head_y[COLS];
static uint8_t speed[COLS];
static uint8_t length[COLS];
static uint32_t rnd_state[COLS];
static bool splash_done;

static uint32_t prng_next(uint32_t *s) {
	*s = *s * 1103515245U + 12345U;
	return *s;
}

static void draw_frame(void) {
	if (splash_done || canvas == NULL) {
		return;
	}

	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.bg_color = lv_color_black();
	lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

	lv_draw_label_dsc_t label_dsc;
	lv_draw_label_dsc_init(&label_dsc);
	label_dsc.color = lv_color_white();
	label_dsc.font = &lv_font_montserrat_8;
	label_dsc.align = LV_TEXT_ALIGN_LEFT;

	for (int c = 0; c < COLS; c++) {
		for (uint8_t i = 0; i < length[c]; i++) {
			int16_t y = head_y[c] - (int16_t)(i * ROW_H);
			if (y < 0 || y >= MATRIX_H) {
				continue;
			}
			uint32_t r = rnd_state[c];
			unsigned idx = prng_next(&rnd_state[c]) % NUM_CHARS;
			char ch[2] = { matrix_chars[idx], '\0' };
			lv_canvas_draw_text(canvas, c * COL_W, y, COL_W, &label_dsc, ch);
		}
		head_y[c] += (int16_t)speed[c];
		if (head_y[c] > MATRIX_H + (int16_t)(length[c] * ROW_H)) {
			head_y[c] = -(int16_t)(length[c] * ROW_H);
			prng_next(&rnd_state[c]);
		}
	}
}

static void switch_to_status_cb(struct k_work *work) {
	(void)work;
	splash_done = true;
	if (next_screen_to_load != NULL) {
		lv_scr_load(next_screen_to_load);
		next_screen_to_load = NULL;
	}
}

static void frame_tick_cb(struct k_work *work) {
	(void)work;
	draw_frame();
	if (!splash_done) {
		k_work_schedule_for_queue(zmk_display_work_q(), work, K_MSEC(FRAME_INTERVAL_MS));
	}
}

K_WORK_DELAYABLE_DEFINE(switch_to_status_work, switch_to_status_cb);
K_WORK_DELAYABLE_DEFINE(frame_tick_work, frame_tick_cb);

lv_obj_t *matrix_splash_screen_create(lv_obj_t *next_screen) {
	next_screen_to_load = next_screen;
	splash_done = false;

	splash_screen = lv_obj_create(NULL);
	lv_obj_set_size(splash_screen, MATRIX_W, MATRIX_H);
	lv_obj_set_style_bg_color(splash_screen, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_border_width(splash_screen, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_all(splash_screen, 0, LV_PART_MAIN);

	canvas = lv_canvas_create(splash_screen);
	lv_canvas_set_buffer(canvas, canvas_buf, MATRIX_W, MATRIX_H, LV_IMG_CF_TRUE_COLOR);
	lv_obj_set_size(canvas, MATRIX_W, MATRIX_H);
	lv_obj_align(canvas, LV_ALIGN_TOP_LEFT, 0, 0);

	/* Initialize column state with simple LCG seed from k_cyc_to_us */
	uint32_t seed = (uint32_t)k_cyc_to_us_floor64(k_cycle_get_32());
	for (int c = 0; c < COLS; c++) {
		seed = seed * 1103515245U + 12345U;
		rnd_state[c] = seed;
		length[c] = 4 + (seed % 5);
		speed[c] = 1 + (seed % 2);
		head_y[c] = -(int16_t)(length[c] * ROW_H) + (seed % (MATRIX_H + length[c] * ROW_H));
	}

	draw_frame();
	k_work_schedule_for_queue(zmk_display_work_q(), &frame_tick_work, K_MSEC(FRAME_INTERVAL_MS));
	k_work_schedule_for_queue(zmk_display_work_q(), &switch_to_status_work, K_MSEC(SPLASH_DURATION_MS));

	return splash_screen;
}
