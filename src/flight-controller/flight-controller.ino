#include <Adafruit_BNO08x.h>
#include <Arduino.h>

#define BNO08X_RESET -1
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// ---------------- Motor output ----------------
struct MotorOutputs {
  int FL, FR, RL, RR;
};

// ---------------- PID gains ----------------
struct PIDGains { float kp, ki, kd; };

// Start LOW. Bring these up gradually while testing (see tuning notes below).
PIDGains rollGains  = {5.0, 0.3, 0.3};
PIDGains pitchGains = {5.0, 0.3, 0.3};
PIDGains yawGains   = {0.6, 0.0, 0.0};

// ---------------- PID persistent state ----------------
// IMPORTANT: yaw gets a much smaller output ceiling than roll/pitch.
// Yaw is a rotational nudge; roll/pitch are what actually keep you level
// and need the majority of the available PWM headroom.
struct PIDState {
  float integral = 0;
  float integralMax;
  float outputMax;
  float outputMin;
};

PIDState rollState  = {0, 150, 250, -250};
PIDState pitchState = {0, 150, 250, -250};
PIDState yawState   = {0, 50,  120, -120};

// Debug switches - zero these out to isolate one axis at a time while tuning
#define ENABLE_ROLL_CORRECTION  1
#define ENABLE_PITCH_CORRECTION 1
#define ENABLE_YAW_CORRECTION   1
#define DEBUG_PRINT             1

// ---------------- Orientation state ----------------
float currentRoll = 0, currentPitch = 0, currentYaw = 0;
float gyroRollRate = 0, gyroPitchRate = 0, gyroYawRate = 0;
float yawHold = 0;
bool yawHoldSet = false;

const int BASE_THROTTLE = 1300;   // hover-ish baseline, tune for your build
const int MIN_PWM = 1000;
const int MAX_PWM = 2000;
const int GYRO_SDA = 9;
const int GYRO_SCL = 10;

unsigned long lastMicros = 0;

// ---------------- BNO085 setup ----------------
void setupIMU() {

  Wire.begin(GYRO_SDA, GYRO_SCL);
  bno08x.begin_I2C(0x4A, &Wire);
  bno08x.enableReport(SH2_ROTATION_VECTOR, 5000);   // 5ms = 200Hz
  bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, 5000);
  lastMicros = micros();
}

// ---------------- Quaternion -> Euler ----------------
// ---------------- Quaternion -> Euler ----------------
void quatToEuler(float qr, float qi, float qj, float qk,
                  float &roll, float &pitch, float &yaw) {

  double sqr,sqi,sqj,sqk;

  sqr = qr * qr;
  sqi = qi * qi;
  sqj = qj * qj;
  sqk = qk * qk;

  roll  = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr)) * RAD_TO_DEG;
  pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr)) * RAD_TO_DEG;
  yaw   = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr)) * RAD_TO_DEG;

  Serial.print("Orientation: ");
  Serial.print(yaw, 4);
  Serial.print(", ");
  Serial.print(pitch, 4);
  Serial.print(", ");
  Serial.print(roll, 4);
  Serial.println("");
}

// ---------------- Pull latest sensor data ----------------
// Returns true if orientation was updated this call.
bool readIMU() {
  bool updated = false;
  while (bno08x.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) {
      quatToEuler(sensorValue.un.rotationVector.real,
                  sensorValue.un.rotationVector.i,
                  sensorValue.un.rotationVector.j,
                  sensorValue.un.rotationVector.k,
                  currentRoll, currentPitch, currentYaw);
      updated = true;
      if (!yawHoldSet) { yawHold = currentYaw; yawHoldSet = true; }
    }
    if (sensorValue.sensorId == SH2_GYROSCOPE_CALIBRATED) {
      // rad/s -> deg/s
      gyroRollRate  = sensorValue.un.gyroscope.x * RAD_TO_DEG;
      gyroPitchRate = sensorValue.un.gyroscope.y * RAD_TO_DEG;
      gyroYawRate   = sensorValue.un.gyroscope.z * RAD_TO_DEG;
    }
  }
  return updated;
}

