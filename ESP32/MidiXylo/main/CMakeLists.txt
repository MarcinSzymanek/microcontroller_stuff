idf_component_register(SRCS "test_mux_adc.cpp" "adc_ctrl.cpp" "mux.hpp"
                        INCLUDE_DIRS "." "../components"
                        PRIV_REQUIRES esp_timer esp_adc driver usb_midi)