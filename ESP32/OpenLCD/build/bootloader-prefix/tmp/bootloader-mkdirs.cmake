# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/nastr/esp/v5.0.6/esp-idf/components/bootloader/subproject"
  "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader"
  "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader-prefix"
  "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader-prefix/tmp"
  "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader-prefix/src"
  "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/nastr/Documents/Microcontroller/microcontroller_stuff/ESP32/OpenLCD/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
