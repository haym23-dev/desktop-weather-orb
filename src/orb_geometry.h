#pragma once

#include <cmath>
#include <cstdint>

namespace orb {

/**
 * A single addressable LED's position on the orb.
 *
 * Directions are unit vectors in a right-handed frame with +Z pointing up
 * (from the base of the orb toward its top):
 *
 *   +Z = up, the "zenith" of the sky
 *   -Z = down, the base
 *   +X = the 0 rad azimuth reference
 *
 * `azimuth` is how far the strand has wrapped around the vertical axis (it
 * grows past 2*pi as the helix climbs), and `altitude` is a cheap 0..1 scalar
 * for the vertical position (0 = bottom pole, 1 = top pole) used by the sky
 * gradients.
 */
struct LedPoint {
  float x { 0.0f };
  float y { 0.0f };
  float z { 0.0f };
  float azimuth  { 0.0f };  // radians, 0 .. turns * 2*pi
  float altitude { 0.0f };  // 0.0 bottom pole .. 1.0 top pole
};

/**
 * Map an LED index to its position on a bottom-to-top helical strand.
 *
 * The strip is wound once around the vertical axis: index 0 sits at the bottom
 * pole and index `count - 1` at the top pole. `turns` is how many full
 * revolutions the strand makes over that span. All the sky effects are derived
 * from the returned direction, so the winding handedness only changes which
 * side of the orb a feature lands on, not whether it works.
 *
 * @param index       LED index, 0 .. count - 1 (0 = bottom)
 * @param count       Total LEDs on the strand
 * @param turns       Full revolutions from bottom to top
 * @param startAngle  Azimuth (radians) of index 0 around the axis
 */
inline LedPoint helixPoint(std::uint16_t index,
                           std::uint16_t count,
                           float turns,
                           float startAngle = 0.0f) noexcept {
  const float t = (count > 1u)
    ? static_cast<float>(index) / static_cast<float>(count - 1u)
    : 0.0f;

  // Polar angle measured from the bottom pole (-pi/2) to the top (+pi/2).
  const float polar   = -0.5f * static_cast<float>(PI) + t * static_cast<float>(PI);
  const float azimuth = startAngle + t * turns * 2.0f * static_cast<float>(PI);
  const float ring    = std::cos(polar);

  LedPoint p;
  p.x = ring * std::cos(azimuth);
  p.y = ring * std::sin(azimuth);
  p.z = std::sin(polar);
  p.azimuth  = azimuth;
  p.altitude = t;
  return p;
}

} // namespace orb
