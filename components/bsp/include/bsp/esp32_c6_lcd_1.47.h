/*
 * SPDX-FileCopyrightText: 2025 A Squared Projects Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP BSP: ESP32-C6-LCD-1.47
 */

#pragma once

#include "driver/gpio.h"
#include "iot_button.h"

/**************************************************************************************************
 *  BSP Board Name
 **************************************************************************************************/

/** @defgroup boardname Board Name
 *  @brief BSP Board Name
 *  @{
 */
#define BSP_BOARD_ESP32_C6_LCD_1_47
/** @} */ // end of boardname

/**************************************************************************************************
 *  BSP Capabilities
 **************************************************************************************************/

/** @defgroup capabilities Capabilities
 *  @brief BSP Capabilities
 *  @{
 */
#define BSP_CAPS_DISPLAY 1
#define BSP_CAPS_TOUCH 0
#define BSP_CAPS_BUTTONS 0
#define BSP_CAPS_KNOB 0
#define BSP_CAPS_AUDIO 0
#define BSP_CAPS_AUDIO_SPEAKER 0
#define BSP_CAPS_AUDIO_MIC 0
#define BSP_CAPS_LED 1
#define BSP_CAPS_SDCARD 0
#define BSP_CAPS_IMU 0
/** @} */ // end of capabilities

/**************************************************************************************************
 *  ESP32-C3-LCDkit pinout
 **************************************************************************************************/

/** @defgroup g04_display Display and Touch
 *  @brief Display BSP API
 *  @{
 */
#define BSP_LCD_DATA0 (GPIO_NUM_0)
#define BSP_LCD_PCLK (GPIO_NUM_1)
#define BSP_LCD_CS (GPIO_NUM_7)
#define BSP_LCD_DC (GPIO_NUM_2)
#define BSP_LCD_RST (GPIO_NUM_NC)
#define BSP_LCD_BACKLIGHT (GPIO_NUM_22)
/** @} */ // end of display

/** @defgroup g06_led Leds
 *  @brief Leds BSP API
 *  @{
 */
#define BSP_RGB_CTRL (GPIO_NUM_8)
/** @} */ // end of leds

/** @defgroup g05_buttons Buttons
 *  @brief Buttons BSP API
 *  @{
 */
#define BSP_BUTTON_BOOT_IO (GPIO_NUM_9)
/** @} */ // end of buttons

/** \addtogroup g05_buttons
 *  @brief BSP Buttons
 *  @{
 */
typedef enum { BSP_BUTTON_BOOT, BSP_BUTTON_NUM } bsp_button_t;

#ifdef __cplusplus
extern "C" {
#endif

#if 0
/** \addtogroup g04_display
 *  @{
 */

/**************************************************************************************************
 *
 * LCD interface
 *
 * ESP32-C3-LCDkit is shipped with 1.28inch GC9A01 display controller.
 * It features 16-bit colors, 240x240 resolution.
 *
 * LVGL is used as graphics library. LVGL is NOT thread safe, therefore the user must take LVGL mutex
 * by calling bsp_display_lock() before calling and LVGL API (lv_...) and then give the mutex with
 * bsp_display_unlock().
 *
 * Display's backlight must be enabled explicitly by calling bsp_display_backlight_on()
 **************************************************************************************************/
#define BSP_LCD_PIXEL_CLOCK_HZ (80 * 1000 * 1000)
#define BSP_LCD_SPI_NUM (SPI2_HOST)

/**
 * @brief BSP display configuration structure
 *
 */
typedef struct {
    lvgl_port_cfg_t lvgl_port_cfg;  /*!< LVGL port configuration */
    uint32_t        buffer_size;    /*!< Size of the buffer for the screen in pixels */
    bool            double_buffer;  /*!< True, if should be allocated two buffers */
    struct {
        unsigned int buff_dma: 1;    /*!< Allocated LVGL buffer will be DMA capable */
        unsigned int buff_spiram: 1; /*!< Allocated LVGL buffer will be in PSRAM */
    } flags;
} bsp_display_cfg_t;

/**
 * @brief Initialize display
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_display_t *bsp_display_start(void);

/**
 * @brief Initialize display
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
 *
 * @param cfg display configuration
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_disp_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg);


/**
 * @brief Get pointer to input device (touch, buttons, ...)
 *
 * @note The LVGL input device is initialized in bsp_display_start() function.
 *
 * @return Pointer to LVGL input device or NULL when not initialized
 */
lv_indev_t *bsp_display_get_input_dev(void);

/**
 * @brief Take LVGL mutex
 *
 * @param timeout_ms Timeout in [ms]. 0 will block indefinitely.
 * @return true  Mutex was taken
 * @return false Mutex was NOT taken
 */
bool bsp_display_lock(uint32_t timeout_ms);

/**
 * @brief Give LVGL mutex
 *
 */
void bsp_display_unlock(void);

/**
 * @brief Rotate screen
 *
 * Display must be already initialized by calling bsp_display_start()
 *
 * @param[in] disp Pointer to LVGL display
 * @param[in] rotation Angle of the display rotation
 */
void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation);

