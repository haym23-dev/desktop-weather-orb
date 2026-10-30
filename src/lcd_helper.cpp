#include "lcd_helper.h"

#include <Arduino.h>
#include <ST77916_QSPI.h>
#include <P5.h>

namespace {
  constexpr float CX  = 179.5f;
  constexpr float CY  = 179.5f;
  constexpr float RIM = 172.0f;
  constexpr float TAU = 6.28318531f;

  constexpr uint32_t MOVEMENT_MS  = 9000u;
  constexpr uint32_t REPORT_MS    = 100u;

  struct Frame {
    float u;    // 0..1 progress through the current movement
    float t;    // seconds since begin()
    float env;  // grow in / hold / collapse-out envelope, 0..1
  };

  // --------------------------------------------------------------------------
  // Examples - Remove later
  // --------------------------------------------------------------------------
  /**
   * Times Table - n points on a circle, i joined to (i*k) mod n
   */
  void timesTable(P5& p5, const Frame& f) {
    constexpr int N = 160;
    const float k = 2.0f + 7.0f * f.u;
    const float R = RIM * (0.88f + 0.05f * std::sin(f.t * 0.6f)) * (0.40f + 0.60f * f.env);
  
    p5.strokeWeight(1.6f);
    p5.circle(CX, CY, R * 2.0f);
  
    p5.strokeWeight(0.9f);
    const int count = static_cast<int>(N * f.env);
    for (int i = 0; i < count; ++i) {
      const float a0 = TAU * i / N;
      const float a1 = TAU * std::fmod(i * k, static_cast<float>(N)) / N;
      p5.line(CX + R * std::cos(a0), CY + R * std::sin(a0),
              CX + R * std::cos(a1), CY + R * std::sin(a1));
    }
  }

  /**
   * Nested polygons, counter rotating
   */
  void polygons(P5& p5, const Frame& f) {
    p5.strokeWeight(2.0f);
    p5.push();
    p5.translate(CX, CY);
    for (int i = 0; i < 6; ++i) {
      const float r = RIM * (0.96f - i * 0.135f) * f.env;
      if (r < 4.0f) continue;
      const float dir = (i & 1) ? -1.0f : 1.0f;
      p5.push();
      p5.rotate(f.t * (0.25f + i * 0.11f) * dir);
      p5.polygon(0, 0, r, 3 + i);
      p5.pop();
    }
    p5.pop();
  }

  /**
   * Radial comb, teeth modulated by travelling three lobe wave
   */
  void comb(P5& p5, const Frame& f) {
    constexpr int rays = 132;
    p5.strokeWeight(2.0f);
    p5.push();
    p5.translate(CX, CY);
    p5.rotate(f.t * 0.35f);
    for (int i = 0; i < rays; ++i) {
      const float a  = TAU * i / rays;
      const float m  = 0.5f + 0.5f * std::sin(3.0f * a - f.t * 1.4f);
      const float r0 = 26.0f + 34.0f * m;
      const float r1 = r0 + (18.0f + 96.0f * m) * f.env;
      p5.line(r0 * std::cos(a), r0 * std::sin(a), r1 * std::cos(a), r1 * std::sin(a));
    }
    p5.pop();
    p5.circle(CX, CY, 8.0f + 36.0f * f.env);
  }

  /**
   * Harmonograph - two damped pendulums with drifting phase
   */
  void harmonograph(P5& p5, const Frame& f) {
    constexpr int STEPS = 600;
    const float p1 = f.t * 0.21f;
    const float p2 = f.t * 0.13f;
    const float A  = RIM * 0.44f * f.env;
  
    p5.strokeWeight(1.8f);
    float lx = 0.0f, ly = 0.0f;
    for (int i = 0; i <= STEPS; ++i) {
      const float s  = static_cast<float>(i) / STEPS;
      const float ph = s * TAU * 4.0f;
      const float d  = std::exp(-s * 0.9f);
      const float x  = CX + A * d * (std::sin(2.00f * ph + p1) + 0.6f * std::sin(3.00f * ph));
      const float y  = CY + A * d * (std::sin(3.01f * ph + p2) + 0.6f * std::sin(2.00f * ph + 1.2f));
      if (i > 0) p5.line(lx, ly, x, y);
      lx = x;
      ly = y;
    }
  }

  /**
   * Moire - Two ring families whose centers orbit apart
   */
  void moire(P5& p5, const Frame& f) {
    const float d     = 28.0f * f.env;
    const float a     = f.t * 0.5f;
    const float pitch = 11.0f + 3.0f * std::sin(f.t * 0.3f);
  
    p5.strokeWeight(1.1f);
    for (int k = 1; k <= 22; ++k) {
      const float r = k * pitch;
      if (r + d > RIM) break;
      p5.circle(CX + d * std::cos(a), CY + d * std::sin(a), r * 2.0f);
      p5.circle(CX - d * std::cos(a), CY - d * std::sin(a), r * 2.0f);
    }
    p5.strokeWeight(2.0f);
    p5.circle(CX, CY, RIM * 2.0f);
  }

