#pragma once

#include "orb_geometry.h"

#include <FastLED.h>

#include <cstdint>

namespace orb {

/**
 * Tuning for the sunny-day scene. All fields are exposed so a future
 * configuration path (web app, NVS) can override them without touching code.
 */
struct SunnyParams {
  // --- Sky gradient ---------------------------------------------------------
  CRGB  zenithColor  { 20, 60, 150 };    // deep blue overhead
  CRGB  horizonColor { 150, 200, 230 };  // pale haze at the horizon

  // --- Sun ------------------------------------------------------------------
  CRGB  sunColor          { 255, 214, 140 };
  float sunElevationDeg   { 52.0f };      // height of the sun above the horizon
  float sunAzimuthDeg     { 40.0f };      // direction of the sun around the orb
  float sunCoreDeg        { 12.0f };      // angular radius of the bright disk
  float sunGlowDeg        { 65.0f };      // angular reach of the soft glow
  float sunIntensity      { 1.0f };       // overall sun brightness

  // --- Clouds ---------------------------------------------------------------
  CRGB  cloudColor        { 205, 215, 225 };
  float cloudScale        { 60.0f };      // noise feature size (bigger = larger)
  float cloudDrift        { 4.0f };       // noise units per second (drift speed)
  float cloudThreshold    { 0.62f };      // noise level where a cloud begins
  float cloudCoverage     { 0.55f };      // how opaque clouds are
  float cloudSunTint      { 0.35f };      // warm tint where clouds meet the sun
  float cloudSunOcclusion { 0.70f };      // how much clouds hide the sun

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
 *   3. seamless drifting cloud cover sampled from 3D noise on the direction,
 *      which then shades the sun and lightens the sky.
 *
 * @param pixels   Pixel buffer to fill (count entries)
 * @param points   Precomputed LED geometry, parallel to `pixels`
 * @param count    Number of LEDs
 * @param params   Scene tuning
 * @param timeMs   Milliseconds since boot; drives the cloud drift
 */
void renderSunny(CRGB* pixels,
                 const LedPoint* points,
                 std::uint16_t count,
                 const SunnyParams& params,
                 std::uint32_t timeMs) noexcept;

} // namespace orb
