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

#define STRIPPIN 10
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
  "/audio/q-116.mp3",
  "/audio/q-109.mp3",
  "/audio/q-072.mp3",
  "/audio/q-011.mp3",
  "/audio/q-056.mp3",
  "/audio/q-063.mp3",
  "/audio/q-075.mp3",
  "/audio/q-126.mp3",
  "/audio/q-032.mp3",
  "/audio/q-106.mp3",
  "/audio/q-127.mp3",
  "/audio/q-090.mp3",
  "/audio/q-045.mp3",
  "/audio/q-121.mp3",
  "/audio/q-082.mp3",
  "/audio/q-059.mp3",
  "/audio/q-091.mp3",
  "/audio/q-069.mp3",
  "/audio/q-030.mp3",
  "/audio/q-094.mp3",
  "/audio/q-087.mp3",
  "/audio/q-065.mp3",
  "/audio/cuddlef.mp3",
  "/audio/q-031.mp3",
  "/audio/q-081.mp3",
  "/audio/q-067.mp3",
  "/audio/q-107.mp3",
  "/audio/q-128.mp3",
  "/audio/q-071.mp3",
  "/audio/q-097.mp3",
  "/audio/q-103.mp3",
  "/audio/q-105.mp3",
  "/audio/q-036.mp3",
  "/audio/q-046.mp3",
  "/audio/q-047.mp3",
  "/audio/q-051.mp3",
  "/audio/q-035.mp3",
  "/audio/q-119.mp3",
  "/audio/q-077.mp3",
  "/audio/q-104.mp3",
  "/audio/q-022.mp3",
  "/audio/q-118.mp3",
  "/audio/q-007.mp3",
  "/audio/q-023.mp3",
  "/audio/q-064.mp3",
  "/audio/q-114.mp3",
  "/audio/q-053.mp3",
  "/audio/q-016.mp3",
  "/audio/q-028.mp3",
  "/audio/q-061.mp3",
  "/audio/q-021.mp3",
  "/audio/track001.mp3",
  "/audio/q-040.mp3",
  "/audio/q-017.mp3",
  "/audio/q-033.mp3",
  "/audio/q-095.mp3",
  "/audio/q-070.mp3",
  "/audio/q-085.mp3",
  "/audio/q-131.mp3",
  "/audio/q-076.mp3",
  "/audio/q-044.mp3",
  "/audio/q-086.mp3",
  "/audio/q-078.mp3",
  "/audio/q-096.mp3",
  "/audio/q-108.mp3",
  "/audio/q-019.mp3",
  "/audio/q-043.mp3",
  "/audio/q-015.mp3",
  "/audio/q-003.mp3",
  "/audio/q-008.mp3",
  "/audio/q-050.mp3",
  "/audio/q-115.mp3",
  "/audio/q-018.mp3",
  "/audio/smuggle.mp3",
  "/audio/q-073.mp3",
  "/audio/q-083.mp3",
  "/audio/q-054.mp3",
  "/audio/q-066.mp3",
  "/audio/namespo.mp3",
  "/audio/q-084.mp3",
  "/audio/q-041.mp3",
  "/audio/evoluti.mp3",
  "/audio/q-010.mp3",
  "/audio/badthi.mp3",
  "/audio/q-002.mp3",
  "/audio/q-111.mp3",
  "/audio/q-009.mp3",
  "/audio/q-099.mp3",
};
const int audioFileCount = 139;
int currentTrack = 0;

//
// Setup for buttons
//
const int playButtonPin = 2;
int playButtonState = 0;
int lastPlayButtonState = 0;
const int playButtonLightPin = 8;

const int replayButtonPin = 5;
int replayButtonState = 0;
int lastReplayButtonState = 0;
const int replayButtonLightPin = 9;

//
// Photocell
//
const int photocellPin = 0; // analog 0
int photocellReading;

void setup() {
  pinMode(photocellPin, INPUT);
  pinMode(playButtonPin, INPUT);
  pinMode(playButtonLightPin, OUTPUT);
  pinMode(replayButtonLightPin, OUTPUT);

  Serial.begin(9600);

  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  // turn the button light on
  turnButtonLightsOn();

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

void checkReplayButtonState() {

  replayButtonState = digitalRead(replayButtonPin);
  Serial.println(replayButtonState);

  if ( replayButtonState != lastReplayButtonState ) {
    if ( replayButtonState ) {
      Serial.println("pushed");
      playCurrentFile();
    }
    delay(50);
  }

  lastReplayButtonState = replayButtonState;
}

void checkPlayButtonState() {

  playButtonState = digitalRead(playButtonPin);
  // Serial.println(playButtonState);

  if ( playButtonState != lastPlayButtonState ) {
    if ( playButtonState ) {
      Serial.println("pushed");
      playNextFile();
    }
    delay(50);
  }

  lastPlayButtonState = playButtonState;
}

void playNextFile() {

  currentTrack++;
  if ( currentTrack >= audioFileCount ) {
    currentTrack = 0;
  }

  playCurrentFile();

}

void playCurrentFile() {
  turnButtonLightsOff();
  musicPlayer.playFullFile(audioFiles[currentTrack]);
  turnButtonLightsOn();
}

void turnButtonLightsOn() {
  digitalWrite(playButtonLightPin, HIGH);
  digitalWrite(replayButtonLightPin, HIGH);
  Serial.println("button light ON");
}

void turnButtonLightsOff() {
  digitalWrite(playButtonLightPin, LOW);
  digitalWrite(replayButtonLightPin, LOW);
  Serial.println("button light OFF");
}





void loop() {

  checkPlayButtonState();

  checkReplayButtonState();

  checkPhotocellState();

//  rainbowCycle(50);

}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
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
