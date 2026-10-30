# Weather Orb

This repo is the firmware and designs for the Weather Orb, a desktop ornament for the current weather outside, emulating the current weather right on your desk. This is done with an ESP32-S3, RGB LED Strip, and small LCD Display, along with a web app for configuration.

## Problem Breakdown

These are the steps of the firmware that need to be defined:

### WiFi Connection

This needs to be done to get the current location, and query the OpenMeteo API for the current weather.

### RGB LED Strip

This is lit up inside the opaque orb to emulate the weather currently taking place. It should use the LEDs to give a simulation of the sky on your desk.

### LCD Display

This is done to show the user the current location, and give other metrics like temperature, AQI, and weather for the coming days. The current design is to use a ST77916 QSPI LCD Display, measuring 1.53 inches in diameter.

## Hardware Info

### ESP32-S3

The board used for this is the ESP32-S3 Super Mini. Originally the ESP32-WROOM-32D was chosen, but the QSPI display required more RAM than could be provided. The ESP32-S3 has 2 MB of PSRAM that can be used for the QSPI display.

### ST77916 QSPI LCD Display

The display is a single chip TFT-LCD controller driver supporting 262k colors and a 360x360 resolution, at 1.53 inches in diameter. It uses QSPI and has the following pinouts on its dev board:

- **VCC:** Input voltage, range is 3.3V to 5V
- **GND:** Ground
- **SCL:** Clock
- **SDA:** Data line 0, Master Out Slave In (MOSI)
- **IO1:** Data line 1, Master In Slave Out (MISO)
- **IO2:** Data line 2, Write Protect (WP)
- **IO3:** Data line 3, Hold/Reset (HD)
- **CS:** Chip Select
- **BL:** Backlight
