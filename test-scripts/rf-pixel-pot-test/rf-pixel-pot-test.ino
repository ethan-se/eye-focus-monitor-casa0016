#include <Adafruit_NeoPixel.h>

//neopixel setup
#define PIN 2
#define NUMPIXELS 8
#define DELAYVAL 500

//rangefinder setup
#define trigPin 0
#define echoPin 1

//potentiometer setup
int potPin = A0;
int potVal = 0;

//smoothing variables for rolling average - preventing neopixel flashing
const int numReadings = 30; //defines size of array
int readings[numReadings]; //array to hold recent readings
int readIndex = 0; //tracks position in array
long total = 0; //current running total
long avgDist = 0; //the smoothed, output value for distance from monitor

Adafruit_NeoPixel pixels(NUMPIXELS, PIN); //init neopixel object

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(potPin, INPUT);

  pixels.begin(); //initialise lights

  // reset rolling average data
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  //clear lights on startup
  pixels.clear();

  //clear trigPin on startup
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  //send a trigger pulse for 10microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  //listen for how long it takes for echo pin to recieve pulse, convert to centimeters
  long dur = pulseIn(echoPin, HIGH);
  float dist = 34400*dur/2000000; //distance conversion?

  //SMOOTHING ALGORITHM
  //subtract last reading
  total = total - readings[readIndex];
  //write raw dist val to index
  readings[readIndex] = constrain(dist, 0, 150);
  //add reading to total
  total = total + readings[readIndex];
  //advance readIndex to next array position
  readIndex = readIndex + 1;
  //if we are at the end of the array, wrap back to start
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  //calculate new rolling average distance
  avgDist = total / numReadings;

  int distToPixel = map(avgDist, 0, 70, 0, 8); //calibrate distance to pixels lit
  int pixelsLit = constrain(distToPixel, 0, 8); //ensure number of pixels does not overflow
  
  potVal = analogRead(potPin);
  int colour = map(potVal, 0, 1024, 0, 255); //calibrate pot values to rgb values

  for (int i=0; i<pixelsLit; i++) {
    pixels.setPixelColor(i, 0, colour, 0);
    pixels.show();
  }

  // Serial.print("distance in cm: ");
  // Serial.println(dist);
  // Serial.print("avgDist: ");
  // Serial.println(avgDist);
  // Serial.print("potentiometer: ");
  // Serial.println(potVal);
}

