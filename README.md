# LVGL Accelerometer Chart on Multiple Displays

Real-time accelerometer chart running on Zephyr RTOS with LVGL, targeting different TFT/OLED displays on a Nordic nRF52832. The application reads X, Y, Z data from an NXP FXOS8700CQ sensor and plots it as a live chart with per-axis G-force readout.

## Supported Displays

| Display | Controller | Resolution | Interface | Status |
|---------|-----------|------------|-----------|--------|
| TFT 240x320 | ILI9341 | 240x320 | SPI (MIPI DBI) | Tested |
| TFT 240x320 | ST7789V | 240x320 | SPI (MIPI DBI) | Tested |

Switching between displays requires only changing the board overlay file — the application code stays the same.

## Hardware

- **MCU**: Nordic nRF52832 (custom board `bruno_nrf52832`)
- **Accelerometer**: NXP FXOS8700CQ (I2C, address 0x1E)
- **Display**: SPI TFT via MIPI DBI interface
- **RTOS**: Zephyr 4.2 / nRF Connect SDK v3.2.1
- **Graphics**: LVGL 9.3

## What It Does

- Reads accelerometer XYZ at a configurable sample rate
- Plots a scrolling line chart (red = X, blue = Y, green = Z)
- Shows live values in G on screen (e.g. `X:0.03G  Y:-0.01G  Z:1.00G`)
- Values scaled x100 internally for integer chart resolution

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

## Building

```bash
cd app
west build -b bruno_nrf52832
west flash
```

## Adding a New Display

1. Create or edit the board overlay (`boards/bruno_nrf52832_nrf52832.overlay`)
2. Define the MIPI DBI node with the new display controller compatible
3. Point `zephyr,display` chosen node to the new display
4. Build — no changes needed in `main.c`

## License

SPDX-License-Identifier: Apache-2.0
