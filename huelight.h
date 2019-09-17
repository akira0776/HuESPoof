#ifndef __HUELIGHT_H__
#define __HUELIGHT_H__

#include <Arduino.h>

typedef void (*LightEventCallback) (bool state, unsigned char brightness);

class HueLight {
  public:
    HueLight(String name, String uid, LightEventCallback callback = NULL, bool hasBrightness = false, bool state = false, unsigned char brightness = 1);
    ~HueLight() {};
    bool setName(String name);
    String getName();
    String getUID();
    bool setState(bool state);
    bool getState();
    bool setBrightness(unsigned char brightness);
    unsigned char getBrightness();
    bool hasBrightness;
  private:
    void callBack();
    LightEventCallback callback;
    String name;
    String uid;
    bool state;
    unsigned char brightness;
};
#endif