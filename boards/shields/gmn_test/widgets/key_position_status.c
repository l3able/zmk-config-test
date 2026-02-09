/*
 * Key position status widget - shows last pressed key as row/column.
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/matrix.h>
#include <zmk/display/widgets/key_position_status.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct key_position_state {
	uint32_t position;
	bool pressed;
};

static void set_key_position_text(lv_obj_t *label, struct key_position_state state) {
	char text[24];

	if (!state.pressed) {
		lv_label_set_text(label, "Key: --");
		return;
	}

	uint32_t pos = state.position;
	if (pos < ZMK_KEYMAP_LEN) {
		uint32_t row = pos / ZMK_MATRIX_COLS;
		uint32_t col = pos % ZMK_MATRIX_COLS;
		snprintf(text, sizeof(text), "R%u C%u", (unsigned)row, (unsigned)col);
	} else {
		snprintf(text, sizeof(text), "Pos %u", (unsigned)pos);
	}
	lv_label_set_text(label, text);
}

static void key_position_update_cb(struct key_position_state state) {
	struct zmk_widget_key_position_status *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		set_key_position_text(widget->obj, state);
	}
}

static struct key_position_state key_position_get_state(const zmk_event_t *eh) {
	struct key_position_state state = { .position = 0, .pressed = false };
	const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);
	if (ev != NULL) {
		state.position = ev->position;
		state.pressed = ev->state;
	}
	return state;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_key_position_status, struct key_position_state,
			     key_position_update_cb, key_position_get_state)
ZMK_SUBSCRIPTION(widget_key_position_status, zmk_position_state_changed);

int zmk_widget_key_position_status_init(struct zmk_widget_key_position_status *widget,
					lv_obj_t *parent) {
	widget->obj = lv_label_create(parent);
	lv_label_set_text(widget->obj, "Key: --");
	sys_slist_append(&widgets, &widget->node);
	widget_key_position_status_init();
	return 0;
}

lv_obj_t *zmk_widget_key_position_status_obj(struct zmk_widget_key_position_status *widget) {
	return widget->obj;
}
