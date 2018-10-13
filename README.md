# stm32f407-vga
VGA demo on Black F407VET6 (using STM32F407VE microcontroller) 

## What it can do
It displays "Hello, world!" on a VGA connected to a Black F407VET6 board.

## Installation
This is the what you need to try this project:

| Hardware      |    Qty|
| ------------- | -----:|
| [Black F407VET6](http://wiki.stm32duino.com/index.php?title=STM32F407) board | 1
| Resistors 470 Ohm | 3
| Resistors 680 Ohm | 3
| Breadboard | 1
| VGA connector (I used one from the old video card) | 1
| Jumper wires | 15
| ST-Link v2 or clone | 1

Software: Install free IDE [System Workbench for STM32](https://www.st.com/en/development-tools/sw4stm32.html/). I am using Windows 10, however STMicroelectronics claims that it also supports Linux and Mac.

How to connect wires:

| PIN  | Description | Connect To | Output |
| ---  | ----------- | ---------- | ------ |
| PE8  | Red 1   | Resistor 470 Ohm | VGA red (1)
| PE9  | Red 2   | Resistor 680 Ohm | VGA red (1)
| PE10 | Green 1 | Resistor 470 Ohm | VGA green (2)
| PE11 | Green 2 | Resistor 680 Ohm | VGA green (2)
| PE12 | Blue 1  | Resistor 470 Ohm | VGA blue (3)
| PE13 | Blue 2  | Resistor 680 Ohm | VGA blue (3)
| PD15 | HSync   | | VGA HSync (13)
| PD14 | VSync   | | VGA VSync (14)
| G    | Ground  | | VGA Ground (5,6,7,8,10)

## Project Description
This project uses [Cliff L. Biffle's m4vgalib](https://github.com/cbiffle/m4vgalib) to display "Hello, world".
