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

void buttonSender(String button, uint8_t key){
  if (button[2] == '1')
  {
    bleKeyboard.press(key);
    return;
  }
  
  if (button[2] == '0')
  {
    bleKeyboard.release(key);
    return;
  }
}

void loop()
{
  if (SerialComm.available() <= 0) // Check if serial has an incoming button
  {
      return;
  }

  String button = SerialComm.readStringUntil('\n'); // Read the button
  SerialMon.println(button);
  SerialMon.flush();

  if(bleKeyboard.isConnected()) {
    // bleKeyboard.setBatteryLevel(lc.cellPercent());

    if (button[0] == 'A')
    {
      switch (button[1])
      {
      case 'U':
        buttonSender(button, KEY_UP_ARROW);
        break;
      case 'D':
        buttonSender(button, KEY_DOWN_ARROW);
        break;
      case 'L':
        buttonSender(button, KEY_LEFT_ARROW);
        break;
      case 'R':
        buttonSender(button, KEY_RIGHT_ARROW);
        break;
      case 'P':
        buttonSender(button, KEY_RETURN);
        break;
      default:
        break;
      }
    }
    else if (button[0] == 'B') {
      switch (button[1])
      {
      case 'U':
        buttonSender(button, 'w');
        break;
      case 'D':
        buttonSender(button, 's');
        break;
      case 'L':
        buttonSender(button, 'a');
        break;
      case 'R':
        buttonSender(button, 'd');
        break;
      case 'P':
        buttonSender(button, 'q');
        break;
      default:
        break;
      }
    }
  }
}
