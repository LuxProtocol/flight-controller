#include <Adafruit_BNO08x.h>
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include <stdarg.h>

// commands
#include "commands.h"

#define BNO08X_RESET -1
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// ---------------- Hardware Pins ----------------
// --- Motor Pins
#define FL_M 39
#define FR_M 38
#define RL_M 36
#define RR_M 37

// --- IMU I2C Pins
#define GYRO_SDA 9      //QWIIC - Blue
#define GYRO_SCL 10     //QWIIC - Yellow

// --- Status LED Pins
//#define R_LED = 5;
//#define G_LED = 6;
//#define B_LED = 7;
// ---------------- Hardware Pins ----------------

// ---------------- Debugging ----------------
#define ENABLE_ROLL_CORRECTION      1
#define ENABLE_PITCH_CORRECTION     1
#define ENABLE_YAW_CORRECTION       0
#define DEBUG_PRINT                 0
#define ENABLE_SERIAL_CMDS          1
#define ENABLE_MOTORS               1
// ---------------- Debugging ----------------

// ---------------- Timing ----------------
TickType_t lastWakeTime;
const TickType_t periodTicks = pdMS_TO_TICKS(10);
// ---------------- Timing ----------------

// ---------------- Motor output ----------------
struct MotorOutputs {
  int FL, FR, RL, RR;
};

// ---------------- PID gains ----------------
struct PIDGains { float kp, ki, kd; };

PIDGains rollGains  = {1.7, 0.15, 0.35};
PIDGains pitchGains = {1.7, 0.15, 0.35};
PIDGains yawGains   = {0.6, 0.0, 0.0};
#define ROLL_TRIM     0.0
#define PITCH_TRIM    0.0

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

// ---------------- Orientation state ----------------
float currentRoll = 0, currentPitch = 0, currentYaw = 0;
float gyroRollRate = 0, gyroPitchRate = 0, gyroYawRate = 0;
float yawHold = 0;
bool yawHoldSet = false;
int counter = 0;

// ---------------- Motor Control ----------------
#define PWM_FREQUENCY       250   // Hz
#define PWM_RESOLUTION      12    // bits  →  0 – 4095

const int BASE_THROTTLE = 1500;   // hover-ish baseline, tune for your build
#define MIN_PWM           1138
#define MAX_PWM           2512
#define OFF_PWM           256

unsigned long lastMicros = 0;

// ---------------- Threading ----------------
TaskHandle_t FlightThread = NULL;
TaskHandle_t CLIThread = NULL;
//TaskHandle_t taskFloat2Handle = NULL;



// ---------------- loggin stuff ----------------
// --- Print
constexpr size_t LOG_MSG_LEN   = 128;  // max chars per message
constexpr size_t LOG_QUEUE_LEN = 64;   // messages buffered (~8 KB total)
constexpr size_t CMD_LINE_LEN  = 64;

struct LogMsg { char text[LOG_MSG_LEN]; };

static QueueHandle_t logQueue;
static uint32_t droppedLogs = 0;

// ---------------- ^ Variable Settings ^ ----------------


// ---------------- BNO085 setup ----------------
void setupIMU() {
  Wire.begin(GYRO_SDA, GYRO_SCL);
  bno08x.begin_I2C(0x4A, &Wire);
  bno08x.enableReport(SH2_ROTATION_VECTOR, 5000);   // 5ms = 200Hz
  bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, 5000);
  lastMicros = micros();
}
// ---------------- BNO085 setup ----------------





// ---------------- ESC setup ----------------
void setupESC() {
  ledcAttach(FL_M, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(FR_M, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(RL_M, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(RR_M, PWM_FREQUENCY, PWM_RESOLUTION);

  ledcWrite(FL_M, OFF_PWM);
  ledcWrite(FR_M, OFF_PWM);
  ledcWrite(RL_M, OFF_PWM);
  ledcWrite(RR_M, OFF_PWM);

  vTaskDelay(pdMS_TO_TICKS(250));

  Serial.println("Arming ESCs — keep propellers clear!");
  ledcWrite(FL_M, MIN_PWM);
  ledcWrite(FR_M, MIN_PWM);
  ledcWrite(RL_M, MIN_PWM);
  ledcWrite(RR_M, MIN_PWM);

  vTaskDelay(pdMS_TO_TICKS(2500));
  Serial.println("ESCs armed!");
}
// ---------------- ESC setup ----------------





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
}
// ---------------- Quaternion -> Euler ----------------





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
// ---------------- Pull latest sensor data ----------------

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

  return constrain(P + I + D, s.outputMin, s.outputMax);
}
// ---------------- Core PID (derivative on measured rate) ----------------



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

  float rollOut  = computePID(errorRoll,  gyroRollRate,  dt, rollGains,  rollState);
  float pitchOut = computePID(errorPitch, gyroPitchRate, dt, pitchGains, pitchState);
  float yawOut   = 0; //ENABLE_YAW_CORRECTION ? computePID(errorYaw,   gyroYawRate,   dt, yawGains,   yawState) : 0;

  // X-config motor mixing
  // blue & red: front
  // green & red: right
  float fl = throttle - pitchOut + rollOut + yawOut; // red/M1
  float fr = throttle + pitchOut + rollOut - yawOut; // green/M2
  float rl = throttle + pitchOut - rollOut + yawOut; // blue/M3
  float rr = throttle - pitchOut - rollOut - yawOut; // yellow/M4

  MotorOutputs out;
  out.FL = constrain((int)fl, MIN_PWM, MAX_PWM);
  out.FR = constrain((int)fr, MIN_PWM, MAX_PWM);
  out.RL = constrain((int)rl, MIN_PWM, MAX_PWM);
  out.RR = constrain((int)rr, MIN_PWM, MAX_PWM);
  return out;
}
// ---------------- Main entry point ----------------



