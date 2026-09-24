# Hardware

## Core hardware

- Arduino UNO
- 5 × MPU6050 / GY-521
- TCA9548A I²C multiplexer
- HC-05 Bluetooth module
- Breadboard and jumper wires

## Documents

- [Components](components.md)
- [Pinout](pinout.md)
- [Wiring](wiring.md)

## I²C design

All five MPU6050 modules can use the same typical address (0x68) because the TCA9548A isolates them on separate downstream channels.

Channel map:

`CH0 → MPU1`, `CH1 → MPU2`, `CH2 → MPU3`, `CH3 → MPU4`, `CH4 → MPU5`.
