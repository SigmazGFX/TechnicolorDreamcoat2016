#pragma once

#include <FastLED.h>
#include "Matrix.h"
#include "Effect.h"

class Snake : public Effect {
  
  private:
  
    static const uint8_t snakeLength = 16;
    
    enum Direction {
        UP, DOWN, LEFT, RIGHT
    };
    
    struct Pixel {
        uint8_t x;
        uint8_t y;
    };
    
    CRGB colours[snakeLength];
    
    uint8_t initialHue;
    
    Pixel pixels[snakeLength];
    
    Direction direction;
    
    void newDirection() {
        switch (direction) {
            case UP:
            case DOWN:
                direction = random(0, 2) == 1 ? RIGHT : LEFT;
                break;
                
            case LEFT:
            case RIGHT:
                direction = random(0, 2) == 1 ? DOWN : UP;
                
            default:
                break;
        }
    }

    void shuffleDown() {
        for (byte i = snakeLength - 1; i > 0; i--) {
            pixels[i] = pixels[i - 1];
        }
    }
  
  public:
    Snake(CRGB *leds) : Effect(leds), initialHue(0) {}
    
    virtual void draw(Controls controls) {
        shuffleDown();
        
        if (random(10) > 6) {
            newDirection();
        }
        
        switch (direction) {
            case UP:
                pixels[0].y = (pixels[0].y + 1) % kMatrixHeight;
                break;
                
            case LEFT:
                pixels[0].x = (pixels[0].x + 1) % kMatrixWidth;
                break;
                
            case DOWN:
                pixels[0].y = pixels[0].y == 0 ? kMatrixHeight - 1 : pixels[0].y - 1;
                break;
                
            case RIGHT:
                pixels[0].x = pixels[0].x == 0 ? kMatrixWidth - 1 : pixels[0].x - 1;
                break;
                
        }
        
        fill_rainbow(colours, snakeLength, initialHue++);
        
        for (uint8_t i = 0; i < snakeLength; i++) {
            leds[XY(pixels[i].x, pixels[i].y)] = colours[i] %= (255 - i * (255 / snakeLength));
        }
    }
    
    bool blankEveryFrame() {
        return true;
    }

};
