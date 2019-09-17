# huESPoof
#### Yet another ESP8266 Hue bridge emulator

### Preamble
huESPoof is one more Phillips Hue emulator for ESP8266.
There are already many similar projects like [ESP8266HueEmulator](https://github.com/probonopd/ESP8266HueEmulator) or [FauxmoESP](https://bitbucket.org/xoseperez/fauxmoesp/src/master/) or but the ones I found didn't suit me. Don't get me wrong, they are nice pieces of work, just don't meet my expectation.
Some require old (outdated?) versions of the espressif/arduino framework, just won't compile at all, or device would not be detected by Alexa, and some even have dependencies, so I figured out it would be faster to code my own library that I could reuse at will.

This is an amateur library, so is far from being l33t optimized, but it does the job for me so here it is.
It supports the minimum needed for an ESP to expose one or more lights to Alexa (SSDP discovery, HTTP responses).
My main goal was a library that would add Hue support to any ESP8266 project with a minimum of work, so it's meant to be easy, yet powerful.

### Example :
```
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
```

So there we go, the above example will expose the foo, bar and baz lights to alexa, and allow it to pair for 120s, after that delay it will be able to control the paired lights, but not discover them again (that's some sort of lame security).

If for some reason you can't use alexa to control the lights, there is a backup web page to do so, at the moment it only manages on/off state, not brightness, but eh, better than nothing.
you can access it using http://esp8266-ip/control

Various debug flags can be used at compile time, I do not use arduino IDE so don't ask me how to set them :-P for platformio, you can simply add the following to your platformio.ini:
```
build_flags = -D HTTP_DEBUG -D SSDP_DEBUG
```

Addidionally, it looks like adding the following helps with device discovery, even though I didn't have any issue so far :
```
build_flags = -D PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH
```

