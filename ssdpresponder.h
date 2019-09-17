#ifndef __SSDPRESPONDER_H__
#define __SSDPRESPONDER_H__

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char PROGMEM SSDP_RESPONSE[] =
    "HTTP/1.1 200 OK\r\n"
    "EXT:\r\n"
    "CACHE-CONTROL: max-age=100\r\n"
    "LOCATION: http://%s:%u/description.xml\r\n"
    "SERVER: ESP8266Hue/1.0, UPnP/1.0, IpBridge/1.17.0\r\n"
    "ST: urn:schemas-upnp-org:device:basic:1\r\n"
    "USN: uuid:2f402f80-da50-11e1-9b23-%s::upnp:rootdevice\r\n"
    "\r\n";

class SSDPResponder {

  public:
    SSDPResponder(unsigned int HTTPPort = 80, IPAddress MulticastAddress = IPAddress(239,255,255,250), unsigned int MulticastPort = 1900);
    ~SSDPResponder() { };
    void handle();
  private:
    WiFiUDP UDPServer;
    unsigned int HTTPPort;
};

#endif