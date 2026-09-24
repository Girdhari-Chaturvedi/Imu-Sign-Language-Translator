# Signal Flow

Start -> I2C init -> TCA init -> initialize five MPU6050 -> calibration -> select channel -> read sensor -> calculate features -> gesture rules -> send DATA/SIGN -> repeat.
