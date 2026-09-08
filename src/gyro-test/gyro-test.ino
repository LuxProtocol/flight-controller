// Basic demo for readings from Adafruit BNO08x
#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <ESP32Servo.h>

const bool debug = true;

// --- hardware pins ---
const int mot1_pin = 5;
const int mot2_pin = 6;
const int mot3_pin = 7;
const int mot4_pin = 4;

const int GYRO_SDA = 9;
const int GYRO_SCL = 10;
// --- hardware pins ---


// --- GYRO STUFF ---
#define BNO08X_RESET -1

Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

double qr,qi,qj,qk;
double sqr,sqi,sqj,sqk;
float roll, pitch, yaw;
float m1,m2,m3,m4;
float throttle = 0;
// --- GYRO STUFF ---

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit BNO08x test!");

  Wire.begin(GYRO_SDA, GYRO_SCL); //SDA , SCL

  // Try to initialize!
  bno08x.begin_I2C(0x4A, &Wire);

  setReports();

  //Serial.println("Reading events");
  delay(100);
}

// Here is where you define the sensor outputs you want to receive
void setReports(void) {
  //Serial.println("Setting desired reports");
  if (! bno08x.enableReport(SH2_ROTATION_VECTOR)) {
    Serial.println("Could not enable rotation vector");
  }
}

void handle_gyro() {
  if (bno08x.wasReset()) {
    //Serial.print("sensor was reset ");
    setReports();
  }
  
  if (! bno08x.getSensorEvent(&sensorValue)) {
    return;
  }
  
  switch (sensorValue.sensorId) {

    case SH2_ROTATION_VECTOR:
      qr = sensorValue.un.rotationVector.real;
      qi = sensorValue.un.rotationVector.i;
      qj = sensorValue.un.rotationVector.j;
      qk = sensorValue.un.rotationVector.k;

      sqr = qr * qr;
      sqi = qi * qi;
      sqj = qj * qj;
      sqk = qk * qk;

      roll = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr)) * RAD_TO_DEG;
      pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr)) * RAD_TO_DEG;
      yaw = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr)) * RAD_TO_DEG;

      // Print labels and values formatted strictly for the Serial Plotter
      if (debug) {
        Serial.print("Orientation: ");
        Serial.print(roll, 4);
        Serial.print(", ");
        Serial.print(pitch, 4);
        Serial.print(", ");
        Serial.print(yaw, 4);
        Serial.println("");
      }
      break;
  }
}

void calc_motor() {

  m1 = 1500; //throttle -roll +pitch +yaw;
  m2 = 1500; //throttle -roll -pitch -yaw;
  m3 = 1000; //throttle +roll +pitch +yaw;
  m4 = 2000; //throttle +roll -pitch +yaw;


  if (debug) {
    Serial.print("Motor: ");
    Serial.print(m1, 4);
    Serial.print(", ");
    Serial.print(m2, 4);
    Serial.print(", ");
    Serial.print(m3, 4);
    Serial.print(", ");
    Serial.print(m4, 4);
    Serial.println("");
  }
}

void loop() {
  delay(10);

  handle_gyro();

  calc_motor();
}
