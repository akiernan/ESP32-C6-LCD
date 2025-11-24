/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_err.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include "bsp/esp-bsp.h"

#include "ST7789.h"
#include "SD_SPI.h"
#include "RGB.h"
#include "Wireless.h"
#include "LVGL_Example.h"

static const char *TAG = "main";

/* Called on button press */
static void btn_handler(void *button_handle, void *usr_data)
{
	int button_index = (int)usr_data;
	ESP_LOGI(TAG, "Button %d pressed", button_index);
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
	button_handle_t btns[BSP_BUTTON_NUM] = { NULL };

	nvs_init();

	Wireless_Init();
	Flash_Searching();
	bsp_led_init();
	RGB_Example();
	SD_Init(); // SD must be initialized behind the LCD
	LCD_Init();
	BK_Light(50);
	LVGL_Init(); // returns the screen object

	bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM);
	/* Register a callback for button press */
	for (int i = 0; i < BSP_BUTTON_NUM; i++)
		iot_button_register_cb(btns[i], BUTTON_PRESS_DOWN, NULL, btn_handler, (void *)i);

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
