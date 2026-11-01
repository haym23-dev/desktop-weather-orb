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

  float lerpf(float a, float b, float t) noexcept {
    return a + (b - a) * t;
  }

  Rgb lerpColor(const Rgb& a, const Rgb& b, float t) noexcept {
    t = clampf(t, 0.0f, 1.0f);
    return Rgb{static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
               static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
               static_cast<std::uint8_t>(a.b + (b.b - a.b) * t)};
  }

  Rgb scaleColor(const Rgb& c, float s) noexcept {
    return Rgb{static_cast<std::uint8_t>(clampf(c.r * s, 0.0f, 255.0f)),
               static_cast<std::uint8_t>(clampf(c.g * s, 0.0f, 255.0f)),
               static_cast<std::uint8_t>(clampf(c.b * s, 0.0f, 255.0f))};
  }

  // --------------------------------------------------------------------------
  // 3D value noise, 0..1. Replaces FastLED's inoise8 so the scene has no
  // dependency on FastLED.
  // --------------------------------------------------------------------------
  std::uint8_t hash3(std::int32_t x, std::int32_t y, std::int32_t z) noexcept {
    std::uint32_t h = static_cast<std::uint32_t>(x) * 374761393u
                    + static_cast<std::uint32_t>(y) * 668265263u
                    + static_cast<std::uint32_t>(z) * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return static_cast<std::uint8_t>(h);
  }

  float valueNoise3(float x, float y, float z) noexcept {
    const std::int32_t xi = static_cast<std::int32_t>(std::floor(x));
    const std::int32_t yi = static_cast<std::int32_t>(std::floor(y));
    const std::int32_t zi = static_cast<std::int32_t>(std::floor(z));

    const float u = smoothstepf(0.0f, 1.0f, x - static_cast<float>(xi));
    const float v = smoothstepf(0.0f, 1.0f, y - static_cast<float>(yi));
    const float w = smoothstepf(0.0f, 1.0f, z - static_cast<float>(zi));

    const float c000 = hash3(xi,     yi,     zi)     / 255.0f;
    const float c100 = hash3(xi + 1, yi,     zi)     / 255.0f;
    const float c010 = hash3(xi,     yi + 1, zi)     / 255.0f;
    const float c110 = hash3(xi + 1, yi + 1, zi)     / 255.0f;
    const float c001 = hash3(xi,     yi,     zi + 1) / 255.0f;
    const float c101 = hash3(xi + 1, yi,     zi + 1) / 255.0f;
    const float c011 = hash3(xi,     yi + 1, zi + 1) / 255.0f;
    const float c111 = hash3(xi + 1, yi + 1, zi + 1) / 255.0f;

    const float x00 = lerpf(c000, c100, u);
    const float x10 = lerpf(c010, c110, u);
    const float x01 = lerpf(c001, c101, u);
    const float x11 = lerpf(c011, c111, u);
    const float y0  = lerpf(x00, x10, v);
    const float y1  = lerpf(x01, x11, v);
    return lerpf(y0, y1, w);
  }
} // namespace

void renderSunny(Rgb* pixels,
                 const LedPoint* points,
                 std::uint16_t count,
                 const SunnyParams& params,
                 std::uint32_t timeMs) noexcept {
  constexpr float kDegToRad = 0.01745329252f;
  constexpr float kRadToDeg = 57.29577951f;

  // Sun direction as a unit vector.
  const float sunEl   = params.sunElevationDeg * kDegToRad;
  const float sunAz   = params.sunAzimuthDeg * kDegToRad;
  const float sunZ    = std::sin(sunEl);
  const float sunRing = std::cos(sunEl);
  const float sunX    = sunRing * std::cos(sunAz);
  const float sunY    = sunRing * std::sin(sunAz);

  // Clouds translate slowly across the sphere.
  const float drift = static_cast<float>(timeMs) * 0.001f * params.cloudDrift * (1.0f / 128.0f);

  // Map the orb's direction (roughly -1..1) onto noise lattice coordinates.
  constexpr float kCloudSpatial = 1.0f / 64.0f;

  // Slow global breathing. On a heavily diffused orb, a whole-orb brightness
  // pulse reads far better than spatial cloud motion.
  const float breathe = 1.0f + 0.12f * std::sin(static_cast<float>(timeMs) * 0.0008f);

  for (std::uint16_t i = 0; i < count; ++i) {
    const LedPoint& q = points[i];

    // 1. Sky gradient, pale at the base fading to blue overhead.
    const float up = smoothstepf(0.0f, 1.0f, q.altitude);
    Rgb color = lerpColor(params.horizonColor, params.zenithColor, up);

    // Bias the whole orb warm. With a central former the diffuser averages all
    // pixels together, so a pure blue sky drags the result to white.
    color = lerpColor(color, params.sunColor, params.sunAmbient);

    // 2. Sun disk plus glow, keyed off the angle between LED and sun.
    const float dot = clampf(q.x * sunX + q.y * sunY + q.z * sunZ, -1.0f, 1.0f);
    const float angleDeg = std::acos(dot) * kRadToDeg;
    const float glow = 1.0f - smoothstepf(params.sunCoreDeg, params.sunGlowDeg, angleDeg);
    const float core = 1.0f - smoothstepf(params.sunCoreDeg * 0.4f, params.sunCoreDeg, angleDeg);
    float sun = (glow * 0.5f + core) * params.sunIntensity;

    // 3. Seamless cloud cover from 3D value noise on the LED direction.
    const float nx = (q.x + 1.0f) * params.cloudScale * kCloudSpatial + drift;
    const float ny = (q.y + 1.0f) * params.cloudScale * kCloudSpatial;
    const float nz = (q.z + 1.0f) * params.cloudScale * kCloudSpatial;
    const float n = valueNoise3(nx, ny, nz);
    const float cloud = smoothstepf(params.cloudThreshold, 1.0f, n);

    // Clouds soften the sun and add a warm haze.
    sun *= (1.0f - cloud * params.cloudSunOcclusion);
    Rgb lit = lerpColor(color, params.cloudColor, cloud * params.cloudCoverage);
    lit = lerpColor(lit, params.sunColor, cloud * params.cloudSunTint);

    // 4. Tint the sky toward the sun instead of adding to it. Adding warm on
    //    top of blue saturates to white; tinting keeps the sun visibly golden.
    lit = lerpColor(lit, params.sunColor, clampf(sun, 0.0f, 1.0f));
    lit = lerpColor(lit, params.sunHotColor,
                    clampf(core * params.sunIntensity, 0.0f, 1.0f) * 0.85f);

    pixels[i] = scaleColor(lit, params.exposure * breathe);
  }
}

} // namespace orb
