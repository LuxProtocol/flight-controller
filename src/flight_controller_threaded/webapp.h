#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>             // by ESP32Async
#include <ESPAsyncWebServer.h>    // by ESP32Async
#include <string.h>

//#include "commands.h"
#include "index_html.h"
#include "mon_struts.h"
#include "img_struts.h"

#define togglePin 4
#define high 1
#define low 0

// ---- Settings ----
#define USE_ACCESS_POINT true 

const char *AP_SSID = "Aerial Matrix";
const char *AP_PASS = "12345678"; // at least 8 characters

const char *WIFI_SSID = "your_ssid"; // only used when USE_ACCESS_POINT is false
const char *WIFI_PASS = "your_password";
// ------------------

// ---- Monitoring ---- //

static char  buf[SYSMON_LINES_BUF_BYTES];
static char *lines[SYSMON_MAX_LINES];

// -------------------- //

AsyncWebServer server(80);
AsyncEventSource events("/events");
AsyncWebSocket ws("/ws");
bool ledState = false;

void handleRoot(AsyncWebServerRequest *request) { request->send_P(200, "text/html", PAGE); }
void handleState(AsyncWebServerRequest *request) { request->send(200, "text/plain", ledState ? "ON" : "OFF"); }
static int getFlightData(char **queue, int max);

/*void handleToggle()
{
  ledState = !ledState;
  digitalWrite(togglePin, ledState);
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
}
*/
// new below
#define PRESS_MS 150 // try 100-300 if the lamp doesn't respond

void pressButton(){
  digitalWrite(togglePin, HIGH);
  delay(PRESS_MS);
  digitalWrite(togglePin, LOW);
}

void handleToggle(AsyncWebServerRequest *request){
  pressButton();
  ledState = !ledState;
  request->send(200, "text/plain", ledState ? "ON" : "OFF");
}

void reboot(AsyncWebServerRequest *request){
  char *args[] = { "reboot", NULL };
  cmdReboot(1, args);
  request->send(200, "text/plain", "Rebooting");
}
void systemStatus(){
  String temp_str = "temp: " + String(getTemp());
  const char *temp = temp_str.c_str();
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

// new ^^^

// ---    AI Streaming      --- //
#define MAX_FRAME_BYTES (256 * 1024)
static void onIngestBody(AsyncWebServerRequest *r, uint8_t *data, size_t len,
                         size_t index, size_t total) {
  if (index == 0) {
    if (total < 16 || total > MAX_FRAME_BYTES) return;     // leaves _tempObject null
    r->_tempObject = allocBuf(total);                      // freed by the request if unused
  }
  if (r->_tempObject) memcpy((uint8_t *)r->_tempObject + index, data, len);
}
static void onIngest(AsyncWebServerRequest *r) {
  uint8_t *msg = (uint8_t *)r->_tempObject;
  size_t total = r->contentLength();
  if (!msg) { r->send(400, "text/plain", "bad size"); return; }

  uint32_t jl;
  memcpy(&jl, msg, 4);
  if ((size_t)jl + 4 >= total) { r->send(400, "text/plain", "bad header"); return; }

  r->_tempObject = nullptr;                                // take ownership of the buffer
  auto f = std::make_shared<Frame>();
  f->msg = msg;
  f->msgLen = total;
  f->jsonLen = jl;
  f->jpegOff = 4 + jl;
  f->jpegLen = total - 4 - jl;
  f->id = ++frameCounter;
  latest = f;
  framesIn++;

  // Relay to every browser. Drop the frame if some client's queue is full.
  if (ws.count() && ws.availableForWriteAll()) ws.binaryAll(f->msg, f->msgLen);

  r->send(204);
}

static void onWsEvent(AsyncWebSocket *, AsyncWebSocketClient *client, AwsEventType type,
                      void *, uint8_t *, size_t) {
  if (type == WS_EVT_CONNECT && latest)                    // show something immediately
    client->binary(latest->msg, latest->msgLen);
}
// ---    AI Streaming      --- //

void webApplication(void *pvParameters)
{
  Serial.begin(115200);
  pinMode(togglePin, OUTPUT);
  digitalWrite(togglePin, LOW);

  char *webQueue[4];
  int count;
  static uint32_t lastRun = 0;

#if USE_ACCESS_POINT
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

  // ---    Attachments       --- //
  server.on("/toggle", handleToggle);
  server.on("/state", handleState);
  // ---    Flight Control    --- //
  server.on("/reboot", reboot);
  //server.on("/systemstatus", systemStatus);
  server.on("/esc/arm", armESC);
  server.on("/esc/disarm", disarmESC);


  // ---    AI Streaming      --- //
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/ingest", HTTP_POST, onIngest, nullptr, onIngestBody);
  server.on("/snapshot.jpg", HTTP_GET, [](AsyncWebServerRequest *r) {
    FramePtr f = latest;
    if (!f) { r->send(503, "text/plain", "No frame yet"); return; }
    AsyncWebServerResponse *res = r->beginResponse(
        "image/jpeg", f->jpegLen,
        [f](uint8_t *buf, size_t maxLen, size_t index) -> size_t {   // f keeps frame alive
          size_t n = std::min(maxLen, f->jpegLen - index);
          memcpy(buf, f->msg + f->jpegOff + index, n);
          return n;
        });
    res->addHeader("Cache-Control", "no-store");
    r->send(res);
  });
  server.on("/results.json", HTTP_GET, [](AsyncWebServerRequest *r) {
    FramePtr f = latest;
    if (!f) { r->send(503, "text/plain", "No frame yet"); return; }
    String s;
    s.concat((const char *)f->msg + 4, f->jsonLen);
    AsyncWebServerResponse *res = r->beginResponse(200, "application/json", s);
    res->addHeader("Cache-Control", "no-store");
    r->send(res);
  });
  // ---    AI Streaming      --- //

  server.addHandler(&events);
  server.begin();

  for(;;){
    //server.handleClient();
    ws.cleanupClients();

    count = getFlightData(webQueue, 2);
    for(int i = 0; i < count; i++) {
      events.send(webQueue[i], "flightData", millis());
    }

    if (millis() - lastRun >= 2500) {
      lastRun = millis();
      systemStatus();
    }

    //vTaskDelay(pdMS_TO_TICKS(50));
  }
}