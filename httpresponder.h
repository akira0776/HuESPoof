#ifndef __HTTPRESPONDER_H__
#define __HTTPRESPONDER_H__

#include <ESP8266WebServer.h>
#include <vector>
#include "huelight.h"

class HTTPResponder {

  public:
    HTTPResponder(String &BridgeID, unsigned int HTTPPort = 80);
    ~HTTPResponder() { };
    bool handle(ESP8266WebServer &HTTP, bool canpair, std::vector<HueLight> &Lights);
  private:
    unsigned int HTTPPort = 0;
    String BridgeID = "";
    String BridgeUID = "";
    String lightJson(HueLight &Light);
    String handleDescription(ESP8266WebServer &HTTP);
    String handleApi(ESP8266WebServer &HTTP, bool canpair);
    String handleLights(ESP8266WebServer &HTTP, std::vector<HueLight> &Lights);
    String handleLight(unsigned char id, ESP8266WebServer &HTTP, std::vector<HueLight> &Lights);
    String handleControl(ESP8266WebServer &HTTP, std::vector<HueLight> &Lights);
};

#endif