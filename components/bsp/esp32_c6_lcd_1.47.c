#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "iot_button.h"
#include "button_gpio.h"
#include "led_strip.h"
#include "led_strip_interface.h"

#include "bsp/esp32_c6_lcd_1.47.h"

static const char *TAG = "ESP32-C6-LCD-1.47";

static led_strip_handle_t led_strip;

/**
 * @brief led configuration structure
 *
 * This configuration is used by default in bsp_led_init()
 */
static const led_strip_config_t bsp_strip_config = {
	.strip_gpio_num = BSP_RGB_CTRL,
	.max_leds = 1,
	.led_model = LED_MODEL_WS2812,
	.flags.invert_out = false,
};

static const led_strip_rmt_config_t bsp_rmt_config = {
	.clk_src = RMT_CLK_SRC_DEFAULT,
	.resolution_hz = 10 * 1000 * 1000,
	.flags.with_dma = false,
};

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
	ESP_LOGI(TAG, "GPIO setting %d", bsp_strip_config.strip_gpio_num);

	ESP_ERROR_CHECK(led_strip_new_rmt_device(&bsp_strip_config, &bsp_rmt_config, &led_strip));
	ESP_ERROR_CHECK(bsp_led_rgb_set(0, 0, 0));

	return ESP_OK;
}

static const button_gpio_config_t bsp_button_config[BSP_BUTTON_NUM] = { {
	.gpio_num = BSP_BUTTON_MAIN_IO,
	.active_level = 0,
} };

esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size)
{
	esp_err_t ret = ESP_OK;
	const button_config_t btn_config = { 0 };

	if ((btn_array_size < BSP_BUTTON_NUM) || (btn_array == NULL))
		return ESP_ERR_INVALID_ARG;

	if (btn_cnt)
		*btn_cnt = 0;

	for (int i = 0; i < BSP_BUTTON_NUM; i++) {
		ret |= iot_button_new_gpio_device(&btn_config, &bsp_button_config[i], &btn_array[i]);
		if (btn_cnt)
			(*btn_cnt)++;
	}

	return ret;
}