/** @} */ // end of display
#endif

/** \addtogroup g06_led
 *  @{
 */

/**************************************************************************************************
 *
 * WS2812
 *
 * There's one RGB light on ESP32-C6-LED-1.47
 **************************************************************************************************/

/**
 * @brief Initialize WS2812
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t bsp_led_init(void);

/**
 * @brief Set RGB for a specific pixel
 *
 * @param r: red part of color
 * @param g: green part of color
 * @param b: blue part of color
 *
 * @return
 *      - ESP_OK: Set RGB for a specific pixel successfully
 *      - ESP_ERR_INVALID_ARG: Set RGB for a specific pixel failed because of invalid parameters
 *      - ESP_FAIL: Set RGB for a specific pixel failed because other error occurred
 */
esp_err_t bsp_led_rgb_set(uint8_t r, uint8_t g, uint8_t b);

/** @} */ // end of leds

/** @defgroup g02_storage SD Card and SPIFFS
 *  @brief SPIFFS and SD card BSP API
 *  @{
 */

/**************************************************************************************************
 *
 * SPIFFS
 *
 * After mounting the SPIFFS, it can be accessed with stdio functions ie.:
 * \code{.c}
 * FILE* f = fopen(BSP_SPIFFS_MOUNT_POINT"/hello.txt", "w");
 * fprintf(f, "Hello World!\n");
 * fclose(f);
 * \endcode
 **************************************************************************************************/
#define BSP_SPIFFS_MOUNT_POINT CONFIG_BSP_SPIFFS_MOUNT_POINT

/**
 * @brief Mount SPIFFS to virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_spiffs_register was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes
 */
esp_err_t bsp_spiffs_mount(void);

/**
 * @brief Unmount SPIFFS from virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if the partition table does not contain SPIFFS partition with given label
 *      - ESP_ERR_INVALID_STATE if esp_vfs_spiffs_unregister was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes
 */
esp_err_t bsp_spiffs_unmount(void);

/** @} */ // end of storage

/** \addtogroup g05_buttons
 *  @{
 */

/**************************************************************************************************
 *
 * Button
 *
 **************************************************************************************************/

/**
 * @brief Initialize all buttons
 *
 * Returned button handlers must be used with espressif/button component API
 *
 * @note For LCD panel button which is defined as BSP_BUTTON_BOOT, bsp_display_start should
 *       be called before call this function.
 *
 * @param[out] btn_array      Output button array
 * @param[out] btn_cnt        Number of button handlers saved to btn_array, can be NULL
 * @param[in]  btn_array_size Size of output button array. Must be at least BSP_BUTTON_NUM
 * @return
 *     - ESP_OK               All buttons initialized
 *     - ESP_ERR_INVALID_ARG  btn_array is too small or NULL
 *     - ESP_FAIL             Underlaying iot_button_create failed
 */
esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size);

/** @} */ // end of buttons
#ifdef __cplusplus
}
#endif
