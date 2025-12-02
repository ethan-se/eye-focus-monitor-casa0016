#include <Adafruit_NeoPixel.h>

//neopixel setup
#define PIN 5
#define NUMPIXELS 8
#define DELAYVAL 500
int tol = 20; //defines window of tolerance for how far/close someone is to the screen

byte colours[][3] = { //array of colour options, corresponding to each screen size option
  {2, 2, 0},      //index 0, yellow
  {3, 1, 0},       //index 1, orange
  {0, 2, 2},      //index 2, cyan
  {0, 0, 4},        //index 3, blue
  {2, 0, 2},      //index 4, magenta
  {1, 1, 1}     //index 5, white
};

//rangefinder setup
#define trigPin 3
#define echoPin 4

//POTENTIOMETER SETUP
int potPin = A1;
int potVal = 0;
//screen size select setup
int screenOptions[] = {14, 17, 21, 24, 27, 30}; //define supported sizes
int numOptions = sizeof(screenOptions) / sizeof(screenOptions[0]); //calculate length of array by using memory size of identically sized items
int currentScreenIndex = 0; //initialise screen selection option tracker
int lastPotVal = 0; //monitor last position of slider

//smoothing variables for rolling average - preventing neopixel flashing
const int numReadings = 20; //defines size of array
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

  //SCREEN SIZE SELECT
    potVal = analogRead(potPin); //read current slider position
    if (abs(potVal - lastPotVal) > 10) { //if slider has moved significantly (to prevent intermitted size changing from passive value fluctuation)
      int tempIndex = map(potVal, 0, 1023, 0, numOptions); //remap slider range to screen size option range
      tempIndex = constrain(tempIndex, 0, numOptions -1); //constrain to prevent accessing index vals that don't exist
      currentScreenIndex = tempIndex; //save index position from remapping calcs in above "if statement"
      lastPotVal = potVal; //update last known slider position
    }
    int selectedScreenSize = screenOptions[currentScreenIndex]; //set screen size based on slider position
    //set target distance and tolerance thresholds
    int targetDist = round(selectedScreenSize * 2.54 * 1.2); //factor of 2.54 used to convert inches to cm. factor of 1.2 used based on industry best practice
    int lowerBound = targetDist - tol;
    int upperBound = targetDist + tol;
  //OBTAIN DISTANCE FROM RF
    //clear trigPin on startup
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);

    //send a trigger pulse for 10microseconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
    
    //listen for how long it takes for echo pin to recieve pulse, convert to centimeters
      long dur = pulseIn(echoPin, HIGH);
      float dist = 34400*dur/2000000; //distance conversion to cm
  //SMOOTHING ALGORITHM
    //subtract last reading
    total = total - readings[readIndex];
    //write raw dist val to index
    readings[readIndex] = constrain(dist, lowerBound, upperBound); //distance capped to just get the thresholds we need
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
  //CONVERT DISTANCE TO # OF LEDS LIT
    //translate distance measured to number of LEDs to be lit
    int distToPixel = map(avgDist, lowerBound, upperBound, 0, 8); //LEDs display how much close or far you are to the target distance
    int pixelsLit = constrain(distToPixel, 0, 8); //ensure number of pixels does not overflow
    //retrieve colours from colour array according to mode selected
      byte r = colours[currentScreenIndex][0];
      byte g = colours[currentScreenIndex][1];
      byte b = colours[currentScreenIndex][2];
  //LIGHT UP LEDS
  for (int i=0; i<pixelsLit; i++) {
    if (i == 3 || i == 4) { //make middle LEDs green, telling user they are sat at the optimal distance
      pixels.setPixelColor(i, 0, 2, 0);
    }
    else {                  //set colours to values that match the selected screen size
      pixels.setPixelColor(i, r, g, b);
    }
  }
  pixels.show();

  //DEBUGGING
    Serial.print("selectedScreenSize: ");
    Serial.println(selectedScreenSize);
    Serial.print("avgDist: ");
    Serial.println(avgDist);
    Serial.print("pixelsLit: ");
    Serial.println(pixelsLit);
}

