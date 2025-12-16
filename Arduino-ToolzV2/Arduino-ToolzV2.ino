//  Arduino Toolz
// Written by Abdelali221

#include <EEPROM.h>
#include "DHT11.h"
#include <ArduinoVT.h>
#include <LiquidCrystal_I2C.h>

DHT11 DHT;
Term Term;

// Constants
#define BACK_SPACE 127
#define BACK_SPACE_ALT 8
#define DHT11_PIN 4
#define BELL 7
#define ESC 27
#define LCD_ROWS 4 // LCD Rows
#define LCD_COLUMNS 16 // LCD Columns
#define LCD_ADDRESS 0x26 // LCD Address

const char* valSelect[] = 

{ "Desired Value",
  "Pin Number",
  "Desired Address",
  "Desired Frequency"
};

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Booleans
bool exitloop = false;
bool Resume = false;

void setup() {
    // Initialization sequence
  Serial.begin(9600); // Serial begin at 9600bps
  Term.Clear();

    // Welcome Screen :
  StartSequence();
  Serial.print(F("$>")); // Shell indicator
}

void loop() {
  if (Serial.available()) { // Wait for incoming data
    noexitloop(); // Sets all booleans for loop and wait to false
    CommandSet(); // Recieve and manage the data
  }
}

void CommandSet() {
  
  char command[20]; // Array to store the command
  StringRead(command, sizeof(command)); // Read the incoming data
  Term.Return();

    // Compares the data to the available commands :
  if (strcmp(command, "Analog") == 0) {
    AnalogTool();
  } else if (strcmp(command, "Bell") == 0) {
    Serial.write(BELL);
  } else if (strcmp(command, "Cls") == 0) {
    Term.Clear();
  } else if (strcmp(command, "DHT11") == 0) {
    DHT11();
  } else if (strcmp(command, "Digital") == 0) {
    Serial.print(F("This Command isn't available anymore"));
    Term.Return();
    Serial.print(F("You can get the separate Digital-Toolz which is way better."));
    Term.Return();
    Serial.print(F("https://github.com/abdelali221/Digital-Toolz/"));
    Term.Return();
  } else if (strcmp(command, "EEPROM") == 0) {
    runEEPROM();
  } else if (strcmp(command, "Rave") == 0) {
    runRave();
  } else if (strcmp(command, "Terminal") == 0){
    runTerminal();
  } else if (strcmp(command, "Tone") == 0) {
    runTone();
  } else if (strcmp(command, "UltraR") == 0) {
    runUltraR();
  } else if (strcmp(command, "LCD") == 0) {
    runLCDutility();
  } else {
    Serial.println(F("Invalid command!"));
  }
  Term.Return();
  Serial.print(F("$>"));
}

void StringRead(char* buffer, int maxLength) {
  int c = 0;
  char chr = 0;
  while (!Resume) { // Leave space for null terminator
    if (Serial.available()) {
      chr = Serial.read();
      
      if (chr == NL || chr == CR) {
        if (c != 0) {
          buffer[c] = '\0'; // Null-terminate the string
          Term.Return();
          return;
        }
      } else if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) { // Backspace
        if (c > 0) {
          Serial.print(F("\b \b")); // Erase character on serial monitor
          c--;
        }
      } else if (chr >= '!') { 
        if (c < maxLength) {
          buffer[c++] = chr;
          Serial.print(chr);
        }
      }
    }
  }
}

void StartSequence() {
  // Prints the Welcome Sequence over Serial :
  lcd.init(); // LCD Screen Init
  lcd.backlight(); // Backlight On
  lcd.noAutoscroll();
  lcd.clear(); // Clear the screen
  lcd.print(F("  Arduino-Toolz"));
  lcd.setCursor(4, 1);
  lcd.print(F("Ver 2.4"));
  Serial.print(F("// Arduino-Toolz "));
  for (size_t i=0;i<2;i++)
    Term.Return();
  Serial.print(F(" Made by Abdelali221"));
  for (size_t i=0;i<2;i++)
    Term.Return();
  Serial.print(F(" Github : https://github.com/abdelali221"));
  for (size_t i=0;i<2;i++)
    Term.Return();
  Serial.print(F("Ver 2.4"));
  for (size_t i=0;i<2;i++)
    Term.Return();
}


