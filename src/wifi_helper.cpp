#include "wifi_helper.h"
#include "secrets.h"

#include <Arduino.h>
#include <WiFi.h>

// WiFi connection
void connectToWifi() {
  // Set WiFi mode to Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(Secrets::SSID_NAME, Secrets::WIFI_PASSWORD);

  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(Secrets::SSID_NAME);

  while (WiFi.status() != WL_CONNECT_FAILED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());     // Local IP Address
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());        // Signal Strength in dBm
}