#include "esp_err.h"
#include "esp_log.h"

#include "iot_button.h"
#include "button_gpio.h"
#include "led_strip.h"
#include "led_strip_interface.h"
#include "led_indicator_strips.h"

#include "bsp/esp32_c6_lcd_1.47.h"

static const char *TAG = "ESP32-C6-LCD-1.47";

typedef enum {
	BSP_BUTTON_TYPE_GPIO,
} bsp_button_type_t;

typedef struct {
	bsp_button_type_t type;
	union {
		button_gpio_config_t gpio;
	} cfg;
} bsp_button_config_t;

static const bsp_button_config_t bsp_button_config[BSP_BUTTON_NUM] = {
	[BSP_BUTTON_BOOT] = {
		.type = BSP_BUTTON_TYPE_GPIO,
		.cfg.gpio = {
			.gpio_num = BSP_BUTTON_BOOT_IO,
			.active_level = 0,
		}
	},
};

esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size)
{
	esp_err_t ret = ESP_OK;
	const button_config_t btn_config = { 0 };

	if ((btn_array_size < BSP_BUTTON_NUM) || (btn_array == NULL))
		return ESP_ERR_INVALID_ARG;

	if (btn_cnt)
		*btn_cnt = 0;

	for (int i = 0; i < BSP_BUTTON_NUM; i++) {
		if (bsp_button_config[i].type == BSP_BUTTON_TYPE_GPIO)
			ret |= iot_button_new_gpio_device(&btn_config, &bsp_button_config[i].cfg.gpio, &btn_array[i]);
		else
			ESP_LOGW(TAG, "Unsupported button type!");

		if (btn_cnt)
			(*btn_cnt)++;
	}

	return ret;
}

static const led_strip_config_t bsp_leds_rgb_strip_config = {
	.strip_gpio_num = BSP_LED_RGB_GPIO, // The GPIO that connected to the LED strip's data line
	.max_leds = BSP_LED_NUM, // The number of LEDs in the strip,
	.led_model = LED_MODEL_WS2812, // LED strip model
	.flags.invert_out = false, // whether to invert the output signal
};

static const led_strip_rmt_config_t bsp_leds_rgb_rmt_config = {
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
	.rmt_channel = 0,
#else
	.clk_src = RMT_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
	.resolution_hz = 10 * 1000 * 1000, // RMT counter clock frequency = 10MHz
	.flags.with_dma = false, // DMA feature is available on ESP target like ESP32-S3
#endif
};

static led_indicator_strips_config_t bsp_leds_rgb_config = {
	.led_strip_cfg = bsp_leds_rgb_strip_config,
	.led_strip_driver = LED_STRIP_RMT,
	.led_strip_rmt_cfg = bsp_leds_rgb_rmt_config,
};

extern blink_step_t const *bsp_led_blink_defaults_lists[];

static const led_indicator_config_t bsp_leds_config = {
	.blink_lists = bsp_led_blink_defaults_lists,
	.blink_list_num = BSP_LED_MAX,
};

esp_err_t bsp_led_indicator_create(led_indicator_handle_t led_array[], int *led_cnt, int led_array_size)
{
	if (led_array == NULL)
		return ESP_ERR_INVALID_ARG;

	led_indicator_new_strips_device(&bsp_leds_config, &bsp_leds_rgb_config, &led_array[0]);
	if (!led_array[0])
		return ESP_FAIL;

	return ESP_OK;
}

#if 0
/* use indicator instead */
static led_strip_handle_t led_strip;

esp_err_t bsp_led_rgb_set(uint8_t r, uint8_t g, uint8_t b)
{
	esp_err_t ret;

	ret = led_strip_set_pixel(led_strip, 0, r, g, b);
	if (ret != ESP_OK)
		return ret;

	ret = led_strip_refresh(led_strip);
	return ret;
}

esp_err_t bsp_led_init()
{
	ESP_ERROR_CHECK(led_strip_new_rmt_device(&bsp_leds_rgb_strip_config, &bsp_leds_rgb_rmt_config, &led_strip));
	ESP_ERROR_CHECK(bsp_led_rgb_set(0, 0, 0));

	return ESP_OK;
}
#endif
