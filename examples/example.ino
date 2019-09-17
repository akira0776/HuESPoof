#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "huESPoof.h"

#define WIFI_SSID "my-wifi-ssid"
#define WIFI_PASS "my-wifi-psk"
#define SERIAL_BAUDRATE 74880
#define HTTP_PORT 80

ESP8266WebServer HTTPServer(HTTP_PORT);
HuESPoof Bridge(HTTP_PORT);

// callback for the "bar" light
void barCallback(bool state, unsigned char brightness) {
  // here we can handle the event in many ways (physical pin toggle, mqtt publication, analog write, etc.)
}

//callback for the "baz" light
void bazCallback(bool state, unsigned char brightness) {
  // same as above, obviously
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (!WiFi.isConnected()) {
    delay(250);
  }
  
  Bridge.addLight("foo");                    // add a "foo" light with no callback, basicly will create a dummy device
  Bridge.addLight("bar", barCallback);       // add a "bar" light which is a simple on/off light
  Bridge.addLight("baz", bazCallback, true); // add a "baz" light which is a dimmer
  Bridge.setPairable(120);                   // allow pairing for 120s after device startup
  
  HTTPServer.onNotFound([]() {
    if (!Bridge.handleHTTP(HTTPServer)) { // here we handle http requests
      HTTPServer.send(404, "text/plain", "page not found");
    }
  });
  HTTPServer.begin();
}

void loop() {
  HTTPServer.handleClient();
  Bridge.handleSSDP(); // here we handle ssdp requests
  delay(10);
}