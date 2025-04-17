//
//


#include <Arduino.h>
#include <BleKeyboard.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// How many internal neopixels do we have? some boards have more than one!
#define NUMPIXELS        1

int X ;           // X-coordinate
int Y ;           // Y-coordinate

struct KeyBinding {
  uint8_t keycode; // HID keycode (e.g., KEY_RETURN, 'A', etc.)
  bool isPressed; 
  uint16_t touchIDs[140];
};

KeyBinding keyBindings[] = {
  { 'A', false, {256, 384, 400}},
  { 'B', false, {}},
  { 'C', false, {}},
};

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_MPR121 cap = Adafruit_MPR121();
BleKeyboard bleKeyboard;

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

// Tracks the millis that a point was touched.
unsigned long keyMatrix[10][14] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int getX(uint16_t touched);
int getY(uint16_t touched);
void drawKeyMatrix(unsigned long keyMatrix[10][14]);
void yield(int delay);
void printTouchLine(uint16_t touched);
bool isKeyPressed(char key);

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial){ true; }

#if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright

  bleKeyboard.begin();
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A, &Wire1)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }

  Serial.println("MPR121 found!");
  Serial.write(27); Serial.print("[?25l");  // ESC[?25l — hide cursor
}

int msgNm = 0;
#define msgHistLen 5
String eventLog[msgHistLen] = {"","","","",""};
String missing = "(__,__)";
unsigned long screenBlank = 0;
bool secondDraw = true;

void loop() {
  // Warmup
  if (secondDraw) {
    secondDraw = !secondDraw;
    Serial.write(27);  // ESC
    Serial.print("[2J"); // Clear screen
  }

  
  {// Keyframe, i.e. full redraw (because otherwise we're just drawing over
   // the last characters or 'overwrite mode' (the opposite of 'insert mode').
   // Which is normally great, because it prevents a flicker, but the frame will
   // eventually get dirty and need a full redraw. Hence the blank.
      if (millis() > screenBlank + 10000) {
      screenBlank = millis();
      Serial.write(27);  // ESC
      Serial.print("[2J"); // Clear screen

      String missing = "(__,__)"; // Good time to reset missing too
    }
  }
  

  // set color to red
  pixels.fill(0xFF0000);
  pixels.show();

  Serial.write(27); Serial.print("[?25l");  // ESC[?25l — hide cursor
  Serial.write(27); Serial.print("[H"); // Move cursor to home (no clear)
  
  {// Draws the last frame, we calculate one frame behind.
    // or, every frame drawn was processed durring the last frames display.
    drawKeyMatrix(keyMatrix);
    // Above:      "  +-------------------------------+
    Serial.println("                  Missing: " + missing);
    Serial.println("  Event Log:");
    for (int i = 0; i < msgHistLen; i++) {
      if (msgNm-i < 0) {
        Serial.println("");
        continue;
      }
      Serial.println("   " + eventLog[(msgNm-i) % msgHistLen] + "      ");
    }
    Serial.println("");
    Serial.println("               Touch ID: #" + String(currtouched)+"   ");
    Serial.print("          "); printTouchLine(currtouched);
  }

  // Continually check the touchpad until a difference is found.
  unsigned long framecap = millis();
  while(currtouched == lasttouched) {
    currtouched = cap.touched();
    if (millis() > framecap + 2000) {
      break;
    }
  }

  int x = getX(currtouched);
  int y = getY(currtouched);
  if (x > 0 && y > 0) {
    for (uint8_t i=0; i<12; i++) {
      // it if *is* touched and *wasnt* touched before, alert!
      if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
        int from = keyMatrix[x][y];
        if (from == 0) {
          keyMatrix[x][y] = millis();

          msgNm++;
          eventLog[msgNm % msgHistLen] = String(msgNm) + ". (" + String(x) + "," + String(y) + ") --> pressed.";

          if(bleKeyboard.isConnected()) {
              // bleKeyboard.press(key);
          }
        }
      }

      // if it *was* touched and now *isnt*, alert!
      if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
        if (keyMatrix[x][y] != 0) {
          // keyMatrix[x][y] = 0;
          // Normally this would be a reset,
          //    but I'm treating it as "any detected activity"
          keyMatrix[x][y] = millis();
          msgNm++;
          eventLog[msgNm % msgHistLen] = String(msgNm) + ". (" + String(x) + "," + String(y) + ") --> released.";
          if(bleKeyboard.isConnected()) {
            if (getKeyForTouchIDisKeyPressed) 
              // bleKeyboard.press(key);
          }
        }
      }
    }
  } else {
    if (
         ( ((!x) || (!y)) && missing == "(__,__)" )
      || ( missing != "(__,__)" )
    ) {
       missing = "(";
      missing += (!x)? "__":((x<10)?"_":"")+String(x);
      missing += ",";
      missing += (!y)? "__":((y<10)?"_":"")+String(y);
      missing += ")";
    }
  }

  // reset our state
  lasttouched = currtouched;

  // turn off
  pixels.fill(0x000000);
  pixels.show();
}

