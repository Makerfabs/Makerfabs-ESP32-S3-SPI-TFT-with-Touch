# Makerfabs ESP32-S3 SPI TFT with Touch

```c++
/*
Version:		V1.0
Author:			Vincent
Create Date:	2022/7/13
Note:
	
*/
```
![](md_pic/main.jpg)


[toc]

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)

# Makerfabs ESP32-S3 SPI TFT with Touch
## Intruduce

Product Link ：[]()

Wiki Link : []()

The 3.5" 320x480 TFT LCD driver is ILI9488, it uses SPI line for communication with ESP32-S3. You can freely use some of GPIO, to connect the ESP32-S3 display with sensors/ actuators, suitable for IoT applications.

![back](md_pic/back.jpg)

## Feature

- Wi-Fi (2.4 GHz band)
- Bluetooth Low Energy
- Dual high performance Xtensa® 32-bit LX7 CPU cores
- Ultra Low Power co-processor running either RISC-V or FSM core
- Multiple peripherals
- Built-in security hardware
- USB OTG interface
- USB Serial/JTAG Controller
- Arduino Compatible: You can play it with Arduino IDE
- LCD 3.5 inch Amorphous-TFT-LCD (Thin Film Transistor Liquid Crystal Display) for mobile-phone or handy electrical equipment
- LCD Driver: ILI9488(SPI)
- LCD Resolution: 320*480
- FT6236 Series ICs are single-chip capacitive touch panel controller IC with a built-in 16 bit enhanced Micro-controller unit (MCU)
- Power supply: 5V, Type-C USB
- Micro SD card slot on the board
- Dual USB Type-C: one for native USB and one for USB-to-UART
- Board size: 66mm * 85mm



# Code Explain

## Complier Option

- Install board : ESP32 .
- Install library : LovyanGFX library. 
- Use type-c use cable connect USB-TTL to PC.
- Upload codes, select "ESP32-S3 DEV Module" and "UART0"

![](md_pic/complier.jpg)

## Firmware

### S3_9488

A simple test procedure. Touch the black area in the middle of the screen, and the screen starts showing solid colors in turn. Finally, an SD card image is displayed.
