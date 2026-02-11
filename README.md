# SSD1306 Display Driver for Zephyr RTOS

A Zephyr RTOS display driver implementation for SSD1306 OLED displays (128x64 monochrome), adapted from the ST7789V color display driver architecture. This project targets Nordic nRF52832 microcontrollers and demonstrates integration with Zephyr's display subsystem.

## Features

- **SSD1306 OLED Support**: Driver implementation for 128x64 monochrome OLED displays using I2C/SPI interface
- **Zephyr Display API**: Full integration with Zephyr's standard display subsystem
- **Nordic nRF52832**: Optimized for Nordic Semiconductor's nRF52 series
- **Hardware Abstraction**: Clean separation between hardware interface and display logic
- **LVGL Ready**: Prepared for integration with LittlevGL graphics library

## Technical Foundation

This driver is based on the ST7789V implementation and adapted for the SSD1306's unique characteristics:

- **Pixel Format**: Monochrome (1-bit per pixel) vs RGB565
- **Memory Architecture**: Page-based memory layout vs pixel-based
- **Command Set**: Adapted for SSD1306 controller differences
- **Resolution**: 128x64 vs ST7789's typical 240x240

## Target Hardware

- **MCU**: Nordic nRF52832 (ARM Cortex-M4F)
- **Display**: SSD1306-based OLED (128x64)
- **Interface**: I2C or SPI (configurable via devicetree)
- **RTOS**: Zephyr RTOS 3.2.1+

## Project Structure

```
SSD1306_DISP_ZEPHYR/
├── app/
│   ├── src/
│   │   └── main.c              # Application entry point
│   ├── boards/
│   │   └── *.overlay           # Board-specific devicetree overlays
│   ├── CMakeLists.txt          # Build configuration
│   └── prj.conf                # Project configuration
└── README.md
```

## Getting Started

### Prerequisites

- Zephyr RTOS SDK 3.2.1 or later
- Nordic nRF Connect SDK (optional, for nRF52 support)
- ARM GCC toolchain
- CMake 3.21+

### Building

```bash
cd app
west build -b bruno_nrf52832_nrf52832
```

### Flashing

```bash
west flash
```

## Configuration

The driver can be configured through `prj.conf`:

```conf
# Display configuration
CONFIG_DISPLAY=y
CONFIG_MIPI_DBI=y
CONFIG_MIPI_DBI_SPI=y

# Logging
CONFIG_LOG=y
CONFIG_DISPLAY_LOG_LEVEL_DBG=y
```

## Devicetree Configuration

Configure the SSD1306 display in your board overlay file:

```dts
&spi0 {
    status = "okay";

    ssd1306: ssd1306@0 {
        compatible = "solomon,ssd1306fb";
        reg = <0>;
        spi-max-frequency = <4000000>;
        /* Additional properties */
    };
};
```

## Project Status

**Work in Progress** - Currently adapting ST7789V color display driver architecture to SSD1306 monochrome OLED requirements.

### Roadmap

- [ ] Implement SSD1306 command set
- [ ] Add page-based memory management
- [ ] Implement monochrome pixel format conversion
- [ ] Add I2C interface support
- [ ] Integrate with LVGL
- [ ] Add example applications

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This project follows the Zephyr Project licensing:
- SPDX-License-Identifier: Apache-2.0

## References

- [Zephyr Display Driver Documentation](https://docs.zephyrproject.org/latest/hardware/peripherals/display.html)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [ST7789V Reference Implementation](https://github.com/zephyrproject-rtos/zephyr/tree/main/drivers/display)

## Author

Developed as part of embedded systems development with Zephyr RTOS and Nordic nRF52 series microcontrollers.
