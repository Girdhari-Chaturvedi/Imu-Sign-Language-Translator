# IMU Sign Language Translator

A wearable motion-based sign/gesture translator using **Arduino UNO + 5× MPU6050 + TCA9548A I²C multiplexer + HC-05 Bluetooth**.

## Features

- Five independent MPU6050 motion sensors
- TCA9548A I²C multiplexing
- Accelerometer + gyroscope telemetry
- Pitch/roll calculation
- Prototype gesture vocabulary
- USB serial output
- HC-05 Bluetooth output
- Optional Windows Python text-to-speech client
- Structured data format suitable for future ML dataset collection

## Architecture

```
MPU6050 ×5
    ↓
TCA9548A
    ↓
Arduino UNO
    ├── USB → PC / Python TTS
    └── HC-05 → Android / Bluetooth terminal
```

## TCA9548A channel map

| Sensor | TCA channel | MPU6050 address |
|---|---:|---:|
| MPU6050 #1 | 0 | 0x68 |
| MPU6050 #2 | 1 | 0x68 |
| MPU6050 #3 | 2 | 0x68 |
| MPU6050 #4 | 3 | 0x68 |
| MPU6050 #5 | 4 | 0x68 |

Typical TCA9548A address: **0x70**.

## Arduino UNO wiring

| Arduino UNO | Device |
|---|---|
| A4 / SDA | TCA9548A SDA |
| A5 / SCL | TCA9548A SCL |
| GND | Common ground |
| D10 | HC-05 TXD |
| D11 | HC-05 RXD through appropriate level shifting/divider |

Each MPU6050 connects to one TCA9548A downstream channel.

See [hardware/wiring.md](hardware/wiring.md) and [hardware/pinout.md](hardware/pinout.md).

## Firmware

Main firmware:

`firmware/arduino_5x_mpu6050/arduino_5x_mpu6050.ino`

It initializes the TCA9548A, checks all five sensors, reads acceleration/gyro data, calculates pitch/roll, prints telemetry, and sends `SIGN:` messages for prototype gesture rules.

## Communication format

Telemetry:

```text
DATA,S1,AX=0.120,AY=-0.940,AZ=0.310,GX=2.0,GY=-1.0,GZ=0.5,P=1.2,R=-70.3
```

Gesture:

```text
SIGN:HELLO
```

Android/desktop clients should parse `SIGN:` lines when converting recognized text to speech.

## Desktop voice client

```bash
python -m pip install -r software/desktop_usb_voice/requirements.txt
python software/desktop_usb_voice/sign_language_usb_voice.py
```

Close Arduino Serial Monitor before Python opens the same COM port.

## Prototype limitation

The included gesture labels are **threshold-based experimental mappings**, not complete recognition of every sign-language word. Five IMUs provide rich motion information, but robust recognition should use consistent sensor placement, calibration, labeled time-series data, and a validated classifier.

## Repository structure

```text
firmware/
hardware/
docs/
diagrams/
software/
README.md
LICENSE
.gitignore
```

## Roadmap

- [ ] Calibrated five-sensor mounting
- [ ] Gesture data logger
- [ ] Labeled dataset
- [ ] ML classifier and confidence score
- [ ] Android Bluetooth + TTS application
- [ ] Wearable/3D-printed sensor mount
- [ ] Evaluation on multiple users

## License

MIT — see [LICENSE](LICENSE).
