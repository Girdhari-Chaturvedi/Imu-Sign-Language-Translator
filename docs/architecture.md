# Architecture

Five MPU6050 sensors -> TCA9548A -> Arduino UNO.

Arduino then sends telemetry through USB and HC-05. USB can feed the Python desktop voice client. HC-05 can feed an Android Bluetooth serial application.

Sensor acquisition is kept separate from gesture recognition so a future ML classifier can replace the prototype threshold rules.
