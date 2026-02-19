LVGL Accelerometer Chart - ILI9341 240x320 TFT + XPT2046 Touch
================================================================

Overview
********

Real-time accelerometer chart using **LVGL** on **Zephyr RTOS** with an
**NXP FXOS8700CQ** sensor on a **240x320 TFT** display driven by an
**ILI9341** controller via SPI, with **XPT2046** resistive touch input.

Display
*******

* **Controller**: ILI9341
* **Resolution**: 240x320
* **Color**: RGB 16-bit
* **Interface**: SPI via MIPI DBI
* **Touch**: XPT2046 resistive touch controller sharing SPI1 bus

The XPT2046 touch controller shares the SPI1 bus with the display.
Touch X-axis is inverted via ``lvgl-pointer-input`` to match display
rotation. A test button with press counter is included.

Chart series use color-coded lines: X=red, Y=blue, Z=green.

Hardware
********

* **MCU**: nRF52832 (custom board ``bruno_nrf52832``)
* **Sensor**: NXP FXOS8700CQ on I2C0 (address 0x1E)
* **Display**: ILI9341 240x320 TFT on SPI1
* **Touch**: XPT2046 on SPI1

Pinout
------

* SCK: P0.11
* MOSI: P0.12
* Display CS: P0.19
* DC: P0.20
* RESET: P0.22
* Touch CS: P0.16
* Touch IRQ: P0.17

Building and Flashing
*********************

::

    west build -b bruno_nrf52832/nrf52832
    west flash

Other Displays
**************

This repository has one branch per display configuration.
See all available branches at the
`repository page <https://github.com/btondin/LVGL_AULA>`_.

License
*******

Apache-2.0
