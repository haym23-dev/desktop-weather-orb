#include <Arduino.h>
#include <ST77916_QSPI.h>
#include <P5.h>
#include <memory>
#include <cstdint>

#include "wifi_helper.h"
#include "lcd_helper.h"

namespace {
  constexpr uint8_t SCL_PIN = 18;
  constexpr uint8_t SDA_PIN = 23;
  constexpr uint8_t IO1_PIN = 19;
  constexpr uint8_t IO2_PIN = 22;
  constexpr uint8_t IO3_PIN = 21;
  constexpr uint8_t CS_PIN  = 5;
  constexpr uint8_t RST_PIN = 4;

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

  connectToWifi();

  // Initialize display vars
  tft = makeUniqueNoThrow<ST77916_QSPI_GFX>(
    SCL_PIN,
    SDA_PIN,
    IO1_PIN,
    IO2_PIN,
    IO3_PIN,
    RST_PIN,
    CS_PIN,
    5           // TODO backlight pin
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

  p5 = makeUniqueNoThrow<P5>(*tft);
  if (!p5) {
    halt("out of memory (p5)");
  }

  display = makeUniqueNoThrow<LcdDisplay>(*tft, *p5);
  if (!display) {
    halt("out of memory (display)");
  }
  display->begin();
}

void loop() {
  display->update();
}
