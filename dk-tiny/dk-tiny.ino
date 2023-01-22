#include <Wireling.h>
#include <SX1505.h> // For interfacing with Joystick and Rotary Switch Wirelings
#include <SPI.h>
#include <TinyScreen.h>

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
TinyScreen display = TinyScreen(TinyScreenPlus);


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

  display.begin();
  //setBrightness(brightness);//sets main current level, valid levels are 0-15
  display.setBrightness(10);
  display.setFlip(true);
}

void loop() {
  Wireling.selectPort(JS_A_PORT);
  joystickA.getPosition();
  Wireling.selectPort(JS_B_PORT);
  joystickB.getPosition();

  report();
  delay(100);
}


void writeText(String text){
  display.clearScreen();
  //setFont sets a font info header from font.h
  //information for generating new fonts is included in font.h
  display.setFont(thinPixel7_10ptFontInfo);
  //getPrintWidth(character array);//get the pixel print width of a string
  // int width=display.getPrintWidth("Example Text!");
  //setCursor(x,y);//set text cursor position to (x,y)- in this example, the example string is centered
  // display.setCursor(48-(width/2),10);
  //fontColor(text color, background color);//sets text and background color
  // display.fontColor(TS_8b_Green,TS_8b_Black);
  // display.print("Example Text!");
  // display.setCursor(15,25);
  // display.fontColor(TS_8b_Blue,TS_8b_Black);
  // display.print("More example Text!");
  // display.setCursor(3,40);
  // display.fontColor(TS_8b_Red,TS_8b_Black);
  // display.print("(Does not wrap)");

  display.setCursor(3,25);
  display.fontColor(TS_8b_White,TS_8b_Black);
  display.print(text);
}

void report() {
  if (joyAU != joystickA.up) {
    joyAU = joystickA.up;
    SerialMon.print("A UP (towards connector)");
    SerialMon.println(joyAU);
    SerialComm.print("AU");
    SerialComm.println(joyAU);
    writeText("A UP (towards connector)");
  }
  else if(joyAD != joystickA.down) {
    joyAD = joystickA.down;
    SerialMon.print("A DOWN (away connector)");
    SerialMon.println(joyAD);
    SerialComm.print("AD");
    SerialComm.println(joyAD);
    writeText("A DOWN (away connector)");
  }
  else if(joyAL != joystickA.left) {
    joyAL = joystickA.left;
    SerialMon.print("A LEFT (<--)");
    SerialMon.println(joyAL);
    SerialComm.print("AL");
    SerialComm.println(joyAL);
    writeText("A LEFT (<--)");
  }
  else if(joyAR != joystickA.right) {
    joyAR = joystickA.right;
    SerialMon.print("A RIGHT (-->)");
    SerialMon.println(joyAR);
    SerialComm.print("AR");
    SerialComm.println(joyAR);
    writeText("A RIGHT (-->)");
  }
  else if(joyAP != joystickA.press) {
    joyAP = joystickA.press;
    SerialMon.print("A PRESSED (center)");
    SerialMon.println(joyAP);
    SerialComm.print("AP");
    SerialComm.println(joyAP);
    writeText("A PRESSED (center)");
  }

  if (joyBU != joystickB.up) {
    joyBU = joystickB.up;
    SerialMon.print("B UP (towards connector)");
    SerialMon.println(joyBU);
    SerialComm.print("BU");
    SerialComm.println(joyBU);
    writeText("B UP (towards connector)");
  }
  else if(joyBD != joystickB.down) {
    joyBD = joystickB.down;
    SerialMon.print("B DOWN (away connector)");
    SerialMon.println(joyBD);
    SerialComm.print("BD");
    SerialComm.println(joyBD);
    writeText("B DOWN (away connector)");
  }
  else if(joyBL != joystickB.left) {
    joyBL = joystickB.left;
    SerialMon.print("B LEFT (<--)");
    SerialMon.println(joyBL);
    SerialComm.print("BL");
    SerialComm.println(joyBL);
    writeText("B LEFT (<--)");
  }
  else if(joyBR != joystickB.right) {
    joyBR = joystickB.right;
    SerialMon.print("B RIGHT (-->)");
    SerialMon.println(joyBR);
    SerialComm.print("BR");
    SerialComm.println(joyBR);
    writeText("B RIGHT (-->)");
  }
  else if(joyBP != joystickB.press) {
    joyBP = joystickB.press;
    SerialMon.print("B PRESSED (center)");
    SerialMon.println(joyBP);
    SerialComm.print("BP");
    SerialComm.println(joyBP);
    writeText("B PRESSED (center)");
  }
  
  SerialMon.flush();
  SerialComm.flush();
}
