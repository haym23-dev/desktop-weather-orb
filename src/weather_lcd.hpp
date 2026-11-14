#pragma once

#include <P5.h>

namespace weatherShapes {

/**
 * drawSunny - Sun only animation
 * 
 * @param p5 - P5 display instance
 * @param x - Starting x location
 * @param y - Starting y location
 * @param size - Size of sun
 */
void drawSunny(P5& p5, float x, float y, float size = 100.f);

/**
 * drawPartlySunny - Sun and cloud animation
 */
void drawPartlySunny(P5& p5, float x, float y, float size = 100.f);

/**
 * drawCloudy - Cloud animation
 * 
 * @param P5 - P5 display instance
 * @param x - Starting x location
 * @param y - Starting y location
 * @param size - Size of clouds
 */
void drawCloudy(P5& p5, float x, float y, float size = 100.0f);
} // namespace weatherShapes
