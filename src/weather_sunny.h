#pragma once

#include "orb_color.h"
#include "orb_geometry.h"

#include <cstdint>

namespace orb {

/**
 * Tuning for the sunny-day scene. All fields are exposed so a future
 * configuration path (web app, NVS) can override them without touching code.
 */
struct SunnyParams {
  // --- Sky gradient ---------------------------------------------------------
  Rgb   zenithColor  { 20, 75, 155 };    // blue overhead
  Rgb   horizonColor { 110, 150, 195 };  // softer blue at the horizon

  // --- Sun ------------------------------------------------------------------
  Rgb   sunColor          { 255, 165, 30 };   // orange-amber body of the sun
  Rgb   sunHotColor       { 255, 235, 150 };  // hot yellow core
  float sunElevationDeg   { 30.0f };      // height of the sun above the horizon
  float sunAzimuthDeg     { 40.0f };      // direction of the sun around the orb
  float sunCoreDeg        { 65.0f };      // angular radius of the bright disk
  float sunGlowDeg        { 170.0f };     // angular reach of the soft glow
  float sunIntensity      { 1.60f };      // overall sun strength
  float sunAmbient        { 0.60f };      // whole-orb warm lean (beats diffusion)

  // --- Clouds ---------------------------------------------------------------
  Rgb   cloudColor        { 235, 215, 180 };  // warm haze, not white
  float cloudScale        { 60.0f };      // noise feature size (bigger = larger)
  float cloudDrift        { 40.0f };       // noise units per second (drift speed)
  float cloudThreshold    { 0.65f };      // noise level where a cloud begins
  float cloudCoverage     { 0.35f };      // how opaque clouds are
  float cloudSunTint      { 0.45f };      // warm tint where clouds meet the sun
  float cloudSunOcclusion { 0.25f };      // how much clouds hide the sun

  // --- Global ---------------------------------------------------------------
  float exposure { 1.0f };
};

/**
 * Render one frame of the sunny-day simulation into `pixels`.
 *
 * The scene is built purely from each LED's direction on the orb:
 *   1. a vertical sky gradient (pale near the base, deep blue overhead),
 *   2. a warm sun disk with a wide glow falloff around `sunAzimuthDeg` /
 *      `sunElevationDeg`,
 *   3. seamless drifting cloud cover sampled from 3D value noise on the
 *      direction, which then shades the sun and lightens the sky.
 *
 * @param pixels   Framebuffer to fill (count entries)
 * @param points   Precomputed LED geometry, parallel to `pixels`
 * @param count    Number of LEDs
 * @param params   Scene tuning
 * @param timeMs   Milliseconds since boot; drives the cloud drift
 */
void renderSunny(Rgb* pixels,
                 const LedPoint* points,
                 std::uint16_t count,
                 const SunnyParams& params,
                 std::uint32_t timeMs) noexcept;

} // namespace orb
