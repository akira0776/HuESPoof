#include "huelight.h"

HueLight::HueLight(String name, String uid, LightEventCallback callback, bool hasBrightness, bool state, unsigned char brightness) {
   this->name = name;
   this->uid = uid;
   this->hasBrightness = hasBrightness;
   this->state = state;
   this->brightness = brightness;
   this->callback = callback;
}

bool HueLight::setName(String name) {
    this->name = name;
    return true;
}

String HueLight::getName() {
    return this->name;
}

String HueLight::getUID() {
  return this->uid;
}

bool HueLight::setState(bool state) {
    if (this->state != state) {
        this->callBack();
        this->state = state;
    }
    return true;
}

bool HueLight::getState() {
    return this->state;
}

bool HueLight::setBrightness(unsigned char brightness) {
    if (this->hasBrightness) {
        if (this->brightness != brightness) {
            this->brightness = brightness;
            this->callBack();
        }
        return true;
    } else {
        return false;
    }
}

unsigned char HueLight::getBrightness() {
  if (this->brightness == 0) this->brightness = 1; // 0 is NOT supported
  return this->brightness;
}

void HueLight::callBack() {
  if (this->callback != NULL) {
    Serial.println((int)this->callback);
    this->callback(this->getState(), this->getBrightness());
  }
}