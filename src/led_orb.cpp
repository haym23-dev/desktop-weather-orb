#include "led_orb.h"

#include <Arduino.h>

namespace orb {

namespace {
  // Peak current budget for the strand. 30 LEDs at ~60 mA each is ~1.8 A;
  // FastLED dims to stay under this rather than letting the 5 V rail sag.
  constexpr std::uint32_t PEAK_MILLIAMPS = 1800;
} // namespace

LedOrb::LedOrb(std::uint16_t count, float turns, float startAngleRad) noexcept
    : count_(count > MAX_LEDS ? MAX_LEDS : count),
      turns_(turns),
      startAngle_(startAngleRad) {}

void LedOrb::begin() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(pixels_, count_);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, PEAK_MILLIAMPS);
  FastLED.setBrightness(255);
  FastLED.clear(true);

  for (std::uint16_t i = 0; i < count_; ++i) {
    points_[i] = helixPoint(i, count_, turns_, startAngle_);
  }
}

void LedOrb::setBrightness(std::uint8_t brightness) noexcept {
  FastLED.setBrightness(brightness);
}

void LedOrb::show() noexcept {
  FastLED.show();
}

} // namespace orb
