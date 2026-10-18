# Weather Orb

This repo is the firmware and designs for the Weather Orb, a desktop ornament for the current weather outside, emulating the current weather right on your desk. This is done with an ESP32, RGB LED Strip, and small LCD Display, along with a web app for configuration.

## Problem Breakdown

These are the steps of the firmware that need to be defined:

### WiFi Connection

This needs to be done to get the current location, and query the OpenMeteo API for the current weather.

### RGB LED Strip

This is lit up inside the opaque orb to emulate the weather currently taking place. It should use the LEDs to give a simulation of the sky on your desk.

### LCD Display

This is done to show the user the current location, and give other metrics like temperature, AQI, and weather for the coming days. The current design is to use a ST77916 QSPI LCD Display, measuring 1.53 inches in diameter.
