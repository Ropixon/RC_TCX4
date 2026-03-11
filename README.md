# RC_TCX4

Firmware for the TCX4 handheld remote control.

## Hardware

| Component | Description |
|---|---|
| MCU | STM32L083RB (ARM Cortex-M0+, low-power) |
| Radio | Silicon Labs Si4463 (sub-GHz transceiver) |
| Frequency | 869.525 MHz (SRD band, Europe) |
| Modulation | 2-FSK |
| Data rate | 9.6 kbps |
| Frequency deviation | ±12.5 kHz |
| Display | Segmented LCD |

## Overview

RC_TCX4 is firmware for a specialized handheld remote control. It communicates via the Si4463 radio module configured to operate at 869.525 MHz in the sub-GHz SRD band permitted license-free in Europe (ETSI EN 300 220).

The firmware targets the STM32L083 with a focus on low power consumption — it uses a tickless sleep mode with wake-up via RTC and LPTIM. Radio configuration is generated using Silicon Labs WDS (Wireless Development Suite).

## Features

- Keypad handling (buttons: +, -, LEFT, RIGHT + backlight)
- Segmented LCD display with segment blinking support
- TX/RX via Si4463 (FSK, 9.6 kbps)
- Runtime RF frequency calculation and configuration
- Configuration storage in internal FLASH
- HW version detection via GPIO pin-strapping
- Watchdog (WWDG) + CRC data integrity check

## Project Structure

```
Core/
  Inc/          - header files
  Src/
    Core_Task/  - keypad handling (KeyBoardProcess)
    DisplayTask/ - LCD display control
    RadioSI446x/ - Si4463 HAL layer
    Tickless/   - low-power sleep management (RTC)
Drivers/
  STM32L0xx_LL_Driver/ - STM32 LL drivers (no HAL overhead)
  CMSIS/               - ARM CMSIS core
```

## Toolchain

- STM32CubeIDE (GCC for ARM)
- STM32L0xx LL drivers (Low Layer API)
- Silicon Labs WDS for Si4463 configuration generation
