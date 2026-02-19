LVGL Accelerometer Chart - Multi-Display Reference
===================================================

Overview
********

Real-time accelerometer chart using **LVGL** on **Zephyr RTOS** with an
**NXP FXOS8700CQ** sensor (I2C) on a custom **nRF52832** board.

Each git branch contains a fully working configuration for a specific display.
Switch branches to get the complete overlay, Kconfig and source code ready to
build and flash.

Available Displays
******************

.. list-table::
   :header-rows: 1
   :widths: 30 20 15 15 20

   * - Branch
     - Display
     - Resolution
     - Interface
     - Notes
   * - ``display/sh1106-128x64-mono``
     - SH1106 (SSD1306-compatible)
     - 128x64
     - SPI
     - Monochrome OLED, 1-bit color depth. Line styles: X=solid, Y=dotted, Z=dashed.
   * - ``display/ssd1331-nkk-smartswitch``
     - NKK ISF15ACP4 SmartSwitch
     - 96x64
     - SPI (MIPI DBI)
     - RGB OLED built into a pushbutton switch. Uses NKK contrast values.
   * - ``display/ssd1331-96x64-rgb``
     - SSD1331 (generic module)
     - 96x64
     - SPI (MIPI DBI)
     - Chinese RGB OLED module.
   * - ``display/st7789v-240x240``
     - ST7789V
     - 240x240
     - SPI (MIPI DBI)
     - Small square IPS TFT display.
   * - ``display/st7789v-240x320``
     - ST7789V
     - 240x320
     - SPI (MIPI DBI)
     - Standard IPS TFT display.
   * - ``display/ili9341-240x320``
     - ILI9341
     - 240x320
     - SPI (MIPI DBI)
     - TFT display with FXOS8700CQ accelerometer chart.
   * - ``display/ili9341-240x320-touch``
     - ILI9341 + XPT2046
     - 240x320
     - SPI (MIPI DBI)
     - Same as above with XPT2046 resistive touch support.

Hardware
********

* **MCU**: nRF52832 (custom board ``bruno_nrf52832``)
* **Sensor**: NXP FXOS8700CQ accelerometer on I2C0 (address 0x1E)
* **Display**: varies per branch (see table above), all on SPI1

Pin assignments (directly on the display or via MIPI DBI, depending on the
branch):

* SCK: P0.11
* MOSI: P0.12
* CS: P0.19
* DC: P0.20
* RESET: P0.22

Building and Flashing
*********************

1. Clone the repository and checkout the branch for your display::

      git clone <repo-url>
      git checkout display/sh1106-128x64-mono   # example

2. Build and flash::

      west build -b bruno_nrf52832/nrf52832
      west flash

Requirements
************

* nRF Connect SDK (tested with v3.2.1 / Zephyr 4.2)
* A custom ``bruno_nrf52832`` board definition (or adapt the overlay to your board)
* FXOS8700CQ accelerometer wired to I2C0
* One of the supported displays wired to SPI1

License
*******

Apache-2.0
