#include <Wireling.h>
#include <SX1505.h> // For interfacing with Joystick and Rotary Switch Wirelings

// Universal TinyCircuits Serial Monitor Config
#if defined (ARDUINO_ARCH_AVR)
#define SerialMon Serial
#define SerialComm Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMon SerialUSB
#define SerialComm Serial
#endif

#define JS_A_PORT 1
#define JS_B_PORT 2

TinyJoystick joystickA = TinyJoystick(); 
TinyJoystick joystickB = TinyJoystick();

bool joyAU = false,
      joyAD = false,
      joyAL = false,
      joyAR = false,
      joyAP = false,
      joyBU = false,
      joyBD = false,
      joyBL = false,
      joyBR = false,
      joyBP = false;

void setup() {
  Wireling.begin();

  Wireling.selectPort(JS_B_PORT);
  joystickB.begin();
  Wireling.selectPort(JS_A_PORT); 
  joystickA.begin();
  
  SerialMon.begin(9600);
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
  if (joyAU != joystickA.up) {
    joyAU = joystickA.up;
    SerialMon.print("A UP (towards connector)");
    SerialMon.println(joyAU);
    SerialComm.print("AU");
    SerialComm.println(joyAU);
  }
  else if(joyAD != joystickA.down) {
    joyAD = joystickA.down;
    SerialMon.print("A DOWN (away connector)");
    SerialMon.println(joyAD);
    SerialComm.print("AD");
    SerialComm.println(joyAD);
  }
  else if(joyAL != joystickA.left) {
    joyAL = joystickA.left;
    SerialMon.print("A LEFT (<--)");
    SerialMon.println(joyAL);
    SerialComm.print("AL");
    SerialComm.println(joyAL);
  }
  else if(joyAR != joystickA.right) {
    joyAR = joystickA.right;
    SerialMon.print("A RIGHT (-->)");
    SerialMon.println(joyAR);
    SerialComm.print("AR");
    SerialComm.println(joyAR);
  }
  else if(joyAP != joystickA.press) {
    joyAP = joystickA.press;
    SerialMon.print("A PRESSED (center)");
    SerialMon.println(joyAP);
    SerialComm.print("AP");
    SerialComm.println(joyAP);
  }

  if (joyBU != joystickB.up) {
    joyBU = joystickB.up;
    SerialMon.print("B UP (towards connector)");
    SerialMon.println(joyBU);
    SerialComm.print("BU");
    SerialComm.println(joyBU);
  }
  else if(joyBD != joystickB.down) {
    joyBD = joystickB.down;
    SerialMon.print("B DOWN (away connector)");
    SerialMon.println(joyBD);
    SerialComm.print("BD");
    SerialComm.println(joyBD);
  }
  else if(joyBL != joystickB.left) {
    joyBL = joystickB.left;
    SerialMon.print("B LEFT (<--)");
    SerialMon.println(joyBL);
    SerialComm.print("BL");
    SerialComm.println(joyBL);
  }
  else if(joyBR != joystickB.right) {
    joyBR = joystickB.right;
    SerialMon.print("B RIGHT (-->)");
    SerialMon.println(joyBR);
    SerialComm.print("BR");
    SerialComm.println(joyBR);
  }
  else if(joyBP != joystickB.press) {
    joyBP = joystickB.press;
    SerialMon.print("B PRESSED (center)");
    SerialMon.println(joyBP);
    SerialComm.print("BP");
    SerialComm.println(joyBP);
  }
  
  SerialMon.flush();
  SerialComm.flush();
}
