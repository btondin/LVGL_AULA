# LVGL Accelerometer Chart - SH1106 128x64 Monochrome OLED

Real-time accelerometer chart using **LVGL** on **Zephyr RTOS** with an **NXP FXOS8700CQ** sensor on a **128x64 monochrome OLED** display driven by a **SH1106** controller (SSD1306-compatible) via SPI.

## Display

- **Controller**: SH1106 (`sinowealth,sh1106`)
- **Resolution**: 128x64
- **Color**: Monochrome (1-bit), OLED
- **Interface**: SPI native (no MIPI DBI)
- **Color depth**: `CONFIG_LV_COLOR_DEPTH_1=y`

The SH1106 has 132 internal columns but only 128 visible, so `segment-offset = <2>` is used in the devicetree overlay.

Chart series are differentiated by line style since color is not available:
- **X axis**: solid line
- **Y axis**: dotted line
- **Z axis**: dashed line

## Hardware

- **MCU**: Nordic nRF52832 (custom board `bruno_nrf52832`)
- **Sensor**: NXP FXOS8700CQ on I2C0 (address 0x1E)
- **Display**: SH1106 128x64 OLED on SPI1

### Pinout

| Signal | Pin |
|--------|-----|
| SCK | P0.11 |
| MOSI | P0.12 |
| CS | P0.19 |
| DC | P0.20 |
| RESET | P0.22 |

## Building

```bash
cd app
west build -b bruno_nrf52832/nrf52832
west flash
```

## Other Displays

This repository has one branch per display configuration. See all available branches on the [repository page](https://github.com/btondin/LVGL_AULA).

## License

Apache-2.0
