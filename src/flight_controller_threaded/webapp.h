#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <string.h>

#include "commands.h"
#include "index_html.h"

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

WebServer server(80);
bool ledState = false;

void handleRoot() { server.send_P(200, "text/html", PAGE); }
void handleState() { server.send(200, "text/plain", ledState ? "ON" : "OFF"); }
static int getFlightData(String * queue, int max);

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

void handleToggle(){
  pressButton();
  ledState = !ledState;
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
}

void reboot(){
  char *args[] = { "reboot", NULL };
  cmdReboot(1, args);
  server.send(200, "text/plain", "Rebooting");
}
void systemStatus(){
  char *args[] = { "uptime", "memory", "temp", NULL };
  cmdStatus(2, args);
  String body;
  for (int i = 0; args[i] != NULL; i++) {
    body += args[i];
    body += "\n";
  }
  server.send(200, "text/plain", body);
}
void armESC(){
  char *args[] = { "esc", "arm", NULL };
  cmdESC(2, args);
  server.send(200, "text/plain", "ARMED");
}
void disarmESC(){
  char *args[] = { "esc", "disarm", NULL };
  cmdESC(2, args);
  server.send(200, "text/plain", "DISARMED");
}

// new ^^^

void webApplication(void *pvParameters)
{
  Serial.begin(115200);
  pinMode(togglePin, OUTPUT);
  digitalWrite(togglePin, LOW);

  String webQueue[8];
  int count;

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
  server.on("/systemstatus", systemStatus);
  server.on("/esc/arm", armESC);
  server.on("/esc/disarm", disarmESC);

  server.begin();

  for(;;){
    server.handleClient();


    count = getFlightData(webQueue, 8);
    for(int i = 0; i < count; i++) {
      server.send(200, "text/plain", webQueue[i]);
    }
  }
}