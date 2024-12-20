# stm32f407-z80emu
Spectrum ZX 48K emulator (using STM32F407 microcontroller)

![Breadboard](https://raw.githubusercontent.com/abelykh0/stm32f407-z80emu/master/doc/stm32f407-z80emu.jpg)

## What it can do
* Emulate Spectrum ZX 48K
* Load snapshot in .Z80 format from SD card
* Save snapshot in .Z80 format to SD card
* Output some sounds to USB MIDI

<a href="http://www.youtube.com/watch?feature=player_embedded&v=PQxoNzHJnkY
" target="_blank"><img src="http://img.youtube.com/vi/PQxoNzHJnkY/0.jpg" 
alt="ZX Spectrum Emulator on STM32F407" width="480" height="360" border="10" /></a>

Video

## Installation
If you want to try my project, this is the only part that you need.

| Hardware      |    Qty|
| ------------- | -----:|
| [Black F407VET6](https://stm32-base.org/boards/STM32F407VET6-STM32-F4VE-V2.0) board | 1
| VGA connector | 1
| PS/2 Keyboard | 1
| Power supply 5V DC for keyboard | 1
| Resistors 470 Ohm | 3
| Resistors 680 Ohm | 3
| Resistors 2.2 KOhm | 2
| Resistors 3.3 KOhm | 2
| Breadboard | 1
| Jumper wires | 18
| ST-Link v2 or clone | 1

Software: Install free IDE [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html/). I am using Windows 10, however STMicroelectronics claims that it also supports Linux and Mac.

How to connect wires:

| PIN | Description | Connect To | Output |
| --- | ----------- | ---------- | ------ |
| PE8  | Red 1   | Resistor 470 Ohm | VGA red (1)
| PE9  | Red 2   | Resistor 680 Ohm | VGA red (1)
| PE10 | Green 1 | Resistor 470 Ohm | VGA green (2)
| PE11 | Green 2 | Resistor 680 Ohm | VGA green (2)
| PE12 | Blue 1  | Resistor 470 Ohm | VGA blue (3)
| PE13 | Blue 2  | Resistor 680 Ohm | VGA blue (3)
| PD15 | HSync   | | VGA HSync (13)
| PD14 | VSync   | | VGA VSync (14)
| PB14 | CLK | Resistor 2K2 to keyboard CLK and resistor 3K3 to GND
| PB13 | DATA | Resistor 2K2 to keyboard DATA and resistor 3K3 to GND
| G | Ground | | VGA Ground (5,6,7,8,10), '-' of passive speaker 

## Third party software
This project uses several libraries (in addition to HAL drivers from STMicroelectronics):
* To display video using VGA: https://github.com/cbiffle/m4vgalib (which requires https://github.com/cbiffle/etl)
* Z80 emulator: https://github.com/anotherlin/z80emu
* FATFS for SD card: http://elm-chan.org/fsw/ff/00index_e.html

## Plans for the future / issues
* Interference when using SD card or playing sound
* Flickering in some games
* The speed is 12% faster than it is supposed to be
