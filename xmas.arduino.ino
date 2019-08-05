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
#define NUMPIXELS 150 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long led[NUMPIXELS];
int color[NUMPIXELS];
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
      carousel(t, 80, 6);
      break;
    case 2:
      lightning(t, 2, 3);
      break;
    case 3:
      sparkle(t, 2, 250);
      break;
    default:
      off();
      break;
  }
}

void off() {
  for (int i = 0; i < NUMPIXELS; i++) {
    led[i] = 0;
  }

  strip.clear();
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

void lightning(unsigned long t, int speed, int duration) {
  if (t % speed != 0) {
    return;
  }
  
  int i = random(0, NUMPIXELS);

  led[i] = duration;

  for (int l = 0; l < NUMPIXELS; l++) {
    if (led[l] > 0) {
      strip.setPixelColor(l, strip.ColorHSV(2000, 225, 255));
      led[l]--;
    }
    else {
      strip.setPixelColor(l, strip.Color(0, 0, 0));
    }
  }

  strip.show();
}

void sparkle(unsigned long t, int speed, int duration) {
  if (t % speed == 0) {
    int i = random(0, NUMPIXELS);
    if (led[i] == 0) {
      led[i] = t;
      color[i] = random(2000, 3000);
    }
  }

  for (int l = 0; l < NUMPIXELS; l++) {
    if (led[l] + duration > t) {
      int d = int(t - led[l]);
      double m = double(d) / double(duration);
      strip.setPixelColor(l, strip.ColorHSV(color[l], 225, int(255 * m)));
    }
    else {
      led[l] = 0;
      strip.setPixelColor(l, strip.Color(0, 0, 0));
    }
  }
  
  strip.show();
}

void handleButtonPress() {
  unsigned long t = millis();

  if (t > buttonLastRead + buttonDebounceDelay) {
    mode = (mode + 1) % 4;
    buttonLastRead = t;
  }
}