void resetYawHold() {
  yawHold = currentYaw;
}

// ---------------- Core PID (derivative on measured rate) ----------------
float computePID(float error, float measuredRate, float dt,
                  PIDGains &g, PIDState &s) {
  float P = g.kp * error;

  s.integral += error * dt;
  s.integral = constrain(s.integral, -s.integralMax, s.integralMax);
  float I = g.ki * s.integral;

  float D = -g.kd * measuredRate;

  float output = P + I + D;
  output = constrain(output, s.outputMin, s.outputMax);
  return output;
}

// ---------------- Main entry point ----------------
// desiredX/Y/Z in range roughly [-1, 1] (e.g. from joystick/RC, normalized)
// maxTiltDeg caps how aggressively x/y get turned into lean angle
MotorOutputs computeMotorOutputs(float desiredX, float desiredY, float desiredZ,
                                  float maxTiltDeg = 25.0,
                                  float maxClimbPWM = 300.0) {
  readIMU();

  unsigned long now = micros();
  float dt = (now - lastMicros) / 1000000.0;
  if (dt <= 0) dt = 0.001;
  lastMicros = now;

  // Map desired movement -> target angles / throttle
  float targetRoll  = constrain(desiredX, -1.0, 1.0) * maxTiltDeg;
  float targetPitch = constrain(desiredY, -1.0, 1.0) * maxTiltDeg;
  float throttle     = BASE_THROTTLE + constrain(desiredZ, -1.0, 1.0) * maxClimbPWM;

  float errorRoll  = targetRoll  - currentRoll;
  float errorPitch = targetPitch - currentPitch;
  float errorYaw   = yawHold    - currentYaw;
  if (errorYaw > 180)  errorYaw -= 360;
  if (errorYaw < -180) errorYaw += 360;

  float rollOut  = ENABLE_ROLL_CORRECTION  ? computePID(errorRoll,  gyroRollRate,  dt, rollGains,  rollState)  : 0;
  float pitchOut = ENABLE_PITCH_CORRECTION ? computePID(errorPitch, gyroPitchRate, dt, pitchGains, pitchState) : 0;
  float yawOut   = ENABLE_YAW_CORRECTION   ? computePID(errorYaw,   gyroYawRate,   dt, yawGains,   yawState)   : 0;

  // X-config motor mixing
  // blue & red: front
  // green & red: right
  float fl = throttle - pitchOut + rollOut + yawOut; // red/M1 throttle - pitchOut + rollOut + yawOut;
  float fr = throttle + pitchOut + rollOut - yawOut; // green/M2 throttle + pitchOut + rollOut - yawOut;
  float rl = throttle + pitchOut - rollOut + yawOut; // blue/M3 throttle - pitchOut - rollOut - yawOut;
  float rr = throttle - pitchOut - rollOut - yawOut; // yellow/M4 throttle + pitchOut - rollOut + yawOut;

  MotorOutputs out;
  out.FL = constrain((int)fl, MIN_PWM, MAX_PWM);
  out.FR = constrain((int)fr, MIN_PWM, MAX_PWM);
  out.RL = constrain((int)rl, MIN_PWM, MAX_PWM);
  out.RR = constrain((int)rr, MIN_PWM, MAX_PWM);
  return out;
}

// ---------------- Example usage ----------------
void setup() {
  Serial.begin(115200);
  setupIMU();
}

void loop() {
  // Replace these with real RC/joystick input, normalized to [-1, 1]
  float desiredX = 0.0; // lateral
  float desiredY = 0.0; // forward/back
  float desiredZ = 0.0; // vertical

  MotorOutputs m = computeMotorOutputs(desiredX, desiredY, desiredZ);

  // writeToESC(FL_PIN, m.FL); etc.
  Serial.print("Motor: ");
  Serial.print(m.FL); Serial.print(", ");
  Serial.print(m.FR); Serial.print(", ");
  Serial.print(m.RL); Serial.print(", ");
  Serial.println(m.RR);



  delay(5); // ~200Hz loop
}