  /**
   * Flow field - short strokes turned by noise
   */
  void flow(P5& p5, const Frame& f) {
    constexpr int   cells = 16;            // 16 * 22 = 352 ≈ the original ±168 span
    constexpr float step  = 22.0f;
    constexpr float span  = 168.0f;
  
    p5.strokeWeight(2.0f);
    // Integer loop counters: float accumulation can drift and add/drop a row.
    for (int yi = 0; yi <= cells; ++yi) {
      const float y = CY - span + yi * step;
      if (y > CY + span) break;
      for (int xi = 0; xi <= cells; ++xi) {
        const float x = CX - span + xi * step;
        if (x > CX + span) break;
        if (std::hypot(x - CX, y - CY) > RIM - 10.0f) continue;
        const float n   = p5.noise(x * 0.006f, y * 0.006f, f.t * 0.15f);
        const float a   = n * TAU * 2.0f;
        const float len = (5.0f + 8.0f * n) * f.env;
        p5.line(x - std::cos(a) * len, y - std::sin(a) * len,
                x + std::cos(a) * len, y + std::sin(a) * len);
      }
    }
  }


  // Array of all of the movements from this class
  using Movement = void (*)(P5&, const Frame&);
  static const std::array<Movement, 6> kMovements{
    &timesTable,
    &polygons,
    &comb,
    &harmonograph,
    &moire,
    &flow
  };
} // namespace


LcdDisplay::LcdDisplay(ST77916_QSPI_GFX& tft, P5& p5) noexcept :
    tft_{tft}, p5_{p5}, paper_{ST77916_WHITE}, ink_{ST77916_BLACK}
{}

void LcdDisplay::begin() {
  p5_.noStroke();
  p5_.stroke(ink_);
  p5_.noFill();
  p5_.noiseSeed(20260723);
  t0_ = millis();
  stats_ = {};
}

void LcdDisplay::setNegative(bool negative) noexcept {
  paper_ = negative ? ST77916_BLACK : ST77916_WHITE;
  ink_ = negative ? ST77916_WHITE : ST77916_BLACK;
}

void LcdDisplay::update() {
  const std::uint32_t now = millis() - t0_;
  const std::size_t idx = (now / MOVEMENT_MS) % kMovements.size();

  Frame f{};
  f.t = static_cast<float>(now) * 0.001f;
  f.u = static_cast<float>(now % MOVEMENT_MS) / static_cast<float>(MOVEMENT_MS);
  f.env = P5::smoothstep(0.0f, 0.18f, f.u) * (1.0f - P5::smoothstep(0.82f, 1.0f, f.u));

  const std::uint32_t tDraw = micros();
  p5_.background(paper_);
  p5_.stroke(ink_);
  (kMovements.at(idx))(p5_, f);
  const std::uint32_t drawUs = micros() - tDraw;

  tft_.present();
  reportStats(idx, drawUs);
}

void LcdDisplay::reportStats(std::size_t movement, std::uint32_t drawUs) {
  stats_.accDrawUs += drawUs;
  ++stats_.frames;

  const std::uint32_t nowMs = millis();
  if (nowMs - stats_.lastReportMs < REPORT_MS) {
    return;
  }

  Serial.printf("mv%u %lu fps   draw %lu us   push %lu us\n",
                static_cast<unsigned>(movement),
                static_cast<unsigned long>(stats_.frames),
                static_cast<unsigned long>(stats_.accDrawUs / stats_.frames),
                static_cast<unsigned long>(tft_.lastPushMicros())
  );

  stats_.lastReportMs = nowMs;
  stats_.frames = 0;
  stats_.accDrawUs = 0;
}

// --------------------------------------------------------------------------
// Weather States
// --------------------------------------------------------------------------
/**
 * Draw sun with rays
 */
void drawSunny(P5& p5) {
  p5.noStroke();
  p5.fill(0xe565);
  p5.circle(CX, CY, 40);

  // Sun rays
  const uint8_t numTriangles = 5;
  const uint8_t distance = 30;
  const uint8_t triangleSize = 10;

  // Draw sun rays
  for (std::uint8_t i = 0; i < numTriangles; ++i) {
    // Index to radian angle
    const float angle = TAU * static_cast<float>(i) / static_cast<float>(numTriangles);
    const float x = CX + std::cos(angle) * distance;
    const float y = CY + std::sin(angle) * distance;

    p5.push();
    p5.translate(x, y);
    p5.rotate(angle + (TAU * 0.25f));
    p5.triangle((-1.0f * triangleSize), triangleSize, 
                  triangleSize, triangleSize, 
                  0, (-1.0f * triangleSize));
    
    p5.pop();
  }
}
// void drawCloudy();
// void drawRainy();
// void drawClearNight();
