#include "lcd_display.h"
#include "weather_lcd.hpp"

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

  constexpr int8_t SCL_PIN = 12;
  constexpr int8_t SDA_PIN = 11;
  constexpr int8_t IO1_PIN = 13;
  constexpr int8_t IO2_PIN = 5;
  constexpr int8_t IO3_PIN = 9;
  constexpr int8_t RST_PIN = 8;
  constexpr int8_t CS_PIN  = 10;
  constexpr int8_t BL_PIN  = -1;   // Set to HIGH

  // --------------------------------------------------------------------------
  // Weather States
  // --------------------------------------------------------------------------
  /**
   * Draw sun with rays
   */
  void drawSunny(P5& p5) {
    p5.noStroke();
    p5.fill(0xe565);
    uint yPos = CY - 50.0f;
    p5.circle(CX, yPos, 80);

    // Sun rays
    const uint8_t numTriangles = 6;
    const uint8_t distance = 65;
    const uint8_t triangleSize = 10;

    // Draw sun rays
    for (std::uint8_t i = 0; i < numTriangles; ++i) {
      // Index to radian angle
      const float angle = TAU * static_cast<float>(i) / static_cast<float>(numTriangles);
      const float x = CX + std::cos(angle) * distance;
      const float y = yPos + std::sin(angle) * distance;

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
} // namespace


LcdDisplay::LcdDisplay() noexcept :
    tft_{
      SCL_PIN,
      SDA_PIN,
      IO1_PIN,
      IO2_PIN,
      IO3_PIN,
      RST_PIN,
      CS_PIN,
      BL_PIN,
    }, p5_{tft_}
{
}

bool LcdDisplay::init() {
  ST77916_QSPI_GFX::Config cfg;
  cfg.clockHz = 70000000;   // 70 MHz
  cfg.doubleBuffer = true;
  cfg.pushTask = true;

  if (!tft_.begin(cfg)) {
    Serial.printf("tft begin failed: %s\n", tft_.lastError());
    return false;
  }
  return true;
}

void LcdDisplay::begin() {
  p5_.noStroke();
  p5_.background(ST77916_BLACK);
  p5_.stroke(ST77916_WHITE);
  p5_.noFill();
  p5_.noiseSeed(20260723);
  t0_ = millis();
  stats_ = {};
}

void LcdDisplay::update() {
  // Clear first: with double buffering the next frame is drawn into the buffer
  // shown two frames ago, so without this the animated sun smears across the
  // panel instead of moving over a clean background.
  p5_.background(ST77916_BLACK);
  weatherShapes::drawSunny(p5_, CX, CY, 40);
  tft_.present();
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
