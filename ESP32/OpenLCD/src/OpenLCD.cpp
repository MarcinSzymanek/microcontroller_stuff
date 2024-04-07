#include "OpenLCD.hpp"
#include "Logger.hpp"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "string.h"


namespace interfaces{
    OpenLCD::OpenLCD(){
        ESP_LOGI(TAG, "Hello from OpenLCD!");
    }

    esp_err_t OpenLCD::init_openLCD(){
        // I2C vars
        // ESP32 Vroom i2c pins
        const int pin_scl = 22;
        const int pin_sda = 21;
        const char i2c_master_no = 0;
        const int i2c_master_freq_hz = 400000;
        const int master_port = 0;

        // Initialize i2c using esp32 sdk i2c methods
        i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = pin_sda;
        conf.scl_io_num = pin_scl;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = i2c_master_freq_hz;
        conf.clk_flags = 0;

        i2c_param_config(master_port, &conf);
        Logger::Log(TAG, "Initialized i2c");
        esp_err_t err = i2c_driver_install(i2c_master_no, conf.mode, 0, 0, 0);

        err = i2c_set_timeout(0, 20000);
        ESP_ERROR_CHECK(err);
        clear_buffer_();
        clear();
        write("XylofonMidi v0.10", 17);

        return err;
    }

    esp_err_t OpenLCD::write(const char* str, size_t size){
        uint8_t buffer[size];
        for(int i = 0; i < size; i++){
            buffer[i] = (uint8_t)*str;
            str++;
        }
        return i2c_master_write_to_device(0, i2c_addr, buffer, size, 500);
    }

    bool OpenLCD::update_display(){
        clear();
        write(lcd_buffer[0], MAX_LINE_WIDTH);
        switch_line_(1);
        write(lcd_buffer[1], MAX_LINE_WIDTH);

        return true;
    }

    bool OpenLCD::switch_line_(size_t line){
        if(line > 4) return false;
        char data[2] = {254, 0b10000000 | 20};
        write(data, 2);
        return true;
    }

    bool OpenLCD::update_line(const char* text, size_t line, size_t length){
        strcpy(lcd_buffer[line], text);

        if(length < 20){
            for(int i = length; i < MAX_LINE_WIDTH; i++){
                lcd_buffer[line][i] = ' ';
            }
        }
        return true;
    }

    void OpenLCD::clear_buffer_(){
        for(int i = 0; i < 2; i++){
            for(int j = 0; j < MAX_LINE_WIDTH; j++){
                lcd_buffer[i][j] = ' ';
            }
        }
    }

    esp_err_t OpenLCD::clear(){
        int ret;
        char data[2] = {'|', '-'};
        return write(data, 2);
    }
} // interfaces