void runLCDutility() {

  char chr = 0;  
  Serial.print(F("// LCDutility"));
  Term.Return();
  Serial.print(F("What do you want to do?"));
  Term.Return();
  Serial.print(F(" 1 - Init the LCD screen. 2 - Turn the Backlight on/off."));
  Term.Return();
  Serial.print(F(" 3 - Enable/Disable Cursor. 4 - Enable/Disable Blink. : "));
  noexitloop();
  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();
      Serial.print(chr);
      Term.Return();
      Resume = true;
    }
  }
  noexitloop();

  switch (chr) {

    case '1':// 1 = LCDinit
      lcd.init();
      lcd.noBacklight();
      delay(500);
      lcd.backlight();

      for (int i = 0; i < 10; i++) {
        delay(150);
        lcd.print(F("TESTING DISPLAY"));
      }

      lcd.clear();
    break;

    case '2': // 2 = Backlight
      Serial.write(CR);
      Serial.write(NL);
      Serial.print(F("ON or OFF? 1 - ON / 2 - OFF : "));
      while (!Resume) {
        if (Serial.available()) {
          chr = Serial.read();
          Serial.print(chr);
          Term.Return();
          Resume = true;
        }
      }

      if (chr == '1') {
        lcd.backlight();
      } else if (chr == '2') {
        lcd.noBacklight();
      }
    break;

    case '3': // 3 = Cursor
      Term.Return();
      Serial.print(F("Enable or Disable? 1 - Enable / 2 - Disable : "));
      while (!Resume) {
        if (Serial.available()) {
          chr = Serial.read();
          Serial.print(chr);
          Serial.write(CR);
          Serial.write(NL);
          Resume = true;
        }
      }

      if (chr == '1') {
        lcd.cursor_on();
      } else if (chr == '2') {
        lcd.cursor_off();
      }
    break;

    case '4': // 4 = Blink
      Term.Return();
      Serial.print(F("Enable or Disable? 1 - Enable / 2 - Disable : "));
      while (!Resume) {
        if (Serial.available()) {
          chr = Serial.read();
          Serial.print(chr);
          Term.Return();
          Resume = true;
        }
      }

      if (chr == '1') {
        lcd.blink_on();
      } else if (chr == '2') {
        lcd.blink_off();
      }
    break;
  }

}

void runTerminal() {
  uint8_t c = 0;
  uint8_t i = 0;
  char ESCString[6];
  bool GetCommand = false;
  lcd.clear();
  Term.Clear();
  lcd.cursor_on();
  while (1) {
    if (Serial.available()) {
      while (Serial.available() > 0) {
        char chr = Serial.read(); // Read the data character by character
        if (chr != CR && chr != NL) { // Verify if there is no NL or CR
          if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) {
            Serial.print("\b\e[K"); // BackSpace command
            if (c > 0) {
              c--;
              if (c < LCD_COLUMNS) { // Goes back by one, Prints " " and return back
                lcd.setCursor(c, 0);
                lcd.print(" ");
                lcd.setCursor(c, 0);
              } else if (c < LCD_COLUMNS * 2) {
                lcd.setCursor(c - LCD_COLUMNS, 1);
                lcd.print(" ");
                lcd.setCursor(c - LCD_COLUMNS, 1);
              } else if (c < LCD_COLUMNS * 3) {
                lcd.setCursor(c - LCD_COLUMNS, 0);
                lcd.print(" ");
                lcd.setCursor(c - LCD_COLUMNS, 0);
              } else if (c < LCD_COLUMNS * 4) {
                lcd.setCursor(c - LCD_COLUMNS * 2, 1);
                lcd.print(" ");
                lcd.setCursor(c - LCD_COLUMNS * 2, 1);
              }
            }
          } else if (chr == ESC) {
            for (uint8_t j=0;j<6;j++) {
              ESCString[j] = 0; // Echo the char
              i = 0;
            }
            GetCommand = true;
            delay(10);
            while (GetCommand) {    
              for (uint8_t i=0;i<6;i++) {
                if (Serial.available() > 0) {
                  chr = Serial.read();
                  ESCString[i] = chr;
                }
              }
              if (ESCString[0] == '[') {
                if (ESCString[2] == '~') {
                  if (ESCString[1] == '4') {
                    StartSequence();
                    return;
                  }
                } else if (ESCString[1] == 'D') {
                  //Term.CursMove('D');
                  GetCommand = false;
                  if (c > 0) {
                    c--;
                    SetLCDCurs(c);
                  }
                } else if (ESCString[1] == 'C') {
                  //Term.CursMove('C');
                  GetCommand = false;
                  if (c < LCD_COLUMNS * LCD_ROWS - 1) {
                    c++;
                    SetLCDCurs(c);
                  }
                } else if (ESCString[1] == 'A') {
                  //Term.CursMove('A');
                  GetCommand = false;
                  if (c >= LCD_COLUMNS) {
                    c -= LCD_COLUMNS;
                    SetLCDCurs(c);
                  }
                } else if (ESCString[1] == 'B') {
                  //Term.CursMove('B');
                  GetCommand = false;
                  if (c < LCD_COLUMNS * LCD_ROWS) {
                    c += LCD_COLUMNS;
                    SetLCDCurs(c);
                  }
                } else {
                  GetCommand = false;
                  chr = ESCString[1];
                }
              } else {
                GetCommand = false;
              }
            }
          } else if (chr >= '!' || chr == ' ') { // LCD Cursor position is controlled using the c variable (c for cursor)
            SetLCDCurs(c);
            if (c == LCD_COLUMNS * LCD_ROWS) {
              c = 0;
              lcd.clear();
            }
            c++; // Add one each time a character is printed
            lcd.print(chr); // Prints the char
            Serial.print(chr); // Echo back to the terminal
          }
        } else {
          Term.Return();
          c = 0;
          lcd.clear();
        }
      }
    }
  }
}

