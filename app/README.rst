LVGL Accelerometer Chart - ST7789V 240x240 IPS TFT
====================================================

Overview
********

Real-time accelerometer chart using **LVGL** on **Zephyr RTOS** with an
**NXP FXOS8700CQ** sensor on a small **240x240 square IPS TFT** display
driven by an **ST7789V** controller via SPI.

Display
*******

* **Controller**: ST7789V
* **Resolution**: 240x240
* **Color**: RGB 16-bit
* **Interface**: SPI via MIPI DBI

Chart series use color-coded lines: X=red, Y=blue, Z=green.

Hardware
********

* **MCU**: nRF52832 (custom board ``bruno_nrf52832``)
* **Sensor**: NXP FXOS8700CQ on I2C0 (address 0x1E)
* **Display**: ST7789V 240x240 IPS TFT on SPI1

Pinout
------

* SCK: P0.11
* MOSI: P0.12
* CS: P0.19
* DC: P0.20
* RESET: P0.22

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
