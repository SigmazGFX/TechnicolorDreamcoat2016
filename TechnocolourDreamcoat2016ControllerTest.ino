#include <FastLED.h>

#include "Matrix.h"
#include "Chase.h"
#include "Plasma.h"
#include "Snake.h"
#include "Life.h"
#include "Starfield.h"
#include "Perlin.h"
#include "Scintillate.h"
#include "Animation22.h"

const uint8_t ledPin = 0;
const uint8_t maxPowerLedPin = 13;

CRGB leds[kNumLeds];

const int analogPin = 14; // read from multiplexer using analog input 0
const int strobePin = 15; // strobe is attached to digital pin 2
const int resetPin = 12; // reset is attached to digital pin 3

Controls controls;

// Master brightness pot
const int brightnessPotPin = 16;

const uint8_t debounceCount = 16;

// Control (blue) button
const uint8_t controlButtonPin = 18;
uint8_t controlButtonDebounce = 0;

// Down (red) button
const uint8_t downButtonPin = 19;
uint8_t downButtonDebounce = 0;
bool downButton = false;

// Up (green) button
const uint8_t upButtonPin = 20;
uint8_t upButtonDebounce = 0;
bool upButton = false;

// Effects
Chase chase(leds);
Starfield starfield(leds);
Scintillate scintillate(leds);
Plasma plasma(leds);
Snake snake(leds);
Life life(leds);
Perlin perlin(leds);
Animation22 animation22(leds);

Effect* effects[] = {
  &chase,
  &starfield,
  &scintillate,
  &snake,
  &life,
  &perlin,
  &plasma,
  &animation22,
  NULL
};
uint8_t effectIndex = 0;
uint8_t effectCount = 0;

//
// Prototypes, required by Arduino-1.6.7
//
void updateMasterBrightness();
void updateButtonValues();
void updateSpectrumValues();
void debugSpectrumValues();
void updateCurrentPattern();

void setup() {
  Serial.begin(9600);

  delay(2000);
  
  pinMode(brightnessPotPin, INPUT);
  pinMode(controlButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(upButtonPin, INPUT_PULLUP);

  while (effects[effectIndex++] != NULL) {
    effectCount++;
  }
  effectIndex = 0;

  FastLED.addLeds<WS2812B, ledPin, GRB>(leds, kNumLeds);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);
  pinMode(ledPin, OUTPUT);
  set_max_power_indicator_LED(maxPowerLedPin);
  
  pinMode(maxPowerLedPin, OUTPUT);
    
  pinMode(analogPin, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  analogReference(EXTERNAL);
  
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
}

void loop() {
  updateMasterBrightness();
  updateSpectrumValues();
  updateButtonValues();
  updateCurrentPattern();

  if (effects[effectIndex]->blankEveryFrame()) {
    fill_solid(leds, kNumLeds, CRGB::Black);
  }
  effects[effectIndex]->draw(controls);

  FastLED.show();
}

void updateSpectrumValues() {
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);  
  for (int i = 0; i < Controls::spectrumCount; i++) {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    controls.spectrumRaw[i] = analogRead(analogPin);
    controls.spectrum[i] = map(analogRead(analogPin), controls.spectrumTrim[i], 1023, 0, 255);
    digitalWrite(strobePin, HIGH);
  }
//  debugSpectrumValues();
}

void debugSpectrumValues() {

  for (int i = 0; i < Controls::spectrumCount; i++) {
    if (controls.spectrumRaw[i] < 10) {
      Serial.print("    "); Serial.print(controls.spectrumRaw[i]);
    } else if (controls.spectrumRaw[i] < 100) {
      Serial.print("   "); Serial.print(controls.spectrumRaw[i]);
    } else if (controls.spectrumRaw[i] < 1000) {
      Serial.print("  "); Serial.print(controls.spectrumRaw[i]);
    } else {
      Serial.print(" "); Serial.print(controls.spectrumRaw[i]);
    }
  }
  Serial.print(" * ");

  for (int i = 0; i < Controls::spectrumCount; i++) {
    if (controls.spectrum[i] < 10) {
      Serial.print("   ");
      Serial.print(controls.spectrum[i]);
    } else if (controls.spectrum[i] < 100) {
      Serial.print("  ");
      Serial.print(controls.spectrum[i]);
    } else {
      Serial.print(" ");
      Serial.print(controls.spectrum[i]);
    }
  }
  Serial.println();
}

void updateMasterBrightness() {
  uint8_t masterBrightness = map(analogRead(brightnessPotPin), 0, 1023, 0, 255);
  FastLED.setBrightness(masterBrightness);
//  Serial.print("brightness pot = "); Serial.println(masterBrightness);
}

void updateButtonValues() {
  if (controlButtonDebounce == 0 && digitalRead(controlButtonPin) == LOW) {
    controlButtonDebounce = debounceCount;
    controls.button = true;
  } else if (controlButtonDebounce > 0) {
    controlButtonDebounce--;
    controls.button = false;
  }
  if (upButtonDebounce == 0 && digitalRead(upButtonPin) == LOW) {
    upButtonDebounce = debounceCount;
    upButton = true;
  } else if (upButtonDebounce > 0) {
    upButtonDebounce--;
    upButton = false;
  }
  if (downButtonDebounce == 0 && digitalRead(downButtonPin) == LOW) {
    downButtonDebounce = debounceCount;
    downButton = true;
  } else if (downButtonDebounce > 0) {
    downButtonDebounce--;
    downButton = false;
  }

//  Serial.print("control button = "); Serial.println(controls.button);
//  Serial.print("     up button = "); Serial.println(upButton);
//  Serial.print("   down button = "); Serial.println(downButton);
}

void updateCurrentPattern() {
  if (downButton && upButton) {
    // special mode?
  } else if (upButton) {
    effectIndex = effectIndex == effectCount - 1 ? 0 : effectIndex + 1;
  } else if (downButton) {
    effectIndex = effectIndex == 0 ? effectCount - 1 : effectIndex - 1;
  }
}

//void register_marks() {
//  leds[XY(0, 0)] = CRGB::Red;
//  leds[XY(kMatrixWidth - 1, 0)] = CRGB::Green;
//  leds[XY(kMatrixWidth - 1, kMatrixHeight - 1)] = CRGB::Blue;
//  leds[XY(0, kMatrixHeight - 1)] = CRGB::White;
//}

// simple chase test pattern
//  fill_solid(leds, kNumLeds, CRGB::Black);
//  static uint16_t i = 0;
//  leds[i++] = CRGB::White;
//  if (i == kNumLeds) {
//    i = 0;
//  }

// Line rising up from bottom to top
//  fadeToBlackBy(leds, kNumLeds, 16);
//  static uint16_t y = 0;
//
//  for (int x = 0; x < kMatrixWidth; x++) {
//    leds[XY(x, y)] = CRGB::White;
//  }
//  if (++y == kMatrixHeight) {
//    y = 0;
//  }

// Basic MSEQ7 output
//  fadeToBlackBy(leds, kNumLeds, 16);
//
//  for (int y = 0; y < spectrum; y++) {
//    uint8_t vol = map(spectrumValue[y], 0, 1023, 0, 15);
//    for (int x = 0; x < vol; x++) {
//      leds[XY(vol, y)] = CRGB(255, 255, 255);
//    }
//  }

