idf_component_register(
                         SRCS "main.c"
                              "LCD_Driver/ST7789.c"
                              "LVGL_Driver/LVGL_Driver.c"
                              "LVGL_UI/LVGL_Example.c"
                              "SD_Card/SD_SPI.c"
                              "RGB/RGB.c"
                              "Wireless/Wireless.c"

                         INCLUDE_DIRS
                              "./LCD_Driver"
                              "./LVGL_Driver"
                              "./LVGL_UI"
                              "./SD_Card"
                              "./RGB"
                              "./Wireless"
                              "."

			 PRIV_REQUIRES
			      "bt"
			      "esp_driver_ledc"
			      "esp_lcd"
			      "fatfs"
			      "nvs_flash"
			      "spi_flash"
                       )
