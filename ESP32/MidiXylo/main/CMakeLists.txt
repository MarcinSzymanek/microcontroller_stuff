idf_component_register(SRCS "main.cpp"
                        INCLUDE_DIRS "." "../components"
                        PRIV_REQUIRES esp_timer esp_adc usb_midi)