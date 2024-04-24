idf_component_register(SRCS "test_all.cpp" "adc_ctrl.cpp" "mux.cpp"
                        INCLUDE_DIRS "." "../components"
                        PRIV_REQUIRES esp_timer esp_adc driver usb_midi)