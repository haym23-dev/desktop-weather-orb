#include "wifi_helper.h"
#include "lcd_helper.h"
#include "led_orb.h"
#include "weather_sunny.h"

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

  // --- LED orb -------------------------------------------------------------
  // Enter two physical measurements; turns are derived so the firmware's
  // helix matches the winding you actually built. See LED_ORB.md.
  constexpr float    CYLINDER_DIAMETER_MM = 25.4f;    // 1 in
  constexpr float    STRIP_LENGTH_MM      = 1000.0f;  // 30 LEDs at 30/m
  constexpr uint16_t LED_COUNT            = 30;

  // One full turn wraps one circumference, so turns = strip length / circumference.
  constexpr float HELIX_TURNS =
      STRIP_LENGTH_MM / (static_cast<float>(PI) * CYLINDER_DIAMETER_MM);

  constexpr uint32_t FRAME_MS = 33;   // ~30 fps

  // Global output cap. Lower this first if the strip browns out.
  constexpr uint8_t ORB_BRIGHTNESS = 80;

  orb::LedOrb ledOrb(LED_COUNT, HELIX_TURNS);
  orb::SunnyParams sunny;
  uint32_t lastFrameMs = 0;

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

  Serial.println("Beginning LED orb");
  ledOrb.begin();
  ledOrb.setBrightness(ORB_BRIGHTNESS);
  Serial.printf("LED orb: %u LEDs, %.2f turns, %.1f mm former dia, %.1f mm/lap\n",
                static_cast<unsigned>(LED_COUNT),
                HELIX_TURNS,
                CYLINDER_DIAMETER_MM,
                3.14159265f * CYLINDER_DIAMETER_MM);
}

void loop() {
  const uint32_t now = millis();
  if (now - lastFrameMs >= FRAME_MS) {
    lastFrameMs = now;
    orb::renderSunny(ledOrb.pixels(), ledOrb.points(), ledOrb.count(), sunny, now);
    ledOrb.show();

    display.update();
  }
}
