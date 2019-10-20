// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN        7 // On Trinket or Gemma, suggest changing this to 1

#define BUTTON_PIN     2

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 275

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int currentLED = 0;

int buttonDebounceDelay = 200;
unsigned long buttonLastRead = 0;

int mode = 1;

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  
  //Serial.begin(9600); // Trun on debugging

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(100); // Set BRIGHTNESS to about 1/5 (max = 255)

  off();
}

void loop() {
  unsigned long t = millis();

  switch (mode) {
    case 1:
      lightsOn2();
      break;
    case 2:
      lightsOn1();
      break;
    case 3:
      carousel(t, 80, 6);
      break;
    default:
      off();
      break;
  }
}

void off() {
  strip.clear();
  strip.show();
}

void lightsOn1() {
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i % 12 < 4) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    else {
      strip.setPixelColor(i, strip.ColorHSV(6000, 200, 200));
    }
  }
  
  strip.show();
}

void lightsOn2() {
  strip.fill(strip.ColorHSV(6000, 200, 150));
  strip.show();
}

void carousel(unsigned long t, int speed, int length) {
  if (t % speed != 0) {
    return;
  }

  int s = currentLED;

  for (int i = 0; i < NUMPIXELS; i++) {
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
    mode = (mode + 1) % 4;
    buttonLastRead = t;
  }
}
