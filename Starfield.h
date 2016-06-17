#pragma once

#include <FastLED.h>
#include "Matrix.h"
#include "Effect.h"

struct Star {
    uint16_t x;
    uint8_t y;
    uint8_t plane;
};

const uint8_t planes[] = {32, 92, 255};
const uint8_t maxStars = kMatrixWidth * kMatrixHeight / 15;

class Starfield : public Effect {
  
  private:
    int frame;
    Star stars[maxStars];
  
  public:
    Starfield(CRGB *leds) : Effect(leds), frame(0) {
        for (int i = 0; i < maxStars; i++) {
            stars[i].x = random16(kMatrixWidth * 0xFF);
            stars[i].y = random8(kMatrixHeight);
            stars[i].plane = random8(3);
        }
    }
    
    void draw(Controls controls) {
        for (int i = 0; i < maxStars; i++) {
            stars[i].x += (1 + stars[i].plane) * 13;
            
            // Check if our star is off the right of the screen
            if (stars[i].x > (kMatrixWidth * 0xFF)) {
                // If so, put it back on the left
                stars[i].x = 0;
            }
            
            uint8_t actualPart = stars[i].x >> 8;
            uint8_t fractionalPart = stars[i].x & 0xFF;
            
//            Serial.print("XY("); Serial.print(actualPart); Serial.print(", "); Serial.print(stars[i].y); Serial.println(")");
//            Serial.print("XY("); Serial.print(actualPart + 1); Serial.print(", "); Serial.print(stars[i].y); Serial.println(")");
            leds[XY(actualPart    , stars[i].y)] = CRGB(planes[stars[i].plane], planes[stars[i].plane], planes[stars[i].plane]) %= (0xFF - fractionalPart);
            leds[XY(actualPart + 1 == kMatrixWidth ? 0 : actualPart + 1, stars[i].y)] = CRGB(planes[stars[i].plane], planes[stars[i].plane], planes[stars[i].plane]) %= fractionalPart;
        }
    }
    
    bool blankEveryFrame() {
        return true;
    }

};



