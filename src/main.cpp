#include "wifi_helper.h"
#include "lcd_helper.h"

#include <Arduino.h>
#include <ST77916_QSPI.h>
#include <P5.h>

#include <memory>
#include <cstdint>

namespace {
  constexpr uint8_t SCL_PIN = 12;
  constexpr uint8_t SDA_PIN = 11;
  constexpr uint8_t IO1_PIN = 13;
  constexpr uint8_t IO2_PIN = 5;
  constexpr uint8_t IO3_PIN = 9;
  constexpr uint8_t RST_PIN = 8;
  constexpr uint8_t CS_PIN  = 10;
  constexpr uint8_t BL_PIN  = -1;

  // Like std::make_unique, but returns nullptr on allocation failure instead of
  // throwing/aborting, so a low-heap condition can be reported over Serial.
  template <typename T, typename... Args>
  std::unique_ptr<T> makeUniqueNoThrow(Args&&... args) {
    return std::unique_ptr<T>(new (std::nothrow) T(std::forward<Args>(args)...));
  }

  // Single owners of LCD Display objects
  std::unique_ptr<ST77916_QSPI_GFX> tft;
  std::unique_ptr<P5> p5;
  std::unique_ptr<LcdDisplay> display;

  [[noreturn]] void halt(const char* why) {
    Serial.printf("fatal: %s\n", why);
    while (true) {
      delay(1000);
    }
  }
} // namespace

void setup() {
  Serial.begin(115200);
  delay(1000);

  // connectToWifi();
  Serial.printf("PSRAM: %u bytes\n", ESP.getPsramSize());
  Serial.printf("Largest free block: %u bytes\n",
              heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

  // Initialize display vars
  tft = makeUniqueNoThrow<ST77916_QSPI_GFX>(
    SCL_PIN,
    SDA_PIN,
    IO1_PIN,
    IO2_PIN,
    IO3_PIN,
    RST_PIN,
    CS_PIN,
    BL_PIN
  );

  if (!tft) {
    halt("out of memory (tft)");
  }

  ST77916_QSPI_GFX::Config cfg;
  cfg.clockHz = 70000000;   // 70 MHz
  cfg.doubleBuffer = true;
  cfg.pushTask = true;
  if (!tft->begin(cfg)) {
    Serial.printf("begin failed: %s\n", tft->lastError());
    halt("display init");
  }

  Serial.println("Set up TFT display");

  p5 = makeUniqueNoThrow<P5>(*tft);
  if (!p5) {
    halt("out of memory (p5)");
  }

  Serial.println("Set up P5");

  display = makeUniqueNoThrow<LcdDisplay>(*tft, *p5);
  if (!display) {
    halt("out of memory (display)");
  }

  Serial.println("Beginning display");

  display->begin();
}

void loop() {
  display->update();
}
