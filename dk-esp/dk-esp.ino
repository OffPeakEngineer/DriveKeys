#include <Arduino.h>
#include <Wire.h>
#include <BleGamepad.h>
#include <Adafruit_LC709203F.h>
#define ARDUINOJSON_USE_DOUBLE 1
#include <ArduinoJson.h>
StaticJsonDocument<128> doc;

#define SerialComm Serial1 // Hardwire
#define SerialMon Serial // USB Serial

Adafruit_LC709203F lc;

BleGamepad bleGamepad(
  "DriveKeys",
  "ShipLift LLC",
  100
);


void setup() {
  SerialMon.begin(9600);
  delay(500);
  SerialComm.begin(9600);
  delay(500);

  bleGamepad.begin();

  SerialMon.println("\nDriveKeys by: ShipLift LLC");

  if (!lc.begin()) {
    SerialMon.println(F("Couldnt find Adafruit LC709203F?\nMake sure a battery is plugged in!"));
    return;
  }

  SerialMon.println(F("Found LC709203F"));
  SerialMon.print("Version: 0x"); SerialMon.println(lc.getICversion(), HEX);

  lc.setThermistorB(3950);
  SerialMon.print("Thermistor B = "); SerialMon.println(lc.getThermistorB());

  lc.setPackSize(LC709203F_APA_3000MAH);

  lc.setAlarmVoltage(3.8);
}

bool buttonSender(String button, uint8_t key, uint8_t key2 = 0){
  if (button[2] == '1')
  {
    bleGamepad.press(key);
    if (key2 != 0)
    {
      bleGamepad.press(key2);
    }
    return true;
  }
  
  if (button[2] == '0')
  {
    bleGamepad.release(key);
    if (key2 != 0)
    {
      bleGamepad.release(key2);
    }
    return false;
  }
}


void loop()
{
  if (SerialComm.available() <= 0) // Check if serial has an incoming button
  {
      return;
  }
  
  deserializeJson(doc, SerialComm);

  if (doc.isNull()) {
    // SerialMon.println("Received nothing.");
    return;
  }
  
  double a, b, c;
  a = (doc["a"].as<double>() * 1.0d);
  b = (doc["b"].as<double>() * 1.0d);
  c = (doc["c"].as<double>() * 1.0d);

  a = abs(a);
  b = abs(b);

  a = a + 5;
  b = b + 1;

  if ( a > 30) {
    a = 30;
  }

  if ( a < 0 ) {
    a = 0;
  }

  if ( b > 30) {
    b = 30;
  }

  if ( b < 0 ) {
    b = 0;
  }

  double x = a / 30.0d; // Convert mag range to percent
  double y = b / 30.0d;

  // x = x * 2 

  // x = (65534 * x) - 32767; // Normalize percent to joy range
  // y = (65534 * y) - 32767;

  x = (30000 * x); // Normalize percent to joy range
  y = 30000 - (30000 * y);

  // SerialMon.printf("%f, %f, %f\n", x, y);
  SerialMon.printf("%f, %f\n", a, b);
  SerialMon.flush();

  if(bleGamepad.isConnected()) {
    bleGamepad.setLeftThumb(x, y);
  }
}
