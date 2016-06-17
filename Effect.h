#pragma once

//
// Base class for effect instances
//

#include "FastLED.h"

struct Controls {

    static const uint8_t spectrumCount = 7;
    
    uint16_t spectrumTrim[spectrumCount] = { 32, 96, 128, 128, 132, 132, 100 };
    
    uint16_t spectrumRaw[spectrumCount];
    uint8_t spectrum[spectrumCount];
    
    bool button = false;
    
};

class Effect {
  
  protected:
    CRGB *leds;
    
  public:
    Effect(CRGB *leds) : leds(leds) {}
    
    virtual void draw(Controls controls) = 0;
    
    virtual bool blankEveryFrame() = 0;
    
};