// ---------------- Thread Health Check ----------------
void checkTaskAlive(TaskHandle_t handle, const char* name) {
  if (handle == NULL) {
    Serial.printf("[%s] Handle is NULL — task was never created or was deleted\n", name);
    return;
  }

  eTaskState state = eTaskGetState(handle);
  const char* stateStr;
  switch (state) {
    case eRunning:   stateStr = "Running"; break;
    case eReady:     stateStr = "Ready"; break;
    case eBlocked:   stateStr = "Blocked"; break;
    case eSuspended: stateStr = "Suspended"; break;
    case eDeleted:   stateStr = "Deleted"; break;
    default:         stateStr = "Unknown"; break;
  }
  Serial.printf("[%s] State: %s\n", name, stateStr);
}
// ---------------- Thread Health Check ----------------



// ---------------- CommandLineInterface Thread ----------------
void logf(const char *fmt, ...) {
  LogMsg msg;
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg.text, sizeof(msg.text), fmt, args);
  va_end(args);

  // Timeout of 0 = never block the caller; drop the message if the queue is full
  if (xQueueSend(logQueue, &msg, 0) != pdTRUE) {
    __atomic_add_fetch(&droppedLogs, 1, __ATOMIC_RELAXED);
  }
}

#ifdef ENABLE_SERIAL_CMDS
void commandLineInterface(void *pvParameters) {
  LogMsg msg;
  char   line[CMD_LINE_LEN];
  size_t len = 0;
  bool   overflow = false;

  for (;;) {
    // --- Print to serial ---
    if (xQueueReceive(logQueue, &msg, pdMS_TO_TICKS(10)) == pdTRUE) {
      Serial.println(msg.text);
      while (xQueueReceive(logQueue, &msg, 0) == pdTRUE) {
        Serial.println(msg.text);
      }
    }

    // --- Count messages not sent ---
    uint32_t dropped = __atomic_exchange_n(&droppedLogs, 0, __ATOMIC_RELAXED);
    if (dropped) Serial.printf("[logger] dropped %u messages\n", dropped);

    // --- Handle incoming serial data ---
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\r' || c == '\n') {
        if (overflow) {
          Serial.print("[error] line too long, discarded\n");
        } else if (len > 0) {
          line[len] = '\0';
          dispatchCommand(line);
        }
        len = 0;
        overflow = false;
      } else if (c == 0x08 || c == 0x7F) {          // backspace / delete
        if (len > 0) len--;
      } else if (isprint((unsigned char)c)) {
        if (len < CMD_LINE_LEN - 1) line[len++] = c;
        else overflow = true;
      }
    }
  }
}
#endif
// ---------------- CommandLineInterface Thread ----------------





// ---------------- FlightController Thread ----------------
void flightController(void *pvParameters) {
  MotorOutputs m;

  float desiredX = 0.0; // lateral
  float desiredY = 0.0; // forward/back
  float desiredZ = 0.0; // vertical

  for(;;){
  lastWakeTime = xTaskGetTickCount();
  // Replace these with real RC/joystick input, normalized to [-1, 1]

  m = computeMotorOutputs(desiredX, desiredY, desiredZ);

  // Write PWM to ESCs
  #ifdef ENABLE_MOTORS
    ledcWrite(FL_M, m.FL);
    ledcWrite(FR_M, m.FR);
    ledcWrite(RL_M, m.RL);
    ledcWrite(RR_M, m.RR);
  #endif

  #ifdef DEBUG_PRINT
    if(counter>10){
    counter=0;
    // Display IMU Readings
    logf("Orientation: %.4f, %.4f, %.4f", currentYaw, currentPitch, currentRoll);

    // Display Motor Vectors
    logf("Motor: %d, %d, %d, %d", m.FL, m.FR, m.RL, m.RR);
    }
    counter++;
  #endif

  vTaskDelayUntil(&lastWakeTime, periodTicks); // 200Hz
  }
}
// ---------------- FlightController Thread ----------------





// ---------------- System Management ----------------
void setup() {
  Serial.begin(115200);
  setupIMU();
  setupESC();

  // Serial Thread
  #ifdef ENABLE_SERIAL_CMDS
    logQueue = xQueueCreate(LOG_QUEUE_LEN, sizeof(LogMsg));
    xTaskCreatePinnedToCore(
      commandLineInterface,         // task function
      "CommandLineInterface",       // name
      4096,                         // stack size (bytes) — increase if stack overflow occurs
      NULL,                         // parameter
      1,                            // priority (higher = more urgent; was 1)
      &CLIThread,                   // handle
      0                             // core
    );
  #endif

  // SMT Thread

  // Flight Thread
  xTaskCreatePinnedToCore(
    flightController,               // task function
    "FlightCtrl",                   // name
    8192,                           // stack size (bytes) — increase if stack overflow occurs
    NULL,                           // parameter
    5,                              // priority (higher = more urgent; was 1)
    &FlightThread,                  // handle
    1                               // core
  );


}

void loop() {
  vTaskDelay(2500); // 2.5 Seconds
  //checkTaskAlive(CLIThread, " [CLI] ");
  //checkTaskAlive(FlightThread, " [Flight] ");

  // --- Maybe add LED stuff here? ---
}
// ---------------- System Management ----------------
