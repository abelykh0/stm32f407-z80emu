# stm32f407-z80emu
Spectrum ZX 48K emulator (using STM32F407 microcontroller)

## What it can do
It emulates Spectrum ZX 48K and can also display some debugging information

TODO: video

## Introduction
This project uses 2 great libraries:
* To display video using VGA: https://github.com/cbiffle/m4vgalib (which requires https://github.com/cbiffle/etl)
* Z80 emulator: https://github.com/anotherlin/z80emu

## Installation
If you want to try my project, this is the only part that you need.

| Hardware      |    Qty|
| ------------- | -----:|
| STM32F407 board | 1
| VGA connector | 1
| PS/2 Keyboard | 1
| Resistors 470 Ohm | 3
| Resistors 680 Ohm | 3
| Resistors 2.2 KOhm | 2
| Resistors 3.3 KOhm | 2
| Breadboard | 1
| Jumper wires | 18
| ST-Link v2 or clone | 1

Software (under Windows): Install [Visual Studio Code](https://code.visualstudio.com/), then [PlatformIO](http://docs.platformio.org/en/latest/ide/vscode.html) plug-in.

How to connect wires:

| PIN | Description | Connect To | Output |
| --- | ----------- | ---------- | ------ |
| PA0 | Red 1 | Resistor 470 Ohm | VGA red (1)
| PA1 | Red 2 | Resistor 680 Ohm | VGA red (1)
| PA2 | Green 1 | Resistor 470 Ohm | VGA green (2)
| PA3 | Green 2 | Resistor 680 Ohm | VGA green (2)
| PA4 | Blue 1 | Resistor 470 Ohm | VGA blue (3)
| PA5 | Blue 2 | Resistor 680 Ohm | VGA blue (3)
| PB6 | HSync | | VGA HSync (13)
| PB7 | VSync | | VGA VSync (14)
| PB14 | CLK | Resistor 2K2 to keyboard CLK and resistor 3K3 to GND
| PB15 | DATA | Resistor 2K2 to keyboard DATA and resistor 3K3 to GND
| G | Ground | | VGA Ground (5,6,7,8,10)

## Plans for the future
* Save / load snapshot to / from flash
* The speed is faster than it is supposed to be
* Sound (will probably require correct speed to work)
* Some games don't work (not sure if it is an issue with the emulator or my code)
* Upload snapshots using USB connection (ZModem if possible or XModem)