void SetLCDCurs(uint8_t c) {
  if (c < LCD_COLUMNS) {
    lcd.setCursor(c, 0);
  } else if (c < LCD_COLUMNS * 2) {
    lcd.setCursor(c - LCD_COLUMNS, 1);
  } else if (c < LCD_COLUMNS * 3) {
    lcd.setCursor(c - LCD_COLUMNS, 0);
  } else if (c < LCD_COLUMNS * 4) {
    lcd.setCursor(c - LCD_COLUMNS * 2, 1);
  }
}

void AnalogTool() {

  int Pin = ValSelect(5, 1); // Pin Variable for Analog/Digital Tools
  int buffer;
  
  if (Pin < 0 || Pin > 5) { // Checks if the Pin number is valid or the user canceled the operation
    if (Pin == -1) {
      Serial.println(F("Canceled."));
      return;
    } else {
      Serial.print(F("Invalid PIN number!"));
      delay(1000);
      return;
    }
  }

  noexitloop();
  uint8_t APIN = Pin + 14;
  Serial.println(F("Press b To exit"));
  Serial.print(F("PIN A"));
  Serial.print(Pin);
  Serial.print(F(" : "));
  
  while (!exitloop) {
      
    buffer = analogRead(APIN);
    if (buffer < 1000 && buffer >= 100) {
    Serial.print(F("0"));
    } else if (buffer < 100 && buffer >= 10) {
      Serial.print(F("00"));
    } else if (buffer < 10) {
      Serial.print(F("000"));
    }
    Serial.print(buffer);
    delay(50);
    for (int i = 0; i < 4; i++) {
      Serial.print(F("\b"));
    }

    if (Serial.available()) {
      char chr = Serial.read();

      if (chr == 'b') { // Exits if b is sent
        Term.Return();
        exitloop = true;
      }
    }
  }
}

void runRave() {

  uint8_t Rave; // Rave even/odd switch

  Serial.println("Press b To exit");

  while (!exitloop) {
    for (int i = 0; i < 20; i++) {
      lcd.write(255);
      lcd.print(F(" "));
    }

    for (int i = 0; i < 20; i++) {
      lcd.print(F(" "));
      lcd.write(255);
    }

    lcd.setCursor(0, 0);
    delay(200);

    for (int i = 0; i < 20; i++) {
      lcd.print(F(" "));
      lcd.write(255);
    }

    for (int i = 0; i < 20; i++) {
      lcd.write(255);
      lcd.print(F(" "));
    }

    delay(200);

    if (Serial.available()) {
      char data = Serial.read();

      if (data == 'b') {
        lcd.clear();
        exitloop = true;
      }
    }
  }
}

