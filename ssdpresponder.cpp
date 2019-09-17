#include "ssdpresponder.h"

SSDPResponder::SSDPResponder(unsigned int HTTPPort, IPAddress MulticastAddress, unsigned int MulticastPort) {
  this->HTTPPort = HTTPPort;
  UDPServer.beginMulticast(WiFi.localIP(), MulticastAddress, MulticastPort);
} 

void SSDPResponder::handle() {
  int len = UDPServer.parsePacket();
  if (len > 0) {
    unsigned char data[len+1];
    UDPServer.read(data, len);
    data[len] = 0;
    String request = (const char *) data;
    if (request.indexOf("M-SEARCH") >= 0) {
      String IP = WiFi.localIP().toString();
      String mac = WiFi.macAddress();
      mac.replace(":", "");
      mac.toLowerCase();
      char response[strlen(SSDP_RESPONSE) + 128];
      sprintf(response, SSDP_RESPONSE, IP.c_str(), this->HTTPPort, mac.c_str());
      #ifdef SSDP_DEBUG
        Serial.printf("Received M-SEARCH packet:\n%sUDP response sent to %s:%d\n%s", data, UDPServer.remoteIP().toString().c_str(), UDPServer.remotePort(), response);
      #endif
      UDPServer.beginPacket(UDPServer.remoteIP(), UDPServer.remotePort());
      UDPServer.write(response);
      UDPServer.endPacket();
    }
  }
}

