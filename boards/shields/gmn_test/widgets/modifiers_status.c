/*
 * Modifiers status widget - shows Ctrl/Shift/Alt/Gui state.
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/modifiers_state_changed.h>
#include <zmk/hid.h>
#include <dt-bindings/zmk/modifiers.h>
#include <zmk/display/widgets/modifiers_status.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

/* MOD_LCTL 0x01, MOD_LSFT 0x02, MOD_LALT 0x04, MOD_LGUI 0x08 (and right variants) */
#define MOD_CTL (MOD_LCTL | MOD_RCTL)
#define MOD_SFT (MOD_LSFT | MOD_RSFT)
#define MOD_ALT (MOD_LALT | MOD_RALT)
#define MOD_GUI (MOD_LGUI | MOD_RGUI)

struct modifiers_state {
	zmk_mod_flags_t mods;
};

static void set_modifiers_text(lv_obj_t *label, struct modifiers_state state) {
	char text[16];
	zmk_mod_flags_t m = state.mods;
	snprintf(text, sizeof(text), "%c%c%c%c",
		 (m & MOD_CTL) ? 'C' : '-',
		 (m & MOD_SFT) ? 'S' : '-',
		 (m & MOD_ALT) ? 'A' : '-',
		 (m & MOD_GUI) ? 'G' : '-');
	lv_label_set_text(label, text);
}

static void modifiers_update_cb(struct modifiers_state state) {
	struct zmk_widget_modifiers_status *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		set_modifiers_text(widget->obj, state);
	}
}

static struct modifiers_state modifiers_get_state(const zmk_event_t *eh) {
	struct modifiers_state state = { .mods = 0 };
	const struct zmk_modifiers_state_changed *ev = as_zmk_modifiers_state_changed(eh);
	if (ev != NULL) {
		/* Event gives (which mods changed, press/release). Use current HID state for display. */
		state.mods = zmk_hid_get_explicit_mods();
	} else {
		state.mods = zmk_hid_get_explicit_mods();
	}
	return state;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_modifiers_status, struct modifiers_state,
			     modifiers_update_cb, modifiers_get_state)
ZMK_SUBSCRIPTION(widget_modifiers_status, zmk_modifiers_state_changed);

int zmk_widget_modifiers_status_init(struct zmk_widget_modifiers_status *widget, lv_obj_t *parent) {
	widget->obj = lv_label_create(parent);
	lv_label_set_text(widget->obj, "----");
	sys_slist_append(&widgets, &widget->node);
	widget_modifiers_status_init();
	return 0;
}

lv_obj_t *zmk_widget_modifiers_status_obj(struct zmk_widget_modifiers_status *widget) {
	return widget->obj;
}
