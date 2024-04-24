idf_component_register(SRCS "main.cpp" "adc_ctrl.cpp" "mux.hpp"
                        INCLUDE_DIRS "." "../components"
                        PRIV_REQUIRES esp_timer esp_adc driver usb_midi)