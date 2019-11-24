#include <Arduino.h>
#include <EEPROM.h>

#include "Adafruit_NeoPixel.h"

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN 7

#define BUTTON_PIN 2

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 277
#define REPEAT 2

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned int led[NUMPIXELS];

unsigned int currentLED = 0;

unsigned int buttonDebounceDelay = 200;
unsigned long buttonLastRead = 0;

unsigned int mode = 1;

unsigned long prevT = 0;

bool set = false;

unsigned int getMode(unsigned int m);
void handleButtonPress();
void off();
void lightsOn1();
void lightsOn2();
void waves(unsigned long t, unsigned int speed, unsigned char size);
void sparkle(unsigned long t, unsigned int deltaT, unsigned int speed, unsigned int duration);
void halloweenLights(unsigned long t, unsigned long period, unsigned int duration);
void lightning(unsigned long t, unsigned int speed, unsigned int duration);
void carousel(unsigned long t, unsigned int speed, unsigned int length);

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  
  Serial.begin(9600); // Trun on debugging

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(255); // Set BRIGHTNESS

  off();

  mode = getMode(EEPROM.read(0));

  Serial.println("All set up");
}

void loop() {
  unsigned long t = millis();
  unsigned int deltaT = t - prevT;

  if (t < 2000) { return; } // Wait a couple of seconds

  while(!strip.canShow());

  switch (mode) {
    case 1:
      if (!set) { lightsOn2(); }
      break;
    case 2:
      if (!set) { lightsOn1(); }
      break;
    case 3:
      waves(t, 15, 40);
      break;
    case 4:
      sparkle(t, deltaT, 5, 300);
      break;
    case 5:
      carousel(t, 80, 6);
      break;
    default:
      if (!set) { off(); }
      break;
  }

  set = true;

  prevT = t;
}

void off() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  for (unsigned char i = 0; i < REPEAT; i++) {
    strip.show();
  }
}

void lightsOn1() {
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i % 12 < 4) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    else {
      strip.setPixelColor(i, strip.ColorHSV(6000, 200, 75));
    }
  }
  
  for (unsigned char i = 0; i < REPEAT; i++) {
    strip.show();
  }
}

void lightsOn2() {
  strip.fill(strip.ColorHSV(6000, 200, 50));
  for (unsigned char i = 0; i < REPEAT; i++) {
    strip.show();
  }
}

void halloweenLights(unsigned long t, unsigned long period, unsigned int duration) {
  if (t % period < duration) {
    lightning(t, 2, 3);
  }
  else {
    strip.fill(strip.ColorHSV(1000, 255, 75));
    strip.show();
  }

  //lightning(t, 2, 3);
}

void lightning(unsigned long t, unsigned int speed, unsigned int duration) {
  if (t % speed != 0) {
    return;
  }
  
  int i = random(0, NUMPIXELS);

  led[i] = duration;

  for (int l = 0; l < NUMPIXELS; l++) {
    if (led[l] > 0) {
      strip.setPixelColor(l, strip.ColorHSV(6000, 200, 255));
      led[l]--;
    }
    else {
      strip.setPixelColor(l, strip.Color(0, 0, 0));
    }
  }

  strip.show();
}

void sparkle(unsigned long t, unsigned int deltaT, unsigned int speed, unsigned int duration) {
  if (t % speed == 0) {
    int i = random(0, NUMPIXELS);
    if (led[i] == 0) {
      led[i] = duration;
      //color[i] = random(2000, 3000);
    }
  }

  for (int l = 0; l < NUMPIXELS; l++) {
    if (led[l] > deltaT) {
      led[l] -= deltaT;
      double d = double(led[l]) / double(duration);
      double m = d * (d - 2.0D) + 1.0D; // Ease-out
      strip.setPixelColor(l, strip.ColorHSV(5000, 235, int(255.0D * m)));
    }
    else {
      led[l] = 0;
      strip.setPixelColor(l, strip.Color(0, 0, 0));
    }
  }
  
  for (unsigned char i = 0; i < REPEAT; i++) {
    strip.show();
  }
}

void waves(unsigned long t, unsigned int speed, unsigned char size) {
  unsigned int start = (speed * t / 1000) % NUMPIXELS;

  for (int i = 0; i < NUMPIXELS; i++) {
    unsigned char b = 50 + 150 * max(0, sin((10.0D / (double)size) * (i - start) / PI));
    strip.setPixelColor(i, strip.ColorHSV(6000, 200, b));
  }
  
  for (unsigned char i = 0; i < REPEAT; i++) {
    strip.show();
  }
}

void carousel(unsigned long t, unsigned int speed, unsigned int length) {
  if (t % speed != 0) {
    return;
  }

  unsigned int s = currentLED;

  for (unsigned int i = 0; i < NUMPIXELS; i++) {
    int v = abs(i - s);

    if (v % (length * 2) < length) {
      strip.setPixelColor(i, strip.ColorHSV(250, 255, 100));
    }
    else {
      strip.setPixelColor(i, strip.ColorHSV(2000, 225, 100));
    }
  }
  
  strip.show();
  
  currentLED = (currentLED + 1) % NUMPIXELS;
}

void handleButtonPress() {
  unsigned long t = millis();

  if (t > buttonLastRead + buttonDebounceDelay) {
    mode = getMode(mode + 1);
    buttonLastRead = t;

    set = false;

    EEPROM.write(0, mode);

    Serial.println(mode);
  }
}

unsigned int getMode(unsigned int m) {
  return m % 7;
}
