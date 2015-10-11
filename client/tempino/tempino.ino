#include <registers.h>
#include <pin_magic.h>
#include <Adafruit_TFTLCD.h>

// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40

// #define YP A1  // must be an analog pin, use "An" notation!
// #define XM A2  // must be an analog pin, use "An" notation!
// #define YM 7   // can be a digital pin
///#define XP 6   // can be a digital pin
#define XM A1
#define YP A2
#define YM 6
#define XP 7

 #define TS_MINX 150
 #define TS_MINY 120
 #define TS_MAXX 880
 #define TS_MAXY 940

//#define TS_MINX 131
//#define TS_MINY 159
//#define TS_MAXX 942
//#define TS_MAXY 878


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Make the icons bigger or smaller
#define ICONSIZE 20

void setup(void) {
  Serial.begin(9600);
  tft.reset();

  // There are various drivers, we check them. This code has been brought
  // by https://github.com/adafruit/TFTLCD-Library 
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9327) {
    Serial.println(F("Found ILI9327 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier == 0x0154) {
    Serial.println(F("Found S6D0154 LCD driver"));
  } else if(identifier == 0x9488) {
    Serial.println(F("Found ILI9488 LCD driver"));
 } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);

  // I happen to like this position, feel free to 
  // experiment and change it
  tft.setRotation(2);

  tft.fillScreen(BLACK);
  drawHeader(false);
 
  pinMode(13, OUTPUT);
}

void drawHeader(bool loading) {
  tft.fillRect(10, 5, 240, 30, BLACK);
  if (!loading) {
      tft.setCursor(10, 5);
      tft.setTextSize(4);
      tft.setTextColor(WHITE);
      tft.println("Sensors");
  } else {
      tft.setCursor(10, 5);
      tft.setTextSize(4);
      tft.setTextColor(WHITE);
      tft.println("Loading");
  }
}

void drawCPU(int x, int y) {
  int bound = ICONSIZE / 10;
  int factor = ICONSIZE / 4;
  int dieSize = ICONSIZE - bound * 2;
  int added = 0;
  tft.fillRect(x, y, x + ICONSIZE, y + ICONSIZE, BLACK);
  for (int c=1;c < 4; c++) {
    added = factor * c;
    tft.drawLine(x + added, y, x + added, y + ICONSIZE, YELLOW);
    tft.drawLine(x + added, y, x + added, y + ICONSIZE, YELLOW);
    tft.drawLine(x, y + added, x + ICONSIZE, y + added, YELLOW);
    tft.drawLine(x, y + added, x + ICONSIZE, y + added, YELLOW);
  }
  tft.fillRect(x + bound, y + bound, dieSize, dieSize, WHITE);
}

void drawFan(int x, int y) {
  int radius = ICONSIZE / 2;
  int centerX = x + radius;
  int centerY = y + radius;
  tft.fillRect(x, y, x + ICONSIZE, y + ICONSIZE, BLACK);

  for(int r = 1; r < 6; r++) {
    tft.drawCircle(centerX, centerY, radius / r, BLUE);
  }
  
}

void drawSystem(int x, int y) {
  int width = ICONSIZE / 3;
  tft.fillRect(x, y, x + ICONSIZE, y + ICONSIZE, BLACK);
  tft.fillRect(x + width, y, width, ICONSIZE, GREEN);
  tft.drawRect(x + width + 1, y + 1, width - 2 , ICONSIZE - 2, BLACK);
}

void drawTemp(int x, int y, String value) {
  int horizontal = x + ICONSIZE + (ICONSIZE / 8);
  int vertical = y + (ICONSIZE / 3);
  char text[50];
  tft.fillRect(horizontal, vertical, 240, ICONSIZE, BLACK);
  tft.setCursor(horizontal, vertical);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  sprintf(text, "%s C", value.c_str());

  tft.println(text);
}

void drawRPM(int x, int y, String value) {
  int horizontal = x + ICONSIZE + (ICONSIZE / 8);
  int vertical = y + (ICONSIZE / 3);
  char text[50];
  tft.fillRect(horizontal, vertical, 240, ICONSIZE, BLACK);
  tft.setCursor(horizontal, vertical);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  sprintf(text, "%s RPM", value.c_str());

  tft.println(text);
}


// Slice and dice the string so we
// get the commands to display
String getCommand(String str) {
  int pos = str.lastIndexOf(';');
  if (pos > -1) {
    return str.substring(pos + 1, str.length());
  } else {
    return str;
  }
}

// Get the rest of the string so it shrinks
String advanceString(String input) {
  int pos = input.lastIndexOf(';');
  if (pos > -1) {
    return input.substring(0, pos);
  } else {
    return input;
  }
}

void loop()
{
  String input;
  String temp;
  String command;
  String sensorType;
  String value;
  int len;
  int cursorPos = 20;
  int increments = 30;
  while(Serial.available() > 0) {
    cursorPos = 40;
    // Get the serial string
    input = Serial.readStringUntil('\n');
    
    while (input.length() > 0)
    {
        command = getCommand(input);
        
        len = 0;
        while((len < command.length() && (command.charAt(len) != '='))) {
          len++;
        }

        // Get the command part and the data part in 
        // different variables
        sensorType = command.substring(0, len);
        value = command.substring(len + 1, command.length());

        if(sensorType.equals("CPU")) {
          drawCPU(10, cursorPos);
          drawTemp(10, cursorPos, value);
        }

        if(sensorType.equals("FAN")) {
          drawFan(10, cursorPos);
          drawRPM(10, cursorPos, value);
        }

        if(sensorType.equals("SYS")) {
          drawSystem(10, cursorPos);
          drawTemp(10, cursorPos, value);
        }

        // Make sure the next icon appears below
        cursorPos = cursorPos + increments;

        if (input.lastIndexOf(';') != -1) {
          // Keep shrinking
          temp = advanceString(input);
          input = temp;
        } else {
          // Finish the while loop
          input = "";
        }
    }
  }
}

