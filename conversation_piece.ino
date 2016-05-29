/***************************************************

This is Converstion Piece

 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define STRIPPIN 9
#define PIXELCOUNT 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELCOUNT, STRIPPIN, NEO_GRB + NEO_KHZ800);
int stripBrightness = 0;

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer =
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

char * audioFiles[20] = {
  "/audio/track001.mp3",
  "/audio/test002.mp3",
  "/audio/test003.mp3",
};
const int audioFileCount = 3;
int currentTrack = 0;

//
// Setup for button
//
const int buttonPin = 2;
int buttonState = 0;
int lastButtonState = 0;
const int buttonLightPin = 8;

//
// Photocell
//
const int photocellPin = 0; // analog 0
int photocellReading;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(buttonLightPin, OUTPUT);

  Serial.begin(9600);

  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  // turn the button light on
  turnButtonLightOn();

  SD.begin(CARDCS);    // initialise the SD card

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(0,10000);

  // Neo pixel strip initialization
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}



void checkPhotocellState() {
  photocellReading = analogRead(photocellPin);

  if ( photocellReading < 400 ) {
    stripBrightness = 255;
  } else {
    stripBrightness = 0;
  }
  // stripBrightness = map(photocellReading, 0, 1024, 255, 0);

  for(uint16_t ii=0; ii<strip.numPixels(); ii++) {
    strip.setPixelColor(ii, strip.Color(stripBrightness, 0, 0));
  }
  strip.show();
  // Serial.println(photocellReading);
}

void checkButtonState() {

  buttonState = digitalRead(buttonPin);

  if ( buttonState != lastButtonState ) {
    if ( buttonState ) {
      Serial.println("pushed");
      playRandomFile();
    }
    delay(50);
  }

  lastButtonState = buttonState;
}

void playRandomFile() {

  turnButtonLightOff();
  musicPlayer.playFullFile(audioFiles[currentTrack]);
  turnButtonLightOn();

  currentTrack++;
  if ( currentTrack >= audioFileCount ) {
    currentTrack = 0;
  }

}

void turnButtonLightOn() {
  digitalWrite(buttonLightPin, HIGH);
  Serial.println("button light ON");
}

void turnButtonLightOff() {
  digitalWrite(buttonLightPin, LOW);
  Serial.println("button light OFF");
}





void loop() {

  checkButtonState();

  checkPhotocellState();

}






// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
