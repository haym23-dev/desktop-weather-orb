#pragma once

#include <FastLED.h>

#include <cstdint>

#include "orb_geometry.h"

namespace orb {

/**
 * Hard cap on strand length. Buffers below are sized for this so the orb owns
 * no heap, matching the rest of the firmware.
 */
constexpr std::uint16_t MAX_LEDS = 256;

/**
 * Data pin for the WS2812B strand (through the 74HCT245 level shifter).
 */
constexpr std::uint8_t DATA_PIN = 4;

/**
 * Owns the FastLED strand and the precomputed geometry of every LED.
 *
 * This class knows nothing about weather; it just hands out the pixel buffer
 * and the LED positions for a renderer (see weather_sunny.h) to fill in.
 */
class LedOrb {
public:
  /**
   * @param count        Number of LEDs on the strand (clamped to MAX_LEDS)
   * @param turns        Helix revolutions from bottom to top
   * @param startAngleRad Azimuth of index 0 around the vertical axis
   */
  LedOrb(std::uint16_t count, float turns, float startAngleRad = 0.0f) noexcept;

  /**
   * Initialise FastLED, the power limiter, and precompute LED geometry.
   * Call once, after Serial is up.
   */
  void begin();

  /**
   * Scale all output, 0..255. Cheap, and does not require a redraw.
   */
  void setBrightness(std::uint8_t brightness) noexcept;

  /**
   * Push the current pixel buffer to the strand.
   */
  void show() noexcept;

  std::uint16_t count() const noexcept { return count_; }
  CRGB* pixels() noexcept { return pixels_; }
  const LedPoint* points() const noexcept { return points_; }

private:
  std::uint16_t count_;
  float turns_;
  float startAngle_;

  CRGB pixels_[MAX_LEDS];
  LedPoint points_[MAX_LEDS];
};

} // namespace orb
