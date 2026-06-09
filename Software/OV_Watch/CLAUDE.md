# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

OV-Watch V2.4.5 — an open-source smartwatch firmware for **STM32F411CEU6** (Cortex-M4, 100MHz, UFQFPN48). Uses **FreeRTOS** (CMSIS-OS2) and **LVGL v8.2** for the GUI on a 240x280 ST7789 LCD.

Author: No-Chicken (不吃油炸鸡) — https://github.com/No-Chicken/OV-Watch

## Build System

- **IDE**: Keil MDK-ARM 5 (project file: `MDK-ARM/OV_Watch.uvprojx`)
- **Code generation**: STM32CubeMX (`OV_Watch.ioc`) — regenerates `Core/` peripheral init code
- **Clean build artifacts**: run `KeilClear.bat` from the project root

No command-line build is configured. Build through the Keil IDE.

## Architecture

### Layer Structure

```
User/              — Application layer
  GUI_App/         — LVGL UI code (ui.c/ui.h)
  Func/            — Hardware middleware (HWDataAccess.c/.h)
  Tasks/           — FreeRTOS task definitions
Drivers/BSP/       — Board Support Package (peripheral drivers)
Middlewares/LVGL/  — LVGL v8.2 library + display/input porting
Core/              — STM32CubeMX-generated HAL/peripheral init
```

### Hardware Abstraction Layer (HWInterface)

The central design pattern is `HWInterface` — a global struct of function pointers (`HW_InterfaceTypeDef` in `User/Func/Inc/HWDataAccess.h`) that wraps all hardware peripherals. UI and task code accesses hardware exclusively through this interface.

Each peripheral (RTC, BLE, Power, LCD, IMU, AHT21, Barometer, Ecompass, HR_meter) has a sub-struct with Init/Read/Write function pointers and data fields. The implementation lives in `User/Func/Src/HWDataAccess.c`.

### Compile-time Hardware Toggles

`HWDataAccess.h` has `#define` flags to enable/disable each peripheral driver:

```c
#define HW_USE_HARDWARE 1  // Master switch: 0 = no hardware (for PC sim)
#define HW_USE_RTC    0
#define HW_USE_BLE    0
#define HW_USE_BAT    1
#define HW_USE_LCD    1
#define HW_USE_IMU    0
#define HW_USE_AHT21  0
// ... etc
```

When `HW_USE_HARDWARE` is 0, all hardware calls become no-ops — this allows the UI layer to compile and run in a PC simulator without any BSP code.

### FreeRTOS Task Layout

Tasks are created in `User/Tasks/Src/user_TasksInit.c` (`User_Tasks_Init()`), called from `Core/Src/freertos.c` (`MX_FREERTOS_Init()`).

- **HardwareInitTask** — runs once at startup, initializes all peripherals and LVGL, then self-deletes
- **LvHandlerTask** — runs `lv_task_handler()` every 5ms (the LVGL main loop)
- **defaultTask** — STM32CubeMX default, toggles PB15 LED every 1s

LVGL tick is driven by `vApplicationTickHook()` calling `lv_tick_inc(1)` — this is in the FreeRTOS tick ISR context.

### LCD / LVGL Display Pipeline

- **LCD**: ST7789, 240x280, SPI1 (PB3=SCK, PB5=MOSI), with DMA on DMA2_Stream2
- **LVGL flush**: Uses DMA async mode — `disp_flush()` calls `LCD_Color_Fill()` which calls `HAL_SPI_Transmit_DMA()`. The DMA completion ISR calls `lv_disp_flush_ready()` via a registered callback (`LCD_Set_Flush_Complete_Callback`). **Do not** call `lv_disp_flush_ready()` directly in the flush function.
- **LVGL buffer**: Single buffer, 1/4 screen size (`LCD_W * LCD_H / 4`)
- **Y-offset**: The LCD has a 20px Y-offset (`OFFSET_Y` in `lcd.c`), applied in `LCD_Address_Set` calls
- **Backlight**: PWM on TIM3_CH3 (PB0), controlled via `LCD_Set_Light(5..100)`

### Touch Input

CST816 capacitive touch controller, connected via I2C (software I2C in `Drivers/BSP/IIC/`). Ported to LVGL as an input device in `Middlewares/LVGL/GUI/lvgl/porting/lv_port_indev_template.c`.

### BSP Drivers (`Drivers/BSP/`)

| Directory | Peripheral | Interface |
|-----------|-----------|-----------|
| AHT21 | Temperature & humidity sensor | I2C |
| BL24C02 | EEPROM (2Kbit) + DataSave | I2C |
| EM7028 | Heart rate + SpO2 sensor | I2C, uses precompiled `.lib` |
| IIC | Software I2C bit-bang driver | GPIO |
| KEY | Physical button input | GPIO |
| KT6328 | BLE module | UART |
| LCD | ST7789 display driver | SPI+DMA |
| LSM303DLH | E-compass (magnetometer+accel) | I2C |
| MPU6050 | IMU (accel+gyro) + DMP | I2C |
| OWDG | Watchdog | — |
| POWER | Battery ADC, charge detect, power enable | ADC, GPIO |
| SPL06_001 | Barometric pressure sensor | I2C |
| TOUCH | CST816 capacitive touch | I2C |

### Peripheral Pin Mapping

- **SPI1**: PB3 (SCK), PB5 (MOSI) — LCD data
- **LCD control**: PB7 (RES), PB8 (CS), PB9 (DC), PB0 (BLK/PWM)
- **Power**: PA3 (power enable), PA2 (charge detect, EXTI2)
- **ADC1_CH1**: Battery voltage
- **USART1**: BLE (KT6328), DMA RX/TX
- **I2C sensors**: Software I2C via `iic_hal.c` (check pin config there)

## Key Conventions

- CubeMX-generated files use `USER CODE BEGIN/END` comment blocks. Only edit between these markers in `Core/` files — CubeMX will overwrite anything outside them.
- BSP drivers are standalone per-peripheral. Each has its own `.c/.h` pair.
- The version is defined in `User/version.h` (`VERSION_MAJOR/MINOR/PATCH`).
- LVGL config is at `Middlewares/LVGL/GUI/lv_conf.h`.
