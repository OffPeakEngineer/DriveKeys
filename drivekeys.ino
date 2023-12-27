
#include "Keyboard.h"


/**
*      DriveKey Physical Layout
*
*    +---------------------+
*    | A | B | C |         |
*    | D | E |             |
*    | F |     OOOO        |
*    |---+    OOOOOO       |
*    |       OOOOOOOO      |
*    |        OOOOOO       |
*    |         OOOO        |
*    |...
*
**/

// Aesthetics
#define TAP_MS 20 // When a tap becomes a hold in milliseconds

// MAX_KEYS must be larger than the highest IO pin on the board used for keymapping
// In this case it's digital pin 13. 20 > 13, so MAX_KEYS of 20 is fine.
// 13 would also be fine, but this memory is cheap, and this project simple.

#define MAX_KEYS 20
static char KEY_MAP[MAX_KEYS];
static bool STATE_MAP[MAX_KEYS];

// Maps the diagram from above to the physical/electrically wired pins.
// This names will be used extensively throughtout this code. Know them.
#define KEY_A  13
#define KEY_B  12
#define KEY_C  11
#define KEY_D  10
#define KEY_E  9
#define KEY_F  6

void setup() {
  // Assign each key to produce a character output.
  KEY_MAP[KEY_A] = 'a';
  KEY_MAP[KEY_B] = 'b';
  KEY_MAP[KEY_C] = 'c';
  KEY_MAP[KEY_D] = 'd';
  KEY_MAP[KEY_E] = 'e';
  KEY_MAP[KEY_F] = 'f';

  // Set the initial state of all keys
  // This map will be used to:
  //  * Know when a key is presses
  //  * Track when it is released
  //  * Know if it's being held
  //  * Know if combos are being pressed
  //  * Know the current state of any one key
  //  * And I think that's it.
  //

  STATE_MAP[KEY_A] = HIGH;
  STATE_MAP[KEY_B] = HIGH;
  STATE_MAP[KEY_C] = HIGH;
  STATE_MAP[KEY_D] = HIGH;
  STATE_MAP[KEY_E] = HIGH;
  STATE_MAP[KEY_F] = HIGH;

  // Since the design has no resistors, we use INPUT_PULLUP for us, HIGH means no-pressing
  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);
  pinMode(KEY_C, INPUT_PULLUP);
  pinMode(KEY_D, INPUT_PULLUP);
  pinMode(KEY_E, INPUT_PULLUP);
  pinMode(KEY_F, INPUT_PULLUP);

  Keyboard.begin();
  Serial.begin(9600);
  // delay(5000);
  Serial.println("Begin!");
}

void stateCheck(int incomingKey) {
  bool incoming = digitalRead(incomingKey);

  if (incoming == STATE_MAP[incomingKey]) {
    return;
  }

  if (incoming == LOW) {
    Serial.print("X ");
    Serial.println(KEY_MAP[incomingKey]);
    Keyboard.press(KEY_MAP[incomingKey]);
  } else {
    Serial.print("O ");
    Serial.println(KEY_MAP[incomingKey]);
    Keyboard.release(KEY_MAP[incomingKey]);
  }
    
  STATE_MAP[incomingKey] = incoming;
}

void loop() {
  stateCheck(KEY_A);
  stateCheck(KEY_B);
  stateCheck(KEY_C);
  stateCheck(KEY_D);
  stateCheck(KEY_E);
  stateCheck(KEY_F);
  delay(TAP_MS);
}
