#include "Arduino.h"
#include "USB/src/USBMIDI.h"
#include "esp_adc/adc_oneshot.h"

void setup(){
  Serial.begin(115200);
}

void loop(){
    Serial.println("Hello world!");
    delay(1000);
}
