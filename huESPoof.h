#ifndef __HUESPOOF_H__
#define __HUESPOOF_H__

#include <ESP8266WebServer.h>
#include <vector>
#include "httpresponder.h"
#include "ssdpresponder.h"
#include "huelight.h"

class HuESPoof {

  public:
    HuESPoof(unsigned int HTTP_PORT = 80, IPAddress MulticastAddress = IPAddress(239,255,255,250), unsigned int MulticastPort = 1900, long PairingTimeout = -1);
    ~HuESPoof() {};
    bool handleSSDP();
    bool handleHTTP(ESP8266WebServer &HTTP);
    void setPairable(long PairingTimeout = -1);
    unsigned char addLight(String name, LightEventCallback callback = NULL, bool hasBrightness = false);
    unsigned char getLightCount();
    String getLights();

  private:
    unsigned int HTTP_PORT;
    String BridgeID;
    SSDPResponder* ssdpresponder;
    HTTPResponder* httpresponder;
    std::vector<HueLight> Lights;
    long PairingTimeout = 0;
    bool isPairable();
};
#endif