/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_err.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include "bsp/esp-bsp.h"

#include "led_indicator.h"
#include "led_indicator_blink_default.h"

#include "ST7789.h"
#include "SD_SPI.h"
#include "Wireless.h"
#include "LVGL_Example.h"

static const char *TAG = "main";

static int example_sel_effect = BSP_LED_BREATHE_SLOW;
static led_indicator_handle_t leds[BSP_LED_NUM];

/* Called on button press */
static void btn_handler(void *button_handle, void *usr_data)
{
	int button_pressed = (int)usr_data;
	ESP_LOGI(TAG, "Button pressed: %d. ", button_pressed);

	led_indicator_stop(leds[0], example_sel_effect);

	if (button_pressed == 0) {
		example_sel_effect++;
		if (example_sel_effect >= BSP_LED_MAX)
			example_sel_effect = BSP_LED_ON;
	}

	ESP_LOGI(TAG, "Changed LED blink effect: %d.", example_sel_effect);
	led_indicator_start(leds[0], example_sel_effect);
}

static void nvs_init(void)
{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);
}

void app_main(void)
{
	nvs_init();

	Wireless_Init();
	Flash_Searching();
	SD_Init(); // SD must be initialized behind the LCD
	LCD_Init();
	BK_Light(50);
	LVGL_Init(); // returns the screen object

	/* Init buttons */
	button_handle_t btns[BSP_BUTTON_NUM] = { NULL };

	ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));
	for (int i = 0; i < BSP_BUTTON_NUM; i++) {
		ESP_ERROR_CHECK(iot_button_register_cb(btns[i], BUTTON_PRESS_DOWN, NULL, btn_handler, (void *)i));
	}

	/* Init LEDs */
	ESP_ERROR_CHECK(bsp_led_indicator_create(leds, NULL, BSP_LED_NUM));

	/* Set LED color for first LED (only for addressable RGB LEDs) */
	led_indicator_set_rgb(leds[0], SET_IRGB(0, 0x00, 0x64, 0x64));

	/* Start effect for each LED
	  (predefined: BSP_LED_ON, BSP_LED_OFF, BSP_LED_BLINK_FAST, BSP_LED_BLINK_SLOW, BSP_LED_BREATHE_FAST,
	   BSP_LED_BREATHE_SLOW)
	 */
	led_indicator_start(leds[0], BSP_LED_BREATHE_SLOW);

	/********************* Demo *********************/
	Lvgl_Example1();

	// lv_demo_widgets();
	// lv_demo_keypad_encoder();
	// lv_demo_benchmark();
	// lv_demo_stress();
	// lv_demo_music();

	while (1) {
		// raise the task priority of LVGL and/or reduce the handler period can improve the performance
		vTaskDelay(pdMS_TO_TICKS(10));
		// The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
		lv_timer_handler();
	}
}
