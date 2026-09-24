# Contributing

Thank you for your interest in contributing to the IMU Sign Language Translator project.

## Project Areas
- Arduino and embedded firmware
- MPU6050 / TCA9548A sensor integration
- Bluetooth communication
- Python desktop applications
- Signal processing and gesture recognition
- Dataset collection and machine learning
- Documentation and diagrams

## Getting Started
1. Fork or clone the repository.
2. Create a focused feature branch.
3. Make small, testable changes.
4. Update documentation when behavior or wiring changes.
5. Verify the Arduino sketch and Python files before opening a pull request.

## Pull Requests
Please include:
- What changed
- Why it changed
- Hardware/software affected
- Testing performed
- Any known limitations

Keep experimental gesture thresholds clearly identified as experimental.

## Code Style
- Use clear names and short comments for non-obvious logic.
- Keep hardware pin mappings documented.
- Avoid committing secrets, API keys, private data, or personal credentials.

## Hardware Changes
If you change wiring, sensor count, I2C addresses, Bluetooth pins, or power requirements, update the relevant files under `hardware/`, `diagrams/`, and `docs/`.

## Recognition Changes
This project currently contains prototype threshold-based gesture logic. Contributions should not present experimental rules as validated sign-language recognition without supporting evaluation data.
