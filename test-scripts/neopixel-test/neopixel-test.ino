#include <Adafruit_NeoPixel.h>

#define PIN 2
#define NUMPIXELS 8
#define DELAYVAL 500

Adafruit_NeoPixel pixels(NUMPIXELS, PIN); //init neopixel object

void setup() {
  pixels.begin(); //turn all lights off on startup
}

void loop() {
  pixels.clear();
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, 2, 0, 2);
    pixels.show();
    delay(DELAYVAL);
  }
}