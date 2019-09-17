#include "huESPoof.h"

HuESPoof::HuESPoof(unsigned int HTTP_PORT, IPAddress MulticastAddress, unsigned int MulticastPort, long PairingTimeout) {
  this->HTTP_PORT = HTTP_PORT;
  this->BridgeID = WiFi.macAddress();
  this->BridgeID.replace(":", "");
  this->BridgeID.toLowerCase();
  this->ssdpresponder = new SSDPResponder(this->HTTP_PORT, MulticastAddress, MulticastPort);
  this->httpresponder = new HTTPResponder(this->BridgeID, this->HTTP_PORT);
  if (PairingTimeout != -1) {
    this->PairingTimeout = millis() + PairingTimeout * 1000;
  } else {
    this->PairingTimeout = PairingTimeout;
  }
}

bool HuESPoof::handleSSDP() {
    this->ssdpresponder->handle();
    return true;
}

bool HuESPoof::handleHTTP(ESP8266WebServer &HTTP) {
    return this->httpresponder->handle(HTTP, this->isPairable(), this->Lights);
}

void HuESPoof::setPairable(long PairingTimeout) {
  if (PairingTimeout != -1) {
    this->PairingTimeout = millis() + PairingTimeout * 1000;
  } else {
    this->PairingTimeout = PairingTimeout;
  }
}

bool HuESPoof::isPairable() {
  if (this->PairingTimeout == -1 || (long)millis() < this->PairingTimeout) return true; else return false;
}

unsigned char HuESPoof::addLight(String name, LightEventCallback callback, bool hasBrightness) {
  unsigned char id = this->Lights.size();
  this->Lights.push_back(HueLight(name, this->BridgeID + "-" + String(id, DEC), callback, hasBrightness));
  return id;
}
