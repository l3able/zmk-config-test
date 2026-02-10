/*
 * Stick X/Y status widget - shows analog stick coordinates (mV) from ADC ch 5 (X) and ch 7 (Y).
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/devicetree.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/stick_xy_status.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

#define STICK_ADC_NODE DT_NODELABEL(adc)

#if DT_NODE_EXISTS(STICK_ADC_NODE)
#define STICK_ADC_DEVICE DEVICE_DT_GET(STICK_ADC_NODE)
#else
#define STICK_ADC_DEVICE NULL
#endif

#define STICK_POLL_MS 200

struct stick_xy_state {
	int x_mv;
	int y_mv;
};

static void set_stick_xy_text(lv_obj_t *label, struct stick_xy_state state) {
	char text[24];
	snprintf(text, sizeof(text), "X:%d Y:%d", state.x_mv, state.y_mv);
	lv_label_set_text(label, text);
}

#if DT_NODE_EXISTS(STICK_ADC_NODE)

static int read_adc_channel(const struct device *adc_dev, uint8_t channel_id, int *mv) {
	int ret;
	struct adc_channel_cfg ch_cfg = {
		.gain = ADC_GAIN_1_6,
		.reference = ADC_REF_INTERNAL,
		.acquisition_time = ADC_ACQ_TIME_DEFAULT,
		.channel_id = channel_id,
		.differential = 0,
	};
	struct adc_sequence seq = {
		.channels = BIT(channel_id),
		.buffer = &(int16_t){0},
		.buffer_size = sizeof(int16_t),
		.resolution = 12,
	};

	ret = adc_channel_setup(adc_dev, &ch_cfg);
	if (ret) {
		return ret;
	}
	ret = adc_read(adc_dev, &seq);
	if (ret) {
		return ret;
	}
	int32_t raw_mv = *(int16_t *)seq.buffer;
	adc_raw_to_millivolts(600, ADC_GAIN_1_6, 12, &raw_mv);
	*mv = (int)raw_mv;
	return 0;
}

static void stick_poll_work_cb(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(stick_poll_work, stick_poll_work_cb);

static struct stick_xy_state stick_last_state;

static void stick_display_work_cb(struct k_work *work) {
	if (!zmk_display_is_initialized()) {
		return;
	}
	struct zmk_widget_stick_xy_status *widget;
	SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		set_stick_xy_text(widget->obj, stick_last_state);
	}
}
static K_WORK_DEFINE(stick_display_work, stick_display_work_cb);

static void stick_poll_work_cb(struct k_work *work) {
	const struct device *adc_dev = STICK_ADC_DEVICE;
	int x_mv = 0, y_mv = 0;

	if (adc_dev && device_is_ready(adc_dev)) {
		read_adc_channel(adc_dev, 5, &x_mv);
		read_adc_channel(adc_dev, 7, &y_mv);
	}
	stick_last_state.x_mv = x_mv;
	stick_last_state.y_mv = y_mv;
	k_work_submit_to_queue(zmk_display_work_q(), &stick_display_work);
	k_work_schedule(&stick_poll_work, K_MSEC(STICK_POLL_MS));
}

#endif /* DT_NODE_EXISTS(STICK_ADC_NODE) */

int zmk_widget_stick_xy_status_init(struct zmk_widget_stick_xy_status *widget, lv_obj_t *parent) {
	widget->obj = lv_label_create(parent);
	lv_label_set_text(widget->obj, "X:0 Y:0");
	sys_slist_append(&widgets, &widget->node);

#if DT_NODE_EXISTS(STICK_ADC_NODE)
	k_work_schedule(&stick_poll_work, K_MSEC(STICK_POLL_MS));
#endif
	return 0;
}

lv_obj_t *zmk_widget_stick_xy_status_obj(struct zmk_widget_stick_xy_status *widget) {
	return widget->obj;
}
