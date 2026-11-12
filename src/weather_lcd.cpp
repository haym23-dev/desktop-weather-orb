#include "weather_lcd.hpp"

#include <Arduino.h>
#include <P5.h>

namespace weatherShapes {
  namespace {
    constexpr int SUN         = 0xfda0;
    constexpr int SUN_CORE    = 0xfe47;
    constexpr int CLOUD       = 0xf7be;
    constexpr int CLOUD_SHADE = 0xc67a;
    constexpr int STORM_CLOUD = 0x8cd4;
    constexpr int STORM_SHADE = 0x634f;
    constexpr int STORM_FLASH = 0xd71c;
    constexpr int RAIN        = 0x559f;
    constexpr int SNOW        = 0xdf9f;
    constexpr int BOLT        = 0xfea1;
    constexpr int MOON        = 0xef13;
    constexpr int STAR        = 0xef13;
    constexpr int FOG         = 0xadb8;

    void begin(P5& p5, float x, float y, float size) {
      p5.push();
      p5.translate(x, y);
      p5.scale(size / 100.0f);
    }

    void end(P5& p5) {
      p5.pop();
    }

    float colorTime() {
      return millis() / 1000.0f;
    }

    float mapFloat(float value, float start1, float stop1, float start2, float stop2) {
      if (start1 == stop1) {
        return start2;
      }
      return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
    }

    void sun(P5& p5, 
            float cx, float cy, float r, 
            uint8_t rays = 10u, float spin = 0.0f, float pulse = 0.0f) {
      p5.push();
      p5.translate(cx, cy);
      p5.noStroke();
      p5.fill(SUN);
      p5.push();
      p5.rotate(spin);
      float distance = r * 1.5f;
      float t = r * 0.28f;
      for (int i = 0; i < rays; ++i) {
        float angle = mapFloat(
          static_cast<float>(i), 
          0.0f, static_cast<float>(rays), 
          0.0f, static_cast<float>(TWO_PI)
        );

        p5.push();
        p5.translate(cos(angle) * distance, sin(angle) * distance);
        p5.rotate(angle + HALF_PI);
        p5.triangle(-t, t, t, t, 0.0f, -t);
        p5.pop();
      }
      p5.pop();
      p5.fill(SUN_CORE);
      p5.circle(0.0f, 0.0f, r * 2.0f * (1.0f + pulse));
      p5.pop();
    }

    void cloudShape(P5& p5, float width) {
      float height = width * 0.32;
      p5.rectMode(P5_CENTER);
      p5.rect(0.0f, height * 0.25f, width, height);
      p5.circle(-width * 0.22f, 0.0f, width * 0.42f);
      p5.circle(width * 0.08f, -width * 0.12f, width * 0.56);
      p5.circle(width * 0.3f, height * 0.05f, width * 0.36f);
    }

    void cloud(P5& p5, float cx, float cy, float w) {
      p5.push();
      p5.translate(cx, cy);
      p5.noStroke();
      p5.push();
      p5.translate(w * 0.03f, w * 0.04f);
      p5.fill(CLOUD_SHADE);
      cloudShape(p5, w);
      p5.pop();
      p5.fill(CLOUD);
      cloudShape(p5, w);
      p5.pop();
    }
  } // namespace

  void drawSunny(P5& p5, float x, float y, float size = 100.0f) {
    float t = colorTime();
    begin(p5, x, y, size);
    sun(p5, 0.0f, 0.0f, 40.0f, 10, t * 0.25, 0.04 * sin(t * 2.0f));
    end(p5);
  }

  void drawPartlySunny(P5& p5, float x, float y, float size = 100.0f) {
    float t = colorTime();
    begin(p5, x, y, size);
    sun(p5, -12.0f, -12.0f, 16.0f, 8, t * 0.3f);
    cloud(p5, 6.0f + sin(t * 0.7f) * 4, 12.0f, 66.0f);
    end(p5);
  }
} // namespace weatherShapes