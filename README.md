# IMU Sign Language Translator

> Wearable motion-based sign/gesture translation prototype using Arduino UNO, 5× MPU6050, TCA9548A and HC-05 Bluetooth.

[![Arduino](https://img.shields.io/badge/Arduino-UNO-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/) [![Python](https://img.shields.io/badge/Python-3.x-3776AB?logo=python&logoColor=white)](https://www.python.org/) [![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

## Overview

This project explores sign/gesture translation using multiple inertial measurement units (IMUs). Five MPU6050 sensors provide acceleration and gyroscope measurements. A TCA9548A I²C multiplexer allows sensors with the same typical I²C address to share the bus.

The Arduino UNO processes motion data, calculates pitch and roll, applies prototype gesture rules, and sends structured telemetry through USB serial and HC-05 Bluetooth. A Windows Python application can read the USB stream and convert detected SIGN messages into speech.

## System Architecture

```text
MPU6050 × 5
    ↓ I²C CH0–CH4
TCA9548A
    ↓
Arduino UNO
   ├── USB → Windows Python + TTS
   └── HC-05 → Bluetooth device
```

## Hardware

| Component | Qty | Purpose |
|---|---:|---|
| Arduino UNO | 1 | Main controller |
| MPU6050 / GY-521 | 5 | Motion sensing |
| TCA9548A | 1 | I²C multiplexing |
| HC-05 | 1 | Bluetooth serial communication |
| Breadboard + jumper wires | As required | Prototyping |

### Arduino UNO Pinout

| Arduino | Connection |
|---|---|
| A4 / SDA | TCA9548A SDA |
| A5 / SCL | TCA9548A SCL |
| GND | Common GND |
| D10 | HC-05 TXD |
| D11 | HC-05 RXD through appropriate level shifting/divider |

> Verify the voltage requirements of the exact breakout boards being used. Protect the HC-05 RX input as required by its breakout.

### TCA9548A Channel Map

| Sensor | Channel | Typical Address |
|---|---:|---:|
| MPU6050 #1 | CH0 | 0x68 |
| MPU6050 #2 | CH1 | 0x68 |
| MPU6050 #3 | CH2 | 0x68 |
| MPU6050 #4 | CH3 | 0x68 |
| MPU6050 #5 | CH4 | 0x68 |

Typical TCA9548A address: **0x70**.

## Firmware

Main firmware: `firmware/arduino_5x_mpu6050/arduino_5x_mpu6050.ino`

The firmware initializes the multiplexer, checks sensors, reads acceleration/gyro data, calculates pitch/roll, calibrates startup orientation, combines motion features, applies prototype gesture rules, and sends DATA/SIGN messages.

## Windows Desktop Voice Client

Requirements: Python 3.x, pyserial, pyttsx3 and Tkinter.

Install:

```cmd
python -m pip install -r software\desktop_usb_voice\requirements.txt
```

Run:

```cmd
python software\desktop_usb_voice\sign_language_usb_voice.py
```

> Close Arduino Serial Monitor before the Python application opens the same COM port.

## Communication Format

Telemetry:

```text
DATA,S1,AX=0.120,AY=-0.940,AZ=0.310,GX=2.0,GY=-1.0,GZ=0.5,P=1.2,R=-70.3
```

Gesture:

```text
SIGN:HELLO
```

## Documentation

- [Hardware components](hardware/components.md)
- [Pinout](hardware/pinout.md)
- [Wiring](hardware/wiring.md)
- [System architecture](docs/architecture.md)
- [Data format](docs/data-format.md)
- [Gesture logic](docs/gesture-logic.md)
- [Setup](docs/setup.md)

## Current Prototype

The firmware contains experimental mappings for words and phrases including HELLO, WATER, FOOD, BYE, YES, NO, HELP, THANK YOU, STOP, WAIT, GO, COME HERE, PLEASE, SORRY, OK and EMERGENCY.

## Limitations

The current recognizer uses threshold-based experimental rules. It is not a complete or scientifically validated sign-language recognition system. Robust recognition requires consistent sensor placement, calibration, labeled time-series data, orientation normalization, model training, confidence scoring and multi-user evaluation.

## Roadmap

- [ ] Finalize wearable five-IMU mounting
- [ ] Improve calibration and orientation normalization
- [ ] Build a labeled sensor dataset
- [ ] Add data logging tools
- [ ] Train an ML/sequence classifier
- [ ] Add confidence scoring
- [ ] Build Android Bluetooth + TTS client
- [ ] Create a 3D-printed wearable sensor mount
- [ ] Evaluate with multiple users

## About

This repository is part of an embedded-AI project series focused on assistive technology, IoT, sensors, robotics and practical engineering prototypes.

## License

MIT — see [LICENSE](LICENSE).