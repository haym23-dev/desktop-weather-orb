#pragma once

#include <cstdint>

namespace orb {

/**
 * Minimal 8-bit RGB color. Kept independent of any LED library so the scene
 * code only depends on the orb driver's interface.
 */
struct Rgb {
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;

  constexpr Rgb() noexcept : r(0), g(0), b(0) {}
  constexpr Rgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept
      : r(red), g(green), b(blue) {}
};

} // namespace orb
