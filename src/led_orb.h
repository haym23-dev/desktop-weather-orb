#pragma once

#include "orb_color.h"
#include "orb_geometry.h"

#include <Adafruit_NeoPixel.h>

#include <cstdint>

namespace orb {

//Hard cap on strand length
constexpr std::uint16_t MAX_LEDS = 256;

/**
 * Data pin for the WS2812B strand (through the level shifter).
 *
 * Must NOT be one of the QSPI display pins (5, 8, 9, 10, 11, 12, 13)
 */
constexpr std::uint8_t DATA_PIN = 4;

/**
 * Owns the NeoPixel strand and the precomputed geometry of every LED.
 *
 * This class knows nothing about weather; it just hands out an RGB
 * framebuffer and the LED positions for a renderer (see weather_sunny.h) to
 * fill in, then pushes that buffer to the strip on show().
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
   * Initialise the strip and precompute LED geometry. Call once, after Serial.
   */
  void begin();

  /**
   * Scale all output, 0..255. Cheap, and does not require a redraw.
   */
  void setBrightness(std::uint8_t brightness) noexcept;

  /**
   * Zero the framebuffer and the strip.
   */
  void clear() noexcept;

  /**
   * Push the current framebuffer to the strand.
   */
  void show() noexcept;

  std::uint16_t count() const noexcept { return count_; }
  Rgb* pixels() noexcept { return pixels_; }
  const LedPoint* points() const noexcept { return points_; }

private:
  std::uint16_t count_;
  float turns_;
  float startAngle_;

  Adafruit_NeoPixel strip_;

  Rgb pixels_[MAX_LEDS];
  LedPoint points_[MAX_LEDS];
};

} // namespace orb
