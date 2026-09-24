# Wiring

Five MPU6050 modules can share the same 0x68 address because each is isolated behind a separate TCA9548A channel.

Arduino A4/SDA -> TCA SDA
Arduino A5/SCL -> TCA SCL
Arduino GND -> common ground

TCA CH0 -> MPU6050 #1
TCA CH1 -> MPU6050 #2
TCA CH2 -> MPU6050 #3
TCA CH3 -> MPU6050 #4
TCA CH4 -> MPU6050 #5

For each downstream sensor connect SDA, SCL, VCC and GND to its channel/bus.

HC-05 TXD -> Arduino D10
Arduino D11 -> HC-05 RXD through the appropriate divider/level shifting
HC-05 GND -> Arduino GND

Keep I2C wiring short and use a stable power supply.
