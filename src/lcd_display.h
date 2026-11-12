#pragma once

#include <ST77916_QSPI.h>
#include <P5.h>

class LcdDisplay {

public:
  /**
   * Constructor
   */
  LcdDisplay() noexcept;

  /**
   * Initialise the panel hardware (SPI bus + framebuffers + panel init).
   *
   * Deliberately NOT done in the constructor: the constructor runs before
   * Serial/PSRAM are up, so a failure there is invisible and a halt() would
   * stop setup() from ever running. Call from setup(), after logPSRAM().
   *
   * @return false if the panel could not be brought up (display left inert)
   */
  bool init();

  /**
   * Call once display has started
   */
  void begin();

  /**
   * Call every loop, draw one frame and present it
   */
  void update();

private:
  void reportStats(std::size_t movement, std::uint32_t drawUs);

  // References to display and P5, defined in main
  ST77916_QSPI_GFX tft_;
  P5 p5_;

  std::uint32_t t0_ { 0u };

  struct Stats {
    std::uint32_t lastReportMs { 0u };
    std::uint32_t accDrawUs { 0u };
    std::uint32_t frames{ 0u };
  } stats_;
};
