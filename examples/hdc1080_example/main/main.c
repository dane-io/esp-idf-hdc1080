#include <stdio.h>

#include "esp_check.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hdc1080.h"

#define I2C_SCL GPIO_NUM_5
#define I2C_SDA GPIO_NUM_4
#define I2C_PORT I2C_NUM_0
#define HDC1080_ADDR 0x40

static const char TAG[] = "HDC1080_sample";

i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = I2C_SCL,
        .sda_io_num = I2C_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };
i2c_master_bus_handle_t i2c_bus_handle;

i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = HDC1080_ADDR,
    .scl_speed_hz = 100000,
    .flags.disable_ack_check = true,
};
i2c_master_dev_handle_t hdc_1080_dev_handle;

void app_main(void)
{
    while(1) {
        // Initialize bus and add device
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle));
        ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &hdc_1080_dev_handle));

        // Configure HDC1080 settings
        hdc1080_settings_t hdc1080_settings;
        hdc1080_settings.device_handle = hdc_1080_dev_handle;
        hdc1080_settings.configuration = HDC1080_CONFIG_TEMP_14BIT_RES_MASK | HDC1080_CONFIG_HUMIDITY_14BIT_RES_MASK | 
                                        HDC1080_CONFIG_HEATER_ENABLE_MASK | HDC1080_CONFIG_TEMP_AND_HUMIDITY_MODE_MASK;
        ESP_ERROR_CHECK(HDC1080_Write_Configuration(&hdc1080_settings));
        
        // Get temperature (degrees C) and humidity (in %) from HDC1080
        float temp, humidity;
        if (HDC1080_Read_Vals(&hdc1080_settings, &temp, &humidity) == ESP_OK) {
            ESP_LOGI(TAG, "Temp: %.3f, Humidity: %.3f", temp, humidity);
        }

        // Deinitialize I2C bus (must be done for software resets)
        ESP_ERROR_CHECK(i2c_master_bus_rm_device(hdc_1080_dev_handle));  // Should be redundant if deleting the bus
        ESP_ERROR_CHECK(i2c_del_master_bus(i2c_bus_handle));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}