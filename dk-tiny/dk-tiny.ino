#include <Wireling.h>
#include <Wire.h>
#include "TinyCircuits_A1454.h" // For the sensor

#define ARDUINOJSON_USE_DOUBLE 1
#include <ArduinoJson.h>
StaticJsonDocument<128> doc;

// Universal TinyCircuits Serial Monitor Config
#if defined (ARDUINO_ARCH_AVR)
#define SerialMon Serial
#define SerialComm Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMon SerialUSB
#define SerialComm Serial
#endif

#define HALL_A_PORT 0
#define HALL_B_PORT 1
#define HALL_C_PORT 2

// A sensor object, the methods will be used to interface with the A1454
TinyCircuits_A1454 hallA = TinyCircuits_A1454();
TinyCircuits_A1454 hallB = TinyCircuits_A1454();
TinyCircuits_A1454 hallC = TinyCircuits_A1454();

double magA = 0,
     magB = 0,
     magC = 0;

void setup() {
  Wireling.begin();

  Wireling.selectPort(HALL_A_PORT);
  hallA.begin();
  //  hall.wake(); 
  Wireling.selectPort(HALL_B_PORT); 
  hallB.begin();
  Wireling.selectPort(HALL_C_PORT); 
  hallC.begin();

  SerialMon.begin(9600);
  delay(500);

  SerialComm.begin(9600);
  delay(500);
}

void loop() {
  Wireling.selectPort(HALL_A_PORT);
  magA = hallA.readMag();

  Wireling.selectPort(HALL_B_PORT);
  magB = hallB.readMag();

  Wireling.selectPort(HALL_C_PORT);
  magC = hallC.readMag();

  report();
}


#define AVG_COUNT 5
double magA_avg[AVG_COUNT];
double magB_avg[AVG_COUNT];
double magC_avg[AVG_COUNT];

uint8_t i = 0;

void report() {
  magA_avg[i] = (double)magA / 4.0/ 10.0;
  magB_avg[i] = (double)magB / 4.0/ 10.0;
  magC_avg[i] = (double)magC / 4.0/ 10.0;

  magA = (magA_avg[0] + magA_avg[1] + magA_avg[2] + magA_avg[3] + magA_avg[4]) / (double)AVG_COUNT;
  magB = (magB_avg[0] + magB_avg[1] + magB_avg[2] + magB_avg[3] + magB_avg[4]) / (double)AVG_COUNT;
  magC = (magC_avg[0] + magC_avg[1] + magC_avg[2] + magC_avg[3] + magC_avg[4]) / (double)AVG_COUNT;

  doc.clear();
  doc["a"] = (double)magA;
  doc["b"] = (double)magB;
  doc["c"] = (double)magC;

  serializeJson(doc, SerialMon);
  SerialMon.println();
  SerialMon.flush();

  serializeJson(doc, SerialComm);
  SerialComm.println();
  SerialComm.flush();

  if (i < AVG_COUNT - 1) {
    i++;
  } else {
    i = 0;
  }
}