void yield(int ms) {
  yield();
  delay(ms);
}


bool isKeyPressed(char key) {
  for (size_t i = 0; i < sizeof(keyBindings) / sizeof(KeyBinding); i++) {
    if (keyBindings[i].keycode == key) {
      return keyBindings[i].isPressed;
    }
  }
  return false; // Not found or not pressed
}


char getKeyForTouchID(uint16_t id) {
  for (size_t i = 0; i < sizeof(keyBindings) / sizeof(KeyBinding); i++) {
    const KeyBinding &binding = keyBindings[i];

    for (size_t j = 0; j < MAX_TOUCH_IDS; j++) {
      uint16_t tid = binding.touchIDs[j];

       if (tid == 0xFFFF) break;

      if (tid == id) {
        return binding.keycode;
      }
    }
  }
  return '\0'; // No match
}

void drawKeyMatrix(unsigned long keyMatrix[10][14]) {
  Serial.println(F("    1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9"));
  Serial.println(F("  +-----------------------------------+"));
  for (int col = 1; col < 14; col++) {
    if (col < 10) {
      Serial.print(F(" ")); // Padding
    }
    Serial.print(String(col));
    Serial.print(F("|"));
    for (int row = 1; row < 10; row++) {
      int rowSelector = 10 - row;
      int colSelector = 14 - col;

      if (keyMatrix[rowSelector][colSelector]) {
        Serial.print(F("███"));
        if (keyMatrix[rowSelector][colSelector] < millis() - 500) {
          keyMatrix[rowSelector][colSelector] = 0; // cell timeout
        }
      } else {
        Serial.print(F("   "));
      }
      if (row < 9) {
        Serial.print(F("⦙"));
      }
    }
    Serial.println(F("|"));
    Serial.write(27); Serial.print("[?25l");  // ESC[?25l — hide cursor
  }
  Serial.println(F("  +-----------------------------------+"));
  Serial.write(27); Serial.print("[?25l");  // ESC[?25l — hide cursor
}

void printTouchLine(uint16_t touched) {
  for (int i = 0; i < 12; i++) {
    if (touched & (1 << i)) {
      Serial.print("⊞");
    } else {
      Serial.print("□");
    }
  }
  Serial.println(); // Move to the next line
}

// int 0 - 9
int getX(uint16_t touched) {
  int a = bitRead(touched, 0);
  int b = bitRead(touched, 1);
  int c = bitRead(touched, 2);
  int d = bitRead(touched, 3);
  int e = bitRead(touched, 4);

  if (touched > 0) {
    if (a == 1 && b != 1) return 1;               // Electrode 0
    if (a != 1 && b == 1 && c != 1) return 3;     // Electrode 1
    if (b != 1 && c == 1 && d != 1) return 5;     // Electrode 2
    if (c != 1 && d == 1 && e != 1) return 7;     // Electrode 3
    if (d != 1 && e == 1) return 9;               // Electrode 4

    if (a == 1 && b == 1) return 2;               // Between 0 and 1
    if (b == 1 && c == 1) return 4;               // Between 1 and 2
    if (c == 1 && d == 1) return 6;               // Between 2 and 3
    if (d == 1 && e == 1) return 8;               // Between 3 and 4
  }

  return 0; // No touch or unrecognized pattern
}

// int 0 - 13
int getY(uint16_t touched) {
  int a = bitRead(touched, 5);
  int b = bitRead(touched, 6);
  int c = bitRead(touched, 7);
  int d = bitRead(touched, 8);
  int e = bitRead(touched, 9);
  int f = bitRead(touched, 10);
  int g = bitRead(touched, 11);

  if (touched > 0) {
    if (a == 1 && b != 1) return 1;               // Electrode 5
    if (a != 1 && b == 1 && c != 1) return 3;     // Electrode 6
    if (b != 1 && c == 1 && d != 1) return 5;     // Electrode 7
    if (c != 1 && d == 1 && e != 1) return 7;     // Electrode 8
    if (d != 1 && e == 1 && f != 1) return 9;     // Electrode 9
    if (e != 1 && f == 1 && g != 1) return 11;    // Electrode 10
    if (f != 1 && g == 1) return 13;              // Electrode 11

    if (a == 1 && b == 1) return 2;               // Between 5 and 6
    if (b == 1 && c == 1) return 4;               // Between 6 and 7
    if (c == 1 && d == 1) return 6;               // Between 7 and 8
    if (d == 1 && e == 1) return 8;               // Between 8 and 9
    if (e == 1 && f == 1) return 10;              // Between 9 and 10
    if (f == 1 && g == 1) return 12;              // Between 10 and 11
  }

  return 0; // No touch or unrecognized pattern
}

