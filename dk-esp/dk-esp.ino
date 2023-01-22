#include <Arduino.h>
#include <Wire.h>
#include <BleKeyboard.h>
#include <Adafruit_LC709203F.h>

#define SerialComm Serial1
#define SerialMon Serial

Adafruit_LC709203F lc;

BleKeyboard bleKeyboard(
  "DriveKeys",
  "ShipLift LLC",
  100
);

void setup() {
  SerialMon.begin(9600);
  delay(500);
  SerialComm.begin(9600);
  delay(500);

  bleKeyboard.begin();

  SerialMon.println("\nAdafruit LC709203F demo");

  if (!lc.begin()) {
    SerialMon.println(F("Couldnt find Adafruit LC709203F?\nMake sure a battery is plugged in!"));
  }

  SerialMon.println(F("Found LC709203F"));
  SerialMon.print("Version: 0x"); SerialMon.println(lc.getICversion(), HEX);

  lc.setThermistorB(3950);
  SerialMon.print("Thermistor B = "); SerialMon.println(lc.getThermistorB());

  lc.setPackSize(LC709203F_APA_3000MAH);

  lc.setAlarmVoltage(3.8);
}

void stateChecker(int button, int *state, uint8_t key){
  int currentButtonState = digitalRead(button);
  if (currentButtonState != *state)
    {
        if (currentButtonState == LOW)
        {
            bleKeyboard.press(key);
        }
        else
        {
            bleKeyboard.release(key);
        }
    }
    *state = currentButtonState;
}

void loop()
{
  if (SerialComm.available() <= 0) // Check if serial has an incoming command
  {
      return;
  }

  SerialMon.println(SerialComm.readString());

  // if(bleKeyboard.isConnected()) {
  //   bleKeyboard.setBatteryLevel(lc.cellPercent());

  //   stateChecker(BUTTON1, &button1State, KEY_LEFT_CTRL); // SW
  //   stateChecker(BUTTON2, &button2State, KEY_LEFT_ALT); // SE
  //   stateChecker(BUTTON3, &button3State, KEY_LEFT_SHIFT); // NE

  //   // Or, actual keys (for debugging etc)
  //   // stateChecker(BUTTON1, &button1State, 'a'); // SW
  //   // stateChecker(BUTTON2, &button2State, 'b'); // SE
  //   // stateChecker(BUTTON3, &button3State, 'c'); // NE
  //   delay(10);
  // }
}
