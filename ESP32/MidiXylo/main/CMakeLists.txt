idf_component_register(SRCS "tests/test_all.cpp" "adc_ctrl.cpp" "mux.cpp" "m_xylo.cpp"
                        INCLUDE_DIRS "." "../components" "include"
                        PRIV_REQUIRES esp_timer esp_adc driver usb_midi)