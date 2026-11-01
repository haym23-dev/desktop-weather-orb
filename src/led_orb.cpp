#include "led_orb.h"

namespace orb {

LedOrb::LedOrb(std::uint16_t count, float turns, float startAngleRad) noexcept
    : count_(count > MAX_LEDS ? MAX_LEDS : count),
      turns_(turns),
      startAngle_(startAngleRad),
      strip_(count_, DATA_PIN, NEO_GRB + NEO_KHZ800) {}

void LedOrb::begin() {
  strip_.begin();
  strip_.setBrightness(255);
  strip_.clear();
  strip_.show();

  for (std::uint16_t i = 0; i < count_; ++i) {
    points_[i] = helixPoint(i, count_, turns_, startAngle_);
  }
}

void LedOrb::setBrightness(std::uint8_t brightness) noexcept {
  strip_.setBrightness(brightness);
}

void LedOrb::clear() noexcept {
  for (std::uint16_t i = 0; i < count_; ++i) {
    pixels_[i] = Rgb{0, 0, 0};
  }
  strip_.clear();
  strip_.show();
}

void LedOrb::show() noexcept {
  for (std::uint16_t i = 0; i < count_; ++i) {
    const Rgb& c = pixels_[i];
    // Gamma-correct like the reference sketch: richer colors and smoother
    // low-end transitions than raw linear values.
    strip_.setPixelColor(i, strip_.gamma32(strip_.Color(c.r, c.g, c.b)));
  }
  strip_.show();
}

} // namespace orb
