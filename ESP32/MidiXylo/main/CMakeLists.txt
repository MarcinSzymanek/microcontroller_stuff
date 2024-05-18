idf_component_register(SRCS "note_controller.cpp" "peripheral_monitor.cpp" "display.cpp" "tests/test_all.cpp" "adc_ctrl.cpp" "mux.cpp" "m_xylo.cpp" "open_lcd.cpp"
                        INCLUDE_DIRS "." "../components" "include"
                        PRIV_REQUIRES esp_timer esp_adc pthread driver usb_midi)