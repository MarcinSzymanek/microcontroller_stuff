idf_component_register(SRCS "main.cpp"
                        INCLUDE_DIRS "." "../components/usb_midi"
                        PRIV_REQUIRES esp_adc usb_midi)