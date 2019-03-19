
# Alternative pinout and read-only file system on high 512K of 1M flash

## Picture


## Pinout

| PIN | Description | Connect To | Output |
| --- | ----------- | ---------- | ------ |
| PD0  | Red 1   | Resistor 470 Ohm | VGA red (1)
| PD1  | Red 2   | Resistor 680 Ohm | VGA red (1)
| PD2 | Green 1 | Resistor 470 Ohm | VGA green (2)
| PD3 | Green 2 | Resistor 680 Ohm | VGA green (2)
| PD4 | Blue 1  | Resistor 470 Ohm | VGA blue (3)
| PD5 | Blue 2  | Resistor 680 Ohm | VGA blue (3)
| PB6 | HSync   | | VGA HSync (13)
| PB7 | VSync   | | VGA VSync (14)
| PB4 | CLK | Resistor 2K2 to keyboard CLK and resistor 3K3 to GND
| PB5 | DATA | Resistor 2K2 to keyboard DATA and resistor 3K3 to GND
| G | Ground | | VGA Ground (5,6,7,8,10), '-' of passive speaker 
