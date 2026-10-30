#include "wifi_helper.h"
#include "lcd_helper.h"

#include <Arduino.h>
#include <ST77916_QSPI.h>
#include <P5.h>

#include <cstdint>

namespace {
  constexpr uint8_t SCL_PIN = 12;
  constexpr uint8_t SDA_PIN = 11;
  constexpr uint8_t IO1_PIN = 13;
  constexpr uint8_t IO2_PIN = 5;
  constexpr uint8_t IO3_PIN = 9;
  constexpr uint8_t RST_PIN = 8;
  constexpr uint8_t CS_PIN  = 10;
  constexpr uint8_t BL_PIN  = -1;   // Set to HIGH

  ST77916_QSPI_GFX tft(
    SCL_PIN,
    SDA_PIN,
    IO1_PIN,
    IO2_PIN,
    IO3_PIN,
    RST_PIN,
    CS_PIN,
    BL_PIN
  );
  P5 p5(tft);
  LcdDisplay display(tft, p5);

  [[noreturn]] void halt(const char* why) {
    Serial.printf("fatal: %s\n", why);
    while (true) {
      delay(1000);
    }
  }
} // namespace

static void logPSRAM() {
  Serial.println("--- PSRAM Diagnostics ---");
  
  // Check if the hardware detects PSRAM at all
  if (psramInit()) {
    Serial.println("PSRAM Hardware Initialized Successfully!");
    
    // Print total and free memory space
    Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM:  %d bytes\n", ESP.getFreePsram());
  } else {
    Serial.println("PSRAM Initialization FAILED! Check your IDE/PlatformIO settings.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  logPSRAM();

  // connectToWifi();

  ST77916_QSPI_GFX::Config cfg;
  cfg.clockHz = 70000000;   // 70 MHz
  cfg.doubleBuffer = true;
  cfg.pushTask = true;
  if (!tft.begin(cfg)) {
    Serial.printf("begin failed: %s\n", tft.lastError());
    halt("display init");
  }

  Serial.println("Beginning display");

  display.begin();
}

void loop() {
  display.update();
}
