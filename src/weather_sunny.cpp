#include "weather_sunny.h"

#include <cmath>
#include <cstdint>

namespace orb {

namespace {
  float clampf(float v, float lo, float hi) noexcept {
    return v < lo ? lo : (v > hi ? hi : v);
  }

  float smoothstepf(float edge0, float edge1, float x) noexcept {
    if (edge1 == edge0) return x < edge0 ? 0.0f : 1.0f;
    const float t = clampf((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
  }

  CRGB lerpColor(const CRGB& a, const CRGB& b, float t) noexcept {
    t = clampf(t, 0.0f, 1.0f);
    return CRGB(static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
                static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
                static_cast<std::uint8_t>(a.b + (b.b - a.b) * t));
  }

  CRGB scaleColor(const CRGB& c, float s) noexcept {
    return CRGB(static_cast<std::uint8_t>(clampf(c.r * s, 0.0f, 255.0f)),
                static_cast<std::uint8_t>(clampf(c.g * s, 0.0f, 255.0f)),
                static_cast<std::uint8_t>(clampf(c.b * s, 0.0f, 255.0f)));
  }

  CRGB addColor(const CRGB& a, const CRGB& b) noexcept {
    return CRGB(static_cast<std::uint8_t>(clampf(a.r + b.r, 0.0f, 255.0f)),
                static_cast<std::uint8_t>(clampf(a.g + b.g, 0.0f, 255.0f)),
                static_cast<std::uint8_t>(clampf(a.b + b.b, 0.0f, 255.0f)));
  }
} // namespace

void renderSunny(CRGB* pixels,
                 const LedPoint* points,
                 std::uint16_t count,
                 const SunnyParams& params,
                 std::uint32_t timeMs) noexcept {
  // Sun direction as a unit vector.
  const float sunEl   = params.sunElevationDeg * static_cast<float>(DEG_TO_RAD);
  const float sunAz   = params.sunAzimuthDeg * static_cast<float>(DEG_TO_RAD);
  const float sunZ    = std::sin(sunEl);
  const float sunRing = std::cos(sunEl);
  const float sunX    = sunRing * std::cos(sunAz);
  const float sunY    = sunRing * std::sin(sunAz);

  // Clouds translate slowly across the sphere.
  const float drift = static_cast<float>(timeMs) * 0.001f * params.cloudDrift;

  for (std::uint16_t i = 0; i < count; ++i) {
    const LedPoint& q = points[i];

    // 1. Sky gradient, pale at the base fading to deep blue overhead.
    const float up = smoothstepf(0.0f, 1.0f, q.altitude);
    CRGB color = lerpColor(params.horizonColor, params.zenithColor, up);

    // 2. Sun disk plus glow, keyed off the angle between LED and sun.
    const float dot = clampf(q.x * sunX + q.y * sunY + q.z * sunZ, -1.0f, 1.0f);
    const float angleDeg = std::acos(dot) * static_cast<float>(RAD_TO_DEG);
    const float glow = 1.0f - smoothstepf(params.sunCoreDeg, params.sunGlowDeg, angleDeg);
    const float core = 1.0f - smoothstepf(params.sunCoreDeg * 0.4f, params.sunCoreDeg, angleDeg);
    float sun = (glow * 0.6f + core) * params.sunIntensity;

    // 3. Seamless cloud cover from 3D noise on the LED direction.
    const auto nx = static_cast<std::uint16_t>((q.x + 1.0f) * params.cloudScale + drift);
    const auto ny = static_cast<std::uint16_t>((q.y + 1.0f) * params.cloudScale);
    const auto nz = static_cast<std::uint16_t>((q.z + 1.0f) * params.cloudScale);
    const float n = static_cast<float>(inoise8(nx, ny, nz)) / 255.0f;
    const float cloud = smoothstepf(params.cloudThreshold, 1.0f, n);

    // Clouds hide the sun and lighten the sky toward a sunlit white.
    sun *= (1.0f - cloud * params.cloudSunOcclusion);
    CRGB lit = lerpColor(color, params.cloudColor, cloud * params.cloudCoverage);
    lit = lerpColor(lit, params.sunColor, cloud * params.cloudSunTint);

    color = addColor(lit, scaleColor(params.sunColor, sun));
    pixels[i] = scaleColor(color, params.exposure);
  }
}

} // namespace orb
