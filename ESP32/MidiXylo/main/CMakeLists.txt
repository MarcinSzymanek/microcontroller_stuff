idf_component_register(SRCS "src/note_controller.cpp" "src/peripheral_monitor.cpp" "src/display.cpp" "tests/test_all.cpp" "src/adc_ctrl.cpp" "src/mux.cpp" "src/m_xylo.cpp" "src/open_lcd.cpp"
                        INCLUDE_DIRS "." "../components" "include"
                        PRIV_REQUIRES esp_timer esp_adc pthread driver usb_midi)