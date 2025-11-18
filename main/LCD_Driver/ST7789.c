#include "ST7789.h"

static const char *TAG_LCD = "WS_LCD";

esp_lcd_panel_handle_t panel_handle = NULL;

void LCD_Init(void)
{
    // ESP_LOGI(TAG_LCD, "Initialize SPI bus");                                            
    // spi_bus_config_t buscfg = {                                                         
    //     .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,                                            
    //     .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,                                            
    //     .miso_io_num = EXAMPLE_PIN_NUM_MISO,                                            
    //     .quadwp_io_num = -1,                                                            
    //     .quadhd_io_num = -1,                                                            
    //     .max_transfer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(uint16_t),    
    // };
    // ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));            

    ESP_LOGI(TAG_LCD, "Install panel IO");                                              
    esp_lcd_panel_io_handle_t io_handle = NULL;                                         
    esp_lcd_panel_io_spi_config_t io_config = {                                             
        .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = example_notify_lvgl_flush_ready,
        .user_ctx = &disp_drv,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    ESP_LOGI(TAG_LCD, "Install ST7789T panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));


    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    /* RAM Control */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xB0, (uint8_t []){0x00, 0xE8}, 2));
    /* Porch Setting */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xB2, (uint8_t []){0x0c, 0x0c, 0x00, 0x33, 0x33}, 5));
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xB7, (uint8_t []){0x75}, 1));
    /* VCOM Setting, VCOM=1.175V */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xBB, (uint8_t []){0x1A}, 1));
    /* LCM Control, XOR: BGR, MX, MH */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC0, (uint8_t []){0x80}, 1));
    /* VDV and VRH Command Enable, enable=1 */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC2, (uint8_t []){0x01, 0xff}, 2));
    /* VRH Set, Vap=4.4+... */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC3, (uint8_t []){0x13}, 1));
    /* VDV Set, VDV=0 */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC4, (uint8_t []){0x20}, 1));
    /* Frame Rate Control, 60Hz, inversion=0 */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC6, (uint8_t []){0x0F}, 1));
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xD0, (uint8_t []){0xA4, 0xA1}, 1));
    /* Positive Voltage Gamma Control */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xE0, (uint8_t []){0xD0, 0x0D, 0x14, 0x0D, 0x0D, 0x09, 0x38, 0x44, 0x4E, 0x3A, 0x17, 0x18, 0x2F, 0x30}, 14));
    /* Negative Voltage Gamma Control */
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xE1, (uint8_t []){0xD0, 0x09, 0x0F, 0x08, 0x07, 0x14, 0x37, 0x44, 0x4D, 0x38, 0x15, 0x16, 0x2C, 0x2E}, 14));

    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));

    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG_LCD, "Turn on LCD backlight");
    // gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
    
    BK_Init();                                                                                          // Initialize the backlight
    BK_Light(75);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Backlight program
static ledc_channel_config_t ledc_channel;
void BK_Init(void)
{
    ESP_LOGI(TAG_LCD, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    
    // 配置LEDC
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .speed_mode = LEDC_LS_MODE,
        .timer_num = LEDC_HS_TIMER,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel.channel    = LEDC_HS_CH0_CHANNEL;
    ledc_channel.duty       = 0;
    ledc_channel.gpio_num   = EXAMPLE_PIN_NUM_BK_LIGHT;
    ledc_channel.speed_mode = LEDC_LS_MODE;
    ledc_channel.timer_sel  = LEDC_HS_TIMER;
    ledc_channel_config(&ledc_channel);
    ledc_fade_func_install(0);
}
void BK_Light(uint8_t Light)
{   
    if(Light > 100) Light = 100;
    uint16_t Duty = LEDC_MAX_Duty-(81*(100-Light));
    if(Light == 0) Duty = 0;
    // 设置PWM占空比
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, Duty);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
}
// end Backlight program
