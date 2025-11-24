if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.3")
    set(REQ esp_driver_gpio)
    set(PRIV_REQ "")
else()
    set(REQ driver)
    set(PRIV_REQ "")
endif()

idf_component_register(
    SRCS "esp32_c6_lcd_1.47.c"
    INCLUDE_DIRS "include"
    REQUIRES ${REQ}
    PRIV_REQUIRES ${PRIV_REQ}
)