void runEEPROM() {

  uint8_t ReadWrite_Switch = 0;
  uint8_t value = 0; // EEPROM Read/Write
  uint16_t address = 0; // EEPROM Address
  uint8_t EVF = 0; // EEPROM Value Format Switch

  Serial.println(F("  EEPROM Tool"));
  Term.Return();
  delay(2000);
  noexitloop();
  Serial.println(F(" Read, Write or Clear?"));
  Serial.print(F("R=0  W=1  C=2 : "));

  while (!Resume) { // Select the format
    if (Serial.available()) {
      Resume = true;
      ReadWrite_Switch = Serial.read() - 48; 
      Serial.println(ReadWrite_Switch);
      delay(500);
    }
  }

  if (ReadWrite_Switch < 0 || ReadWrite_Switch > 2) { // Already explained in Digital/Analog
    Serial.println(F(" Invalid Choice!"));
    delay(1000);
    return;
  }

  noexitloop();

  if (ReadWrite_Switch == 0) {
    Serial.println(F("INT, HEX or BIN?  "));
    Serial.print(F(" I=0  H=1  B=2 : "));

    while (!Resume) { // Select the format
      if (Serial.available()) {
        Resume = true;
        EVF = Serial.read() - 48; 
        Serial.println(EVF);
        delay(500);
      }
    }
  }

  if (EVF < 0 || EVF > 2) { // Already explained in Digital/Analog
    Serial.println(F(" Invalid Choice!"));
    delay(1000);
    return;
  }

  noexitloop();

  if (ReadWrite_Switch != 2) {
      // Reads the Address
    address = ValSelect(EEPROM.length(), 2);
    if (address == -1) {
      Serial.println(F("Canceled."));
      return;
    }
  }

  if (address > EEPROM.length()) { // Compares the address to the EEPROM length
    Serial.println(F("Invalid Address!"));
    delay(3000);
    return;
  }

  noexitloop();

  switch (ReadWrite_Switch) {
    case 0: // Read

      value = EEPROM.read(address);
      Serial.print(F("  The value of Addr"));
      Serial.print(address);
      Serial.print(F(" is "));
      
      switch (EVF) {
        case 0:
          Serial.println(value);
        break;

        case 1:
          Serial.println(value, HEX);
        break;

        case 2:
          Serial.println(value, BIN);
        break;
      }
      delay(2000);
      break;

    case 1: // Write

    value = ValSelect(255, 0);

      if (value < 0 || value > 255) {
        Serial.println(F("Invalid Value!"));
        delay(3000);
        return;
      }

      noexitloop();
      EEPROM.write(address, value);
      if (value == EEPROM.read(address)) {
        Serial.print(F("Value "));
        Serial.print(value);
        Serial.print(F(" Was written to Address : "));
        Serial.print(address);
        Term.Return();
        delay(1000);
      } else {
        Serial.println(F("ERROR WRITING VALUE!!"));
        delay(500);
      }
    break;

    case 2:

      Term.Clear();
      Serial.println(F("THIS OPERATION WILL ERASE ANY DATA LOCATED ON THE EEPROM"));
      Term.Return();
      Serial.print(F("Do you want to proceed? Y/N : "));

      while (!exitloop) {
        if (Serial.available()) {
          char chr = Serial.read();

          if (chr == 'y' || chr == 'Y') {
            Serial.println(F("Y"));
            exitloop = true;
          } else if (chr == 'n' || chr == 'N') {
            Serial.println(F("N"));
            return;
          }
        }
      }
      for (uint8_t i = 0; i <= EEPROM.length(); i++) {
        EEPROM.write(i, 0);
        Serial.print(F("Address : "));
        Serial.print(i);
        Serial.write(CR);
        if (EEPROM.read(i) != 0) {
          Serial.println(F("ERROR!"));
          delay(500);
          break;
        }
      }

      delay(200);
      Serial.println(F("EEPROM is clear!"));
      delay(3000);
      break;
  }
}

