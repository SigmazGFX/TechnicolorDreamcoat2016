#pragma once

#include <FastLED.h>
#include "Matrix.h"
#include "Effect.h"

//
// Adapted from a FastLED G+ post by Stefan Petrick
// https://plus.google.com/115124694226931502095/posts/iMQiXg3tTH3
//

#define MIDPOINT 10

class Animation22 : public Effect {
  
  private:
    
  public:
    Animation22(CRGB *leds) : Effect(leds) {}
    
    virtual void draw(Controls controls) {
        fadeToBlackBy(leds, kNumLeds, 2);
        fillHorizontal(MIDPOINT, CRGB(controls.spectrum[6], controls.spectrum[5] / 2, controls.spectrum[1] / 2));
        fadeHorizontal(MIDPOINT, controls.spectrum[3] / 12);
        
        //move up
        for (int i = kMatrixHeight - 1; i > MIDPOINT; i--) {
            fillHorizontal(i, leds[XY(0, i - 1)]);
        }
        // move down
        for (int i = 0; i < MIDPOINT; i++) {
            fillHorizontal(i, leds[XY(0, i + 1)]);
        }
        blur2d(leds, kMatrixWidth, kMatrixHeight, 64);
    }
    
    void fillHorizontal(uint8_t y, CRGB colour) {
        for (int i = 0; i < kMatrixWidth; i++) {
            leds[XY(i, y)] = colour;
        }
    }
    
    void fadeHorizontal(uint8_t y, uint8_t fadeAmount) {
        for (int i = 0; i < kMatrixWidth; i++) {
            leds[XY(i, y)].fadeToBlackBy(fadeAmount);
        }
    }
    
    bool blankEveryFrame() {
        return false;
    }

};