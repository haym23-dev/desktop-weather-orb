#include "lcd_helper.h"

#include <Arduino.h>
#include <ST77916_QSPI.h>
#include <P5.h>

#include <cstdint>

namespace {
  constexpr float CX = 179.5f;
  constexpr float CY = 179.5f;
  constexpr float RIM = 172.0f;
  constexpr float TAU = 6.28318531f;

  constexpr uint32_t MOVEMENT_MS = 9000u;
  const uint8_t MOVEMENTS = 6u;
} // namespace

void LcdDisplay::begin() {
  p5_.noStroke();
  p5_.stroke(ink_);
  p5_.noFill();
  p5_.noiseSeed(20260723);
  t0_ = millis();
  stats_ = {};
}

void LcdDisplay::setNegative(bool negative) {
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
  (this->*kMovements.at(idx))(f);
  const std::uint32_t drawUs = micros() - tDraw;

  tft_.present();
  reportStats(idx, drawUs);
}