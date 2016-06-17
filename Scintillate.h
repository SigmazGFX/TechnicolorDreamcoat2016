#pragma once

#include <FastLED.h>
#include "Matrix.h"
#include "Effect.h"

#define CONCURRENT_SPARKLES (kNumLeds/20)

struct Sparkle {
    uint8_t x, y;
    uint8_t colourIndex;
};

static CRGB sparklePalette[] = {CRGB::Black, CRGB::White, CRGB::DarkBlue};

class Scintillate : public Effect {
  
  private:
    bool soundReactive;
    uint8_t brightness;
    Sparkle sparkles[CONCURRENT_SPARKLES];
    
  public:
    Scintillate(CRGB *leds) : Effect(leds), soundReactive(false), brightness(0) {
    }
    
    virtual void draw(Controls controls) {
        if (controls.button) {
            soundReactive = !soundReactive;
        }
        if (controls.spectrum[0] > 192) {
            brightness = 255;
        }
        for (int i = 0; i < CONCURRENT_SPARKLES; i++) {
            if (sparkles[i].colourIndex == 0) {
                if (random() > 128) {                  
                    sparkles[i].x = random(kMatrixWidth);
                    sparkles[i].y = random(kMatrixHeight);
                    sparkles[i].colourIndex = 1;
                }                
            } else if (sparkles[i].colourIndex == 2) {
                sparkles[i].colourIndex = 0;                
            } else {
                sparkles[i].colourIndex++;
            }
            CRGB sparkle = CRGB(sparklePalette[sparkles[i].colourIndex]);
            if (soundReactive) {
                sparkle.nscale8(brightness);
            }
            leds[XY(sparkles[i].x, sparkles[i].y)] = sparkle;
        }
        brightness = constrain(brightness - 48, 0, 255);
    }
    
    bool blankEveryFrame() {
        return true;
    }
    
};



