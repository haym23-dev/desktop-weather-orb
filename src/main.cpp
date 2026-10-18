#include <Arduino.h>
#include "wifi.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectToWifi();
}

void loop() {
  // put your main code here, to run repeatedly:
}
