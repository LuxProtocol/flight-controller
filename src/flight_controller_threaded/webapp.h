#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <AsyncTCP.h>             // by ESP32Async
#include <ESPAsyncWebServer.h>    // by ESP32Async
#include <string.h>

//#include "commands.h"
#include "index_html.h"
#include "mon_struts.h"

// ---- WiFi Settings ---- //
#define USE_ACCESS_POINT true 

const char *AP_SSID = "Aerial Matrix";
const char *AP_PASS = "12345678"; // at least 8 characters

IPAddress AP_IP(192, 168, 4, 1);
IPAddress AP_GATEWAY(192, 168, 4, 1);
IPAddress AP_SUBNET(255, 255, 255, 0);

const char *WIFI_SSID = "your_ssid"; // only used when USE_ACCESS_POINT is false
const char *WIFI_PASS = "your_password";
// ------------------

// ---- Monitoring ---- //
static char  buf[SYSMON_LINES_BUF_BYTES];
static char *lines[SYSMON_MAX_LINES];
// -------------------- //

// ---- Attachments ---- //
#define LIGHT_MODULE          3100    //  2.5V
#define GROVE_AI_MODULE       2500    //  2.0V
#define PORTENTA_AI_MODULE    1850    //  1.5V
#define ENVIRONMENTAL_MODULE  1250    //  1.0V

#define ATTACHMENT_1        18
#define ATTACHMENT_1_SENSE  17
#define ATTACHMENT_2        12
#define ATTACHMENT_2_SENSE  13
uint8_t togglePin = 0;
bool ledState = false;

#define ATTACH_TOLERANCE   100   // +/- ADC counts around each signature
#define ATTACH_SCAN_MS     500   // how often the loop rescans the sense pins
 
enum AttachmentType : uint8_t {
  ATT_NONE = 0,
  ATT_LIGHT,
  ATT_GROVE_AI,
  ATT_PORTENTA_AI,
  ATT_ENVIRONMENTAL
};
 
// attachments[0] = attachment 1, attachments[1] = attachment 2
// (each port holds at most one module; either port may be empty)
static AttachmentType attachments[2] = { ATT_NONE, ATT_NONE };

static const char *attachmentName(AttachmentType t) {
  switch (t) {
    case ATT_LIGHT:         return "LIGHT";
    case ATT_GROVE_AI:      return "GROVE_AI";
    case ATT_PORTENTA_AI:   return "PORTENTA_AI";
    case ATT_ENVIRONMENTAL: return "ENVIRONMENTAL";
    default:                return "NONE";
  }
}

static AttachmentType readAttachment(uint8_t sensePin) {
  int counts = analogRead(sensePin);
  if (abs(counts - LIGHT_MODULE)         <= ATTACH_TOLERANCE) return ATT_LIGHT;
  if (abs(counts - GROVE_AI_MODULE)      <= ATTACH_TOLERANCE) return ATT_GROVE_AI;
  if (abs(counts - PORTENTA_AI_MODULE)   <= ATTACH_TOLERANCE) return ATT_PORTENTA_AI;
  if (abs(counts - ENVIRONMENTAL_MODULE) <= ATTACH_TOLERANCE) return ATT_ENVIRONMENTAL;
  return ATT_NONE;  // empty port / no matching signature
}

static void checkAttachments() {
  attachments[0] = readAttachment(ATTACHMENT_1_SENSE);
  attachments[1] = readAttachment(ATTACHMENT_2_SENSE);
}
// --------------------- //

// ---- Module health / data ---- //
#define MODULE_TIMEOUT_MS 15000     // timeout
 
static volatile uint32_t envLastSeen = 0;
static volatile bool     envEverSeen = false;
static volatile uint32_t aiLastSeen  = 0;
static volatile bool     aiEverSeen  = false;
 
// Latest correction from the AI module
static volatile float    aiDeltaX = 0.0f;
static volatile float    aiDeltaY = 0.0f;
static volatile uint32_t aiDataMs = 0;
 
bool getAiCorrection(float *dx, float *dy, uint32_t *ageMs) {
  if (!aiDataMs) return false;
  *dx = aiDeltaX;
  *dy = aiDeltaY;
  *ageMs = millis() - aiDataMs;
  return true;
}
 
static String moduleStatus(bool everSeen, uint32_t lastSeen) {
  if (!everSeen) return "WAITING (no contact yet)";
  uint32_t age = millis() - lastSeen;
  return String(age > MODULE_TIMEOUT_MS ? "TIMEOUT" : "OK") + " (" + String(age) + " ms since last)";
}
// ------------------------------ //


AsyncWebServer server(80);
AsyncEventSource events("/events");

void handleRoot(AsyncWebServerRequest *request) { request->send_P(200, "text/html", PAGE); }
static int getFlightData(char **queue, int max);

// new below
#define PRESS_MS 150 // try 100-300 if the lamp doesn't respond

