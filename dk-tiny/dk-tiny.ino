/*
  Joystick Example
  Prints to the Serial Monitor the direction the joystick is moved in (Up, down, left, right)
  The "up" direction is the direction closest to the 5-pin connector.

  Written by: Laveréna Wienclaw for TinyCircuits
  Last Updated: March 2020
*/

#include <Wireling.h>
#include <SX1505.h>             // For interfacing with Joystick and Rotary Switch Wirelings

// Universal TinyCircuits Serial Monitor Config
#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#define SerialComm Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#define SerialComm Serial
#endif

/* * * * * * * * * * Joystick * * * * * * * * * */
#define JS_A_PORT 1
#define JS_B_PORT 2

TinyJoystick joystickA = TinyJoystick(); 
TinyJoystick joystickB = TinyJoystick();

void setup() {
  Wireling.begin();

  /* * * * * * Joystick Init * * * * */
  Wireling.selectPort(JS_B_PORT);
  joystickB.begin();
  Wireling.selectPort(JS_A_PORT); 
  joystickA.begin();
  
  SerialMonitorInterface.begin(9600);
  delay(500);

  SerialComm.begin(9600);
  delay(500);
}

void loop() {
  Wireling.selectPort(JS_A_PORT);
  joystickA.getPosition();
  Wireling.selectPort(JS_B_PORT);
  joystickB.getPosition();

  report();
  delay(100);
}

void report() {
  if(joystickA.up) {
    SerialMonitorInterface.println("A UP (towards connector)");
    SerialComm.println("AU");
  }
  else if(joystickA.down) {
    SerialMonitorInterface.println("A DOWN (away connector)");
    SerialComm.println("AD");
  }
  else if(joystickA.left) {
    SerialMonitorInterface.println("A LEFT (<--)");
    SerialComm.println("AL");
  }
  else if(joystickA.right) {
    SerialMonitorInterface.println("A RIGHT (-->)");
    SerialComm.println("AR");
  }
  else if(joystickA.press) {
    SerialMonitorInterface.println("A PRESSED (center)");
    SerialComm.println("AP");
  }

  if(joystickB.up) {
    SerialMonitorInterface.println("B UP (towards connector)");
    SerialComm.println("BU");
  }
  else if(joystickB.down) {
    SerialMonitorInterface.println("B DOWN (away connector)");
    SerialComm.println("BD");
  }
  else if(joystickB.left) {
    SerialMonitorInterface.println("B LEFT (<--)");
    SerialComm.println("BL");
  }
  else if(joystickB.right) {
    SerialMonitorInterface.println("B RIGHT (-->)");
    SerialComm.println("BR");
  }
  else if(joystickB.press) {
    SerialMonitorInterface.println("B PRESSED (center)");
    SerialComm.println("BP");
  }
  
  SerialMonitorInterface.flush();
  SerialComm.flush();
}
