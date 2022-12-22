#include <Arduino.h>
#include <BleKeyboard.h>

#define BUTTON1 26 
#define BUTTON2 25 
#define BUTTON3 13 

BleKeyboard bleKeyboard(
  "DriveKeys",
  "ShipLift",
  100
);

int button1State = HIGH;
int button2State = HIGH;
int button3State = HIGH;

void setup() {
  bleKeyboard.begin();
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
}

void stateChecker(int button, int *state, uint8_t key){
  int currentButton1State = digitalRead(button);
  if (currentButton1State != *state)
    {
        if (currentButton1State == LOW)
        {
            bleKeyboard.press(key);
        }
        else
        {
            bleKeyboard.release(key);
        }
    }
    *state = currentButton1State;
}

void loop() {
  if(bleKeyboard.isConnected()) {
    stateChecker(BUTTON1, &button1State, KEY_LEFT_CTRL); // SW
    stateChecker(BUTTON2, &button2State, KEY_LEFT_ALT); // SE
    stateChecker(BUTTON3, &button3State, KEY_LEFT_SHIFT); // NE

    // Or, actual keys (for debugging etc)
    // stateChecker(BUTTON1, &button1State, 'a'); // SW
    // stateChecker(BUTTON2, &button2State, 'b'); // SE
    // stateChecker(BUTTON3, &button3State, 'c'); // NE
  }
}
