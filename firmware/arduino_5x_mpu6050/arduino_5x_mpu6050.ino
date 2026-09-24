#include <Wire.h>
#include <SoftwareSerial.h>
#include <math.h>

#define TCA_ADDR 0x70
#define MPU_ADDR 0x68
#define SENSOR_COUNT 5

#define PWR_MGMT_1 0x6B
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B
#define ACCEL_XOUT_H 0x3B
#define WHO_AM_I 0x75

SoftwareSerial bluetooth(10, 11);

struct IMUData {
  float ax, ay, az;
  float gx, gy, gz;
  float pitch, roll;
  bool connected;
};

IMUData imu[SENSOR_COUNT];
float neutralPitch[SENSOR_COUNT];
float neutralRoll[SENSOR_COUNT];

const float ANGLE_THRESHOLD = 25.0;
const float MOTION_THRESHOLD = 80.0;
const float STRONG_MOTION = 250.0;

String currentGesture = "";
unsigned long gestureStart = 0;
unsigned long lastSent = 0;
unsigned long lastTelemetry = 0;

const unsigned long HOLD_TIME = 350;
const unsigned long COOLDOWN = 1800;
const unsigned long TELEMETRY_INTERVAL = 300;

bool tcaSelect(uint8_t channel) {
  if (channel > 7) return false;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  return Wire.endTransmission() == 0;
}

void tcaDisable() {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(0);
  Wire.endTransmission();
}

void writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0;
  Wire.requestFrom(MPU_ADDR, (uint8_t)1, true);
  return Wire.available() ? Wire.read() : 0;
}

bool initSensor(uint8_t channel) {
  if (!tcaSelect(channel)) return false;
  delay(5);

  uint8_t id = readReg(WHO_AM_I);
  if (id == 0) {
    tcaDisable();
    return false;
  }

  writeReg(PWR_MGMT_1, 0x00);
  delay(50);
  writeReg(ACCEL_CONFIG, 0x00); // +/-2g
  writeReg(GYRO_CONFIG, 0x00);  // +/-250 deg/s
  delay(20);

  bool ok = readReg(WHO_AM_I) != 0;
  tcaDisable();
  return ok;
}

bool readSensor(uint8_t channel, IMUData &s) {
  if (!tcaSelect(channel)) return false;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  if (Wire.endTransmission(false) != 0) {
    tcaDisable();
    return false;
  }

  Wire.requestFrom(MPU_ADDR, (uint8_t)14, true);
  if (Wire.available() < 14) {
    tcaDisable();
    return false;
  }

  int16_t ax = ((int16_t)Wire.read() << 8) | Wire.read();
  int16_t ay = ((int16_t)Wire.read() << 8) | Wire.read();
  int16_t az = ((int16_t)Wire.read() << 8) | Wire.read();

  Wire.read();
  Wire.read();

  int16_t gx = ((int16_t)Wire.read() << 8) | Wire.read();
  int16_t gy = ((int16_t)Wire.read() << 8) | Wire.read();
  int16_t gz = ((int16_t)Wire.read() << 8) | Wire.read();

  tcaDisable();

  s.ax = ax / 16384.0;
  s.ay = ay / 16384.0;
  s.az = az / 16384.0;

  s.gx = gx / 131.0;
  s.gy = gy / 131.0;
  s.gz = gz / 131.0;

  s.pitch = atan2(-s.ax, sqrt(s.ay * s.ay + s.az * s.az)) * 180.0 / PI;
  s.roll = atan2(s.ay, s.az) * 180.0 / PI;

  return true;
}

void calibrateAll() {
  Serial.println("CALIBRATION: keep all sensors still.");
  bluetooth.println("CALIBRATION");

  delay(2500);

  float ps[SENSOR_COUNT] = {0};
  float rs[SENSOR_COUNT] = {0};
  uint16_t n[SENSOR_COUNT] = {0};

  for (int sample = 0; sample < 100; sample++) {
    for (int i = 0; i < SENSOR_COUNT; i++) {
      if (imu[i].connected && readSensor(i, imu[i])) {
        ps[i] += imu[i].pitch;
        rs[i] += imu[i].roll;
        n[i]++;
      }
    }
    delay(10);
  }

  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (n[i]) {
      neutralPitch[i] = ps[i] / n[i];
      neutralRoll[i] = rs[i] / n[i];
    }
  }

  Serial.println("CALIBRATION COMPLETE");
  bluetooth.println("CALIBRATION_COMPLETE");
}

void combinedFeatures(float &p, float &r, float &gx, float &gy, float &gz) {
  p = r = gx = gy = gz = 0;
  int count = 0;

  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (!imu[i].connected) continue;

    p += imu[i].pitch - neutralPitch[i];
    r += imu[i].roll - neutralRoll[i];
    gx += imu[i].gx;
    gy += imu[i].gy;
    gz += imu[i].gz;
    count++;
  }

  if (count) {
    p /= count;
    r /= count;
    gx /= count;
    gy /= count;
    gz /= count;
  }
}