void DHT11() {

  char chr = 0;
  Serial.print(F("Make sure your module is connected correctly."));
  Term.Return();
  Serial.print(F("Data : D4"));

  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();

      if (chr == NL || chr == CR) {
        Term.Return();
        Resume = true;
      }
    }
  }
  Serial.println(F("Press b To exit"));

  while (!exitloop) {

    delay(500);
    DHT.readata(DHT11_PIN);
    switch (DHT.humidity) {
    case -1:
      Serial.print(F("Error "));
      Serial.println(DHT.humidity);
      delay(1000);
      return;
    break;
    case -2:
      Serial.print(F("Error "));
      Serial.println(DHT.humidity);
      delay(1000);
      return;
    break;
    }
    Serial.print(F("Temp : "));
    Serial.print(DHT.temperature);
    Serial.print(F(" C /"));
    Serial.print(F(" Humid : "));
    Serial.print(DHT.humidity);
    Serial.print(F(" % "));
    Serial.write(CR);

    if (Serial.available()) { 
      chr = Serial.read();

      if (chr == 'b') {
        Term.Return();
        exitloop = true;
      }
    }
  }
}

void runTone() {
  Term.Clear();
  Serial.println(F("Tone Generator"));
  delay(2000);
  Term.Return();
  int Pin = ValSelect(13, 1);
  Term.Return();
  Serial.print(F(" Press Enter To Continue..."));
  while (!exitloop) {

    if (Serial.available()) {
      noTone(Pin);
      char chr = Serial.read();

      if (chr == NL || chr == CR) {
        Term.Clear();
        int Freq = ValSelect(15000, 3);
        chr = 0;
        Term.Return();
        Serial.print(F("Frequency : "));
        if (Freq < 100) {
          Serial.print(F("000"));
        } else if (Freq < 1000) {
          Serial.print(F("00"));
        } else if (Freq < 10000) {
          Serial.print(F("0"));
        }
        Serial.print(Freq);
        Serial.print(F(" Hz"));
        tone(Pin, Freq);
      }      
    }
  }
}


void runUltraR() {

  int Pin = ValSelect(13, 1);

  if (Pin < 0 || Pin > 12) {
    if (Pin == -1) {
      Serial.println(F("Canceled."));
      return;
    } else {
        Serial.print(F("   Invalid PIN number!"));
        delay(1000);
      return;
    }
  }

  Serial.println(F("Press b To exit"));

  while (!exitloop) {

    long duration, cm;

    pinMode(Pin, OUTPUT);
    digitalWrite(Pin, LOW);
    delayMicroseconds(2);
    digitalWrite(Pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(Pin, LOW);
    pinMode(Pin + 1, INPUT);
    duration = pulseIn(Pin + 1, HIGH);
    cm = ms2cm(duration);

    if (cm < 400 && cm > 4) {
      if (cm < 100 && cm >= 10) {
        Serial.print(F("0"));
      } else if (cm < 10) {
        Serial.print(F("00"));
      }
      Serial.print(cm);
      Serial.print(F("cm"));
      for (int i = 0; i < 5; i++) {
        Serial.print(F("\b"));
      }
    } else {
      Serial.print(F("Out Of Range!"));
      delay(500);
      for (int i = 0; i < 13; i++) {
        Serial.print(F("\b \b"));
      }
    }

    delay(100);

    if (Serial.available()) {
      char chr = Serial.read();

      if (chr == 'b') {
        Term.Return();
        exitloop = true;
      }

    }

  }

}

long ms2cm(long microseconds) {
  return microseconds / 29 / 2;
}

int ValSelect(int MaxValue, int i) {

  int value = 0;
  int c = 0;

  Serial.print(F("Please Enter The "));
  Serial.println(valSelect[i]);
  Serial.print(F("0 - "));
  Serial.print(MaxValue);
  Serial.print(F(" : "));

  while (!exitloop) {
    if (Serial.available()) {

      char chr = Serial.read();

      if (chr == NL || chr == CR) {
        Term.Return();
        exitloop = true;
      } else if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) {
        for (int i = 0; i < c; i++) {
          Serial.write(chr);
        }
        c = 0;
        value = 0;
      } else {
        if (chr >= '0' && chr <= '9') {
          if (c < 5) {
            for (int i = 0; i < c; i++) {
              Serial.write(BACK_SPACE);
            }
            c++;
            value = (value * 10) + chr - 48;
            Serial.print(value);
          }
        }
      }
    }
  }
  noexitloop();
  return value;
}

void noexitloop() {
  exitloop = false; // Init the Stay in loop
  Resume = false; // Init the Wait For action
}