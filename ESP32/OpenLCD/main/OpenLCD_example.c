/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "i2c-simple-example";

#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MPU9250_SENSOR_ADDR                 0x72       /*!< Slave address of the MPU9250 sensor */
#define MPU9250_WHO_AM_I_REG_ADDR           0x75        /*!< Register addresses of the "who am I" register */

#define MPU9250_PWR_MGMT_1_REG_ADDR         0x6B        /*!< Register addresses of the power managment register */
#define MPU9250_RESET_BIT                   7

/**
 * @brief Read a sequence of bytes from a MPU9250 sensor registers
 */
static esp_err_t mpu9250_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU9250_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief Write a byte to a MPU9250 sensor register
 */
static esp_err_t mpu9250_register_write_byte(uint8_t* data, size_t size)
{
    int ret;
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_SENSOR_ADDR, data, size, 100/portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "i2c_master_write complete");
    vTaskDelay(10);
    return ret;
}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


void app_main(void)
{
    uint8_t data[2];
    ESP_ERROR_CHECK(i2c_master_init());
    i2c_set_timeout(0, 5000);
    ESP_LOGI(TAG, "I2C initialized successfully");
    /* Read the MPU9250 WHO_AM_I register, on power up the register should have the value 0x71 */
    while(1){
        /* Demonstrate writing by reseting the MPU9250 */
        data[0] = 124;
        data[1] = '-';
        vTaskDelay(100/portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(mpu9250_register_write_byte(data, 2));
        //vTaskDelay(100/portTICK_PERIOD_MS);
        //ESP_ERROR_CHECK(mpu9250_register_write_byte("oogaboogab", 'H'));
        //vTaskDelay(1000/portTICK_PERIOD_MS);
        uint8_t data2[11] = {'h', 'e', 'l', 'l', 'o',' ', 'w', 'o', 'r', 'l', 'd'};
        ESP_LOGI(TAG, "sizeof data2: %d", sizeof(data2));
        ESP_LOGI(TAG, "sizeof data2 total: %d", sizeof(data2)* 11);
        ESP_LOGI(TAG, "sizeof uint8_t: %d", sizeof(uint8_t));
        ESP_ERROR_CHECK(mpu9250_register_write_byte(data2, 11));
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(mpu9250_register_write_byte(data2, 11));

    }
       ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
}