String detectGesture() {
  float p, r, gx, gy, gz;
  combinedFeatures(p, r, gx, gy, gz);

  float motion = fabs(gx) + fabs(gy) + fabs(gz);

  if (motion > STRONG_MOTION) return "EMERGENCY";

  if (p > 10 && fabs(gz) > 35) return "I NEED HELP";
  if (p < -10 && fabs(gz) > 35) return "PLEASE";
  if (r > 10 && fabs(gz) > 35) return "SORRY";
  if (r < -10 && fabs(gz) > 35) return "OK";

  if (fabs(gx) > MOTION_THRESHOLD) return gx > 0 ? "COME HERE" : "GO";
  if (fabs(gy) > MOTION_THRESHOLD) return gy > 0 ? "STOP" : "WAIT";
  if (fabs(gz) > MOTION_THRESHOLD) return gz > 0 ? "GOOD MORNING" : "GOOD NIGHT";

  if (p > ANGLE_THRESHOLD && r > ANGLE_THRESHOLD) return "HELLO";
  if (p > ANGLE_THRESHOLD && r < -ANGLE_THRESHOLD) return "WATER";
  if (p < -ANGLE_THRESHOLD && r > ANGLE_THRESHOLD) return "FOOD";
  if (p < -ANGLE_THRESHOLD && r < -ANGLE_THRESHOLD) return "BYE";

  if (p > ANGLE_THRESHOLD) return "YES";
  if (p < -ANGLE_THRESHOLD) return "HELP";
  if (r > ANGLE_THRESHOLD) return "NO";
  if (r < -ANGLE_THRESHOLD) return "THANK YOU";

  return "";
}

void sendGesture(const String &g) {
  Serial.print("SIGN:");
  Serial.println(g);
  bluetooth.print("SIGN:");
  bluetooth.println(g);
  lastSent = millis();
}

void sendTelemetry() {
  if (millis() - lastTelemetry < TELEMETRY_INTERVAL) return;
  lastTelemetry = millis();

  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (!imu[i].connected) continue;

    Serial.print("DATA,S"); Serial.print(i + 1);
    Serial.print(",AX="); Serial.print(imu[i].ax, 3);
    Serial.print(",AY="); Serial.print(imu[i].ay, 3);
    Serial.print(",AZ="); Serial.print(imu[i].az, 3);
    Serial.print(",GX="); Serial.print(imu[i].gx, 1);
    Serial.print(",GY="); Serial.print(imu[i].gy, 1);
    Serial.print(",GZ="); Serial.print(imu[i].gz, 1);
    Serial.print(",P="); Serial.print(imu[i].pitch, 1);
    Serial.print(",R="); Serial.println(imu[i].roll, 1);

    bluetooth.print("DATA,S"); bluetooth.print(i + 1);
    bluetooth.print(",AX="); bluetooth.print(imu[i].ax, 3);
    bluetooth.print(",AY="); bluetooth.print(imu[i].ay, 3);
    bluetooth.print(",AZ="); bluetooth.print(imu[i].az, 3);
    bluetooth.print(",GX="); bluetooth.print(imu[i].gx, 1);
    bluetooth.print(",GY="); bluetooth.print(imu[i].gy, 1);
    bluetooth.print(",GZ="); bluetooth.print(imu[i].gz, 1);
    bluetooth.print(",P="); bluetooth.print(imu[i].pitch, 1);
    bluetooth.print(",R="); bluetooth.println(imu[i].roll, 1);
  }
}

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  Wire.begin();
  Wire.setClock(400000L);
  delay(500);

  Serial.println("======================================");
  Serial.println("5x MPU6050 SIGN LANGUAGE SYSTEM");
  Serial.println("======================================");

  Wire.beginTransmission(TCA_ADDR);
  if (Wire.endTransmission() != 0) {
    Serial.println("ERROR: TCA9548A NOT FOUND");
    bluetooth.println("ERROR:TCA9548A");
    while (true) delay(1000);
  }

  Serial.println("TCA9548A OK");

  int found = 0;
  for (int i = 0; i < SENSOR_COUNT; i++) {
    imu[i].connected = initSensor(i);
    Serial.print("S"); Serial.print(i + 1);
    Serial.print(" MPU6050: ");
    Serial.println(imu[i].connected ? "OK" : "NOT FOUND");
    if (imu[i].connected) found++;
  }

  bluetooth.println("BT_READY");
  bluetooth.println("5_IMU_SYSTEM_STARTING");

  calibrateAll();

  Serial.print("Sensors online: ");
  Serial.println(found);
  Serial.println("SYSTEM READY");
  bluetooth.println("SYSTEM_READY");
}

void loop() {
  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (!imu[i].connected) continue;
    if (!readSensor(i, imu[i])) {
      Serial.print("WARNING:S");
      Serial.print(i + 1);
      Serial.println(" READ ERROR");
    }
  }

  sendTelemetry();

  String detected = detectGesture();

  if (detected.length()) {
    if (detected != currentGesture) {
      currentGesture = detected;
      gestureStart = millis();
    }

    if (millis() - gestureStart >= HOLD_TIME &&
        millis() - lastSent >= COOLDOWN) {
      sendGesture(detected);
    }
  } else {
    currentGesture = "";
  }

  delay(20);
}
