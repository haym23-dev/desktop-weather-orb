#pragma once

#include <ST77916_QSPI.h>
#include <P5.h>

#include <array>
#include <cstdint>

class LcdDisplay {

public:
  /**
   * Constructor
   * 
   * @param display - QSPI object reference
   * @param p5 - P5 drawing object refernce
   */
  LcdDisplay(ST77916_QSPI_GFX& display, P5& p5) noexcept;

  /**
   * Call once display has started
   */
  void begin();

  /**
   * Call every loop, draw one frame and present it
   */
  void update();

  /**
   * Set background color
   * 
   * @param negative - White if true, black if false
   */
  void setNegative(bool negative) noexcept;

private:
  void reportStats(std::size_t movement, std::uint32_t drawUs);

  // References to display and P5, defined in main
  ST77916_QSPI_GFX& tft_;
  P5& p5_;

  std::uint16_t paper_;
  std::uint16_t ink_;
  std::uint32_t t0_ { 0u };

  struct Stats {
    std::uint32_t lastReportMs { 0u };
    std::uint32_t accDrawUs { 0u };
    std::uint32_t frames{ 0u };
  } stats_;
};