void handleToggle(AsyncWebServerRequest *request){
  if(!togglePin){ return; }
  digitalWrite(togglePin, HIGH);
  delay(PRESS_MS);
  digitalWrite(togglePin, LOW);
  ledState = !ledState;
  request->send(200, "text/plain", ledState ? "ON" : "OFF");
}

void envHealth(AsyncWebServerRequest *request){
  envLastSeen = millis();
  envEverSeen = true;
  request->send(200, "text/plain", "OK");
}

void aiHealth(AsyncWebServerRequest *request){
  aiLastSeen = millis();
  aiEverSeen = true;
  request->send(200, "text/plain", "OK");
}


void reboot(AsyncWebServerRequest *request){
  char *args[] = { "reboot", NULL };
  cmdReboot(1, args);
  request->send(200, "text/plain", "Rebooting");
}
void systemStatus(){
  String temp_str = "temp: " + String(getTemp());
  const char *temp = temp_str.c_str();  

  String att_str = "att1: " + String(attachmentName(attachments[0])) + ", att2: " + String(attachmentName(attachments[1]));
  events.send(att_str.c_str(), "attachments", millis());


  String env_str = "env: " + moduleStatus(envEverSeen, envLastSeen);
  events.send(env_str.c_str(), "envHealth", millis());
 
  String ai_str = "ai: " + moduleStatus(aiEverSeen, aiLastSeen);
  events.send(ai_str.c_str(), "aiHealth", millis());

  events.send(temp, "temperature", millis());
  for (int j = 0; j < system_monitor_get_lines(buf, sizeof buf, lines, SYSMON_MAX_LINES); j++){ 
      events.send(lines[j], "monitoringData", millis());
    }
}
void armESC(AsyncWebServerRequest *request){
  char *args[] = { "esc", "arm", NULL };
  cmdESC(2, args);
  request->send(200, "text/plain", "ARMED");
}
void disarmESC(AsyncWebServerRequest *request){
  char *args[] = { "esc", "disarm", NULL };
  cmdESC(2, args);
  request->send(200, "text/plain", "DISARMED");
}

static bool getFloatArg(AsyncWebServerRequest *request, const char *name, float &out){
  if (request->hasParam(name)) {
    out = request->getParam(name)->value().toFloat();
    return true;
  }
  if (request->hasParam(name, true)) {
    out = request->getParam(name, true)->value().toFloat();
    return true;
  }
  return false;
}
 
// AI module reports correction ~1/s:  /acc/ai/data?dx=1.5&dy=-0.8
void aiData(AsyncWebServerRequest *request){
  float dx, dy;
  if (!getFloatArg(request, "dx", dx) || !getFloatArg(request, "dy", dy)) {
    request->send(400, "text/plain", "Missing dx or dy");
    return;
  }
  aiDeltaX = dx;
  aiDeltaY = dy;
  aiDataMs = millis();
 
  // A data report also proves the module is alive
  aiLastSeen = aiDataMs;
  aiEverSeen = true;
 
  // TODO: hand dx/dy to the flight controller here if it should act on them immediately
  request->send(200, "text/plain", "OK");
}



// --- WEB THREAD BELOW --- //

void webApplication(void *pvParameters)
{
  Serial.begin(115200);
  pinMode(togglePin, OUTPUT);
  digitalWrite(togglePin, LOW);

  char *webQueue[4];
  int count;
  static uint32_t lastRun = 0;;
  static uint32_t lastAttachScan = 0;

#if USE_ACCESS_POINT
  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("Connect to Wi-Fi '");
  Serial.print(AP_SSID);
  Serial.println("', then open:");
  Serial.println(WiFi.softAPIP()); // 192.168.4.1 by default
#else
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Open in your browser: ");
  Serial.println(WiFi.localIP());
#endif

  server.on("/", handleRoot);

  // ---    Flight Control    --- //
  server.on("/reboot", reboot);
  server.on("/esc/arm", armESC);
  server.on("/esc/disarm", disarmESC);

  // ---    Attachments       --- //
  server.on("/acc/light/toggle", handleToggle);       // toggle light attachment(s)
  server.on("/acc/env/health", envHealth);            // response health of environmental module(s)
  server.on("/acc/ai/health", aiHealth);              // response health of ai module(s)
  server.on("/acc/ai/data", aiData);                  // accepts data from ai modules

  server.addHandler(&events);
  server.begin();

  for(;;){

    count = getFlightData(webQueue, 2);
    for(int i = 0; i < count; i++) {
      events.send(webQueue[i], "flightData", millis());
    }

    if (millis() - lastAttachScan >= ATTACH_SCAN_MS) {
      lastAttachScan = millis();
      checkAttachments();
    }

    if (millis() - lastRun >= 2500) {
      lastRun = millis();
      systemStatus();
    }
    vTaskDelay(10);
  }
}