#include <WiFi.h>

namespace {
  const char *SSID = "Smartaira360";
  const char *PASSWORD = "postagebroiler";
}

// WiFi connection
void connectToWifi() {
  // Set WiFi mode to Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(SSID);

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