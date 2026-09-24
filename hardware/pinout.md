# Pinout

Arduino UNO -> TCA9548A:
- A4/SDA -> SDA
- A5/SCL -> SCL
- GND -> GND
- Suitable supply -> VCC/VIN according to board rating

TCA9548A -> MPU6050:
- Channel 0 -> Sensor 1
- Channel 1 -> Sensor 2
- Channel 2 -> Sensor 3
- Channel 3 -> Sensor 4
- Channel 4 -> Sensor 5

HC-05:
- D10 <- TXD
- D11 -> RXD through suitable divider/level shifting
- GND -> common GND
- VCC -> supply appropriate for the breakout
