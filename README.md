# LVGL Accelerometer Chart - Multi-Display Reference

Real-time accelerometer chart using **LVGL** on **Zephyr RTOS** with an **NXP FXOS8700CQ** sensor (I2C) on a custom **nRF52832** board.

Each git branch contains a fully working configuration for a specific display. Switch branches to get the complete overlay, Kconfig and source code ready to build and flash.

## Available Displays

| Branch | Display | Resolution | Interface | Notes |
|--------|---------|------------|-----------|-------|
| `display/sh1106-128x64-mono` | SH1106 (SSD1306-compatible) | 128x64 | SPI | Monochrome OLED, 1-bit. Lines: X=solid, Y=dotted, Z=dashed |
| `display/ssd1331-nkk-smartswitch` | NKK ISF15ACP4 SmartSwitch | 96x64 | SPI (MIPI DBI) | RGB OLED built into pushbutton switch |
| `display/ssd1331-96x64-rgb` | SSD1331 (generic module) | 96x64 | SPI (MIPI DBI) | Chinese RGB OLED module |
| `display/st7789v-240x240` | ST7789V | 240x240 | SPI (MIPI DBI) | Small square IPS TFT |
| `display/st7789v-240x320` | ST7789V | 240x320 | SPI (MIPI DBI) | Standard IPS TFT |
| `display/ili9341-240x320` | ILI9341 | 240x320 | SPI (MIPI DBI) | TFT display |
| `display/ili9341-240x320-touch` | ILI9341 + XPT2046 | 240x320 | SPI (MIPI DBI) | TFT with resistive touch |

## Hardware

- **MCU**: Nordic nRF52832 (custom board `bruno_nrf52832`)
- **Accelerometer**: NXP FXOS8700CQ (I2C, address 0x1E)
- **Display**: varies per branch (see table above), all on SPI1
- **RTOS**: Zephyr 4.2 / nRF Connect SDK v3.2.1
- **Graphics**: LVGL 9.3

## Pinout

| Signal | Pin |
|--------|-----|
| SCK | P0.11 |
| MOSI | P0.12 |
| CS | P0.19 |
| DC | P0.20 |
| RESET | P0.22 |

## Building

```bash
git checkout display/sh1106-128x64-mono   # pick your display
cd app
west build -b bruno_nrf52832/nrf52832
west flash
```

## Project Structure

```
LVGL_AULA/
├── app/
│   ├── src/
│   │   └── main.c                              # Application
│   ├── boards/
│   │   └── bruno_nrf52832_nrf52832.overlay      # Display & sensor devicetree
│   ├── CMakeLists.txt
│   └── prj.conf
└── README.md
```

## License

Apache-2.0
