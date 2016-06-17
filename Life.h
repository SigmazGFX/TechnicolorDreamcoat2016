#pragma once

#include <FastLED.h>
#include "Matrix.h"
#include "Effect.h"

#define DENSITY 60

class Life : public Effect {
    
private:

    uint8_t hue;
    uint8_t array1[kMatrixWidth][kMatrixHeight];
    uint8_t array2[kMatrixWidth][kMatrixHeight];

    uint8_t (*currState)[kMatrixWidth][kMatrixHeight];
    uint8_t (*nextState)[kMatrixWidth][kMatrixHeight];
    
    uint8_t frame;

public:

    Life(CRGB *leds) : Effect(leds), hue(1), frame(0) {
        currState = &array1;
        nextState = &array2;
        seed(DENSITY);
    }

    void draw(Controls controls) {
      random16_add_entropy(controls.spectrum[0]);
      
        if (frame++ == 1 || controls.button) {
            seed(random8(DENSITY));
        }

//      Serial.println("Current state:");
//      for (int x = 0; x < kMatrixWidth; x++) {
//          for (int y = 0; y < kMatrixHeight; y++) {
//              Serial.print(" "); Serial.print((*currState)[x][y]);
//          }
//          Serial.println();
//      }
//      Serial.println();
        
        // Update the nextState array with the next generation (skip 0, our sentinel value)
        if (++hue == 0) {
            hue = 1;
        }

        
//        Serial.print("Hue for this generation = "); Serial.println(hue);
        for (int x = 0; x < kMatrixWidth; x++) {
            for (int y = 0; y < kMatrixHeight; y++) {
//                Serial.print("draw() x = "); Serial.print(x); Serial.print(", y = "); Serial.println(y);
                if (alive(x, y)) {
//                    Serial.println("It lives!");
                    if ((*currState)[x][y] == 0) {
                        // it's new, so give it the current hue
                       (*nextState)[x][y] = hue;
                    } else {
                        // it's old, so give it the existing hue
                        (*nextState)[x][y] = (*currState)[x][y];
                    }
                } else {
//                    Serial.println("It dies!");                  
                    (*nextState)[x][y] = 0;  
                }
//                Serial.println("Going around again?");
            }
        }
        
//      Serial.println("Next state:");
//      for (int x = 0; x < kMatrixWidth; x++) {
//          for (int y = 0; y < kMatrixHeight; y++) {
//              Serial.print(" "); Serial.print((*nextState)[x][y]);
//          }
//          Serial.println();
//      }
//      Serial.println();
        

        // Copy nextState into leds array
        copyToLedsArray(nextState);

        // make nextState our new currentState, ready for the next generation
        uint8_t (*tempState)[kMatrixWidth][kMatrixHeight] = currState;
        currState = nextState;
        nextState = tempState;
    };

    //Any live cell with fewer than two live neighbours dies, as if caused by under-population.
    //Any live cell with two or three live neighbours lives on to the next generation.
    //Any live cell with more than three live neighbours dies, as if by overcrowding.
    //Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
    bool alive(uint8_t x, uint8_t y) {
        uint8_t neighbours = numNeighbours(x, y);
        bool retVal = false;
        if ((*currState)[x][y] > 0) {
            retVal = neighbours == 2 || neighbours == 3;
        } else {
            retVal = neighbours == 3;
        }
//        Serial.print("alive("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(") = "); Serial.println(retVal);      
        return retVal;
    };

    uint8_t numNeighbours(uint8_t x, uint8_t y) {
        uint8_t neighbours = 0;

        // top left
        if ((*currState)[toroidalXindex(x - 1)][toroidalYindex(y + 1)] > 0) {
//            Serial.println("top left");
            neighbours++;
        }
        
        // top
        if ((*currState)[toroidalXindex(x)][toroidalYindex(y + 1)] > 0) {
//            Serial.println("top");          
            neighbours++;
        }
        
        // top right
        if ((*currState)[toroidalXindex(x + 1)][toroidalYindex(y + 1)] > 0) {
//            Serial.println("top right");          
            neighbours++;
        }
        
        // left
        if ((*currState)[toroidalXindex(x - 1)][toroidalYindex(y)] > 0) {
//            Serial.println("left");          
            neighbours++;
        }
        
        // right
        if ((*currState)[toroidalXindex(x + 1)][toroidalYindex(y)] > 0) {
//            Serial.println("right");          
            neighbours++;
        }
        
        // bottom left
        if ((*currState)[toroidalXindex(x - 1)][toroidalYindex(y - 1)] > 0) {
//            Serial.println("bottom left");          
            neighbours++;
        }          
        
        // bottom
        if ((*currState)[toroidalXindex(x)][toroidalYindex(y - 1)] > 0) {
//            Serial.println("bottom");          
            neighbours++;
        }
        
        // bottom right
        if ((*currState)[toroidalXindex(x + 1)][toroidalYindex(y - 1)] > 0) {
//            Serial.println("bottom right");          
            neighbours++;
        }

//        Serial.print("neighbours("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(") = "); Serial.println(neighbours);
        return neighbours;
    }

    uint8_t toroidalXindex(int8_t x) {
        uint8_t retVal;
        if (x < 0) {
            retVal = kMatrixWidth - 1;
        } else if (x == kMatrixWidth) {
            retVal = 0;
        } else {
          retVal = x;
        }
//        Serial.print("toroidalXindex("); Serial.print(x); Serial.print(") = "); Serial.println(retVal);
        return retVal;
    }

    uint8_t toroidalYindex(int8_t y) {
        if (y < 0) return kMatrixHeight - 1;
        if (y == kMatrixHeight) return 0;
        return y;
    }

    // This is called from the constructor, so if you put a Serial.println() here, you're gonna have a bad time    
    void seed(uint8_t chance) {
        for (int x = 0; x < kMatrixWidth; x++) {
            for (int y = 0; y < kMatrixHeight; y++) {
                if (random8() < chance) {
                    (*currState)[x][y] = hue;
                }
            }
        }
    }
    
    void copyToLedsArray(uint8_t (*state)[kMatrixWidth][kMatrixHeight]) {
        for (int x = 0; x < kMatrixWidth; x++) {
            for (int y = 0; y < kMatrixHeight; y++) {
                uint8_t hugh = (*state)[x][y];
                if (hugh > 0) {
                    leds[XY(x, y)] = CHSV(hugh, 255, 255);
                }
            }
        }      
    }
    
    bool blankEveryFrame() {
        return true;
    }
    
};



