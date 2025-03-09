#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "DHT11.h"
#include <string.h>

DHT11 DHT;
#define DHT11_PIN 4
// Constants
const int BACK_SPACE = 127;
const int BACK_SPACE_ALT = 8;
const int NL = 10; // NewLine command
const int CR = 13; // Carriage Return command
const int ESC = 27;
const int LCD_ROWS = 4; // LCD Rows
const int LCD_COLUMNS = 16; // LCD Columns
const int LCD_ADDRESS = 0x27; // LCD Address
const int DIRECTION = 91;

const char* commandlist[] = 

{ " Analog",
  " Cls",
  " DHT11",
  " Digital",
  " EEPROM",
  " Help",
  " Intro",
  " LCD",
  " Rave",
  " Terminal",
  "\0"
}; // Command list

const char* welcome[] = 

{ "// Arduino Toolz",
  " Proudly developped by Abdelali221",
  " Ver 2.0 (New Release/Entirely rewritten)",
  " Github : https://github.com/abdelali221/",
  " There is a list of the commands :",
  "\0"
}; // Welcome Text

const char* DHTtext[] = 

{ "ERROR! The DHT11 Pin returned invalid data",
  "Either it's broken or not connected correctly",
  "ERROR! The DHT11 is not connected",
  "Plug your DHT11 | VCC to VCC, GND to GND and data to D4",
  "Once done press Enter",
  "\0"
};

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Soon :
/*// Time Variables
int sec = 0; // Seconds 
int min = 0; // Minutes
int hrs = 0; // Hours
int hrs12 = 12; // Hours (AM/PM)
*/

// System Variables
int c; // Char Counter
int ReadWrite_Switch;
int value; // EEPROM Read/Write
int address; // EEPROM Address
int EVF; // EEPROM Value Format Switch
int Rave; // Rave even/odd switch
char chr;

// Booleans
bool exitloop = false;
bool Resume = false;

void setup() {
    // Initialization sequence
  Serial.begin(9600); // Serial begin at 9600bps
  Serial.write(ESC); // ESC command (Required to send the Clear Screen instruction)
  Serial.print("[2J"); // Clear the terminal
  lcd.init(); // LCD Screen Init
  lcd.backlight(); // Backlight On
  lcd.clear(); // Clear the screen
    // Welcome Screen :
  serialwelcome(); 
  lcd.print("  Arduino Toolz");
  lcd.setCursor(3, 1);
  lcd.print("Release 2.0");
  delay(2000);
  lcd.clear();
  Serial.print("$>"); // For the Shell
  lcd.print("$>"); // For the Shell
}

void loop() {
  if (Serial.available()) { // Wait for incoming data
    noexitloop(); // Sets all System booleans for loop and wait to false
    lcd.clear();
    lcd.print("$>"); 
    CommandSet(); // Recieve and manage the data
  }
}

void CommandSet() {
  
  char command[15]; // Array to store the command
  StringRead(command, sizeof(command)); // Read the incoming data

    // Compares the data to the available commands :
  if (strcmp(command, "Analog") == 0) {
    AnalogTool();
  } else if (strcmp(command, "Cls") == 0) {
    Serial.write(ESC);
    Serial.print("[2J");
  } else if (strcmp(command, "DHT11") == 0) {
    DHT11();
  } else if (strcmp(command, "Digital") == 0) {
    DigitalTool();
  } else if (strcmp(command, "EEPROM") == 0) {
    runEEPROM();
  } else if (strcmp(command, "Help") == 0) {
    runHelp();
  } else if (strcmp(command, "Intro") == 0) {
    serialwelcome();
  } else if (strcmp(command, "LCD") == 0) {
    runLCDutility();
  } else if (strcmp(command, "Rave") == 0) {
    runRave();
  } else if (strcmp(command, "Terminal") == 0) {
    runTerminal();
  } else {
    Serial.println("Invalid command!");
  }
  Serial.print("$>");
  lcd.print("$>");
}

void StringRead(char* buffer, int maxLength) {
  int index = 0;
  while (!Resume) { // Leave space for null terminator
    if (Serial.available()) {
      char chr = Serial.read();
      
      if (chr == NL || chr == CR) {
        buffer[index] = '\0'; // Null-terminate the string
        ReturnToline();
        lcd.clear();
        return;
      } else if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) { // Backspace
        if (index > 0) {
          Serial.print("\b \b"); // Erase character on serial monitor
          lcd.setCursor(index + 1, 0);
          lcd.print(" ");
          lcd.setCursor(index + 1, 0); 
          index--;
        }
      } else { 
        if (index < 14) {
          buffer[index++] = chr;
          Serial.print(chr);
          lcd.print(chr);
        }
      }
    }
  }
}

void runTerminal() {

  if (Serial.available()) {
    if (Serial.available() > 0) {
      chr = Serial.read(); // Read the data character by character

      if (chr != CR && chr != NL) { // Verify if there is no NL or CR
        if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) {
          Serial.print("\b\e[K"); // Clear Screen command

          if (c > 0) {
            c--;
            if (c < 16) { // Goes back by one, Prints " " and return back
              lcd.setCursor(c, 0);
              lcd.print(" ");
              lcd.setCursor(c, 0);
            } else {
              lcd.setCursor(c - 16, 1); // Same but 16 should be omitted from c to have the right Column value
              lcd.print(" ");
              lcd.setCursor(c - 16, 1);
            }
          }
        } else { // LCD Cursor position is controlled using the c variable (c for cursor)
          if (c < 16) {
            lcd.setCursor(c, 0);
          } else if (c > 15) {
            if (c == 32) { // Clears the LCD screen once it's filled
              c = 0; // resets c (Cursor)
              delay(50);
              lcd.clear();
            } else {
              lcd.setCursor(c - 16, 1);
            }
          }

          c++; // Add one each time a character is printed
          lcd.write(chr); // Prints the char
          Serial.print(chr); // Echo the char
        }
      } else {
        Serial.write(CR); // Sends Cariage Return
        Serial.write(NL); // Sends New Line
        c = 0;
        lcd.clear();
      }
    }
  }

  runTerminal(); // Loop
}

void runHelp() {
  // Print the command list through Serial
  ReturnToline();
  Serial.print("// Command list :");

  for(int i = 0; i < sizeof(commandlist) / sizeof(commandlist[0]); i++){
    ReturnToline();
    Serial.print(commandlist[i]);
  }

  ReturnToline();
}

void serialwelcome() {
  // Prints the Welcome Sequence over Serial :
  for (int i = 0; i < sizeof(welcome) / sizeof(welcome[0]); i++) {
    Serial.println(welcome[i]);
  }
  runHelp();
}

void DigitalTool() {

  int Pin = PinSelect(2); // Pin Variable for Analog/Digital Tools
  lcd.clear();

  if (Pin == -1) {
    Serial.println("Canceled");
    return;
  }
    
  if (Pin < 0 || Pin > 13) { // Checks if the Pin number is valid
    lcd.print("   Invalid PIN");
    lcd.setCursor(4, 1);
    lcd.print(" number!");
    delay(1000);
    lcd.clear();
    return;
  }

  noexitloop();

  while (!Resume) {
    lcd.setCursor(0, 0);
    lcd.print("Read or Write?  ");
    lcd.setCursor(4, 1);
    lcd.print("R=0  W=1 ");

    if (Serial.available()) {
      Resume = true;
      ReadWrite_Switch = Serial.read() - 48; // Read the instruction
      lcd.print(ReadWrite_Switch); // Print the selected instruction
      delay(500);
    }
  } 

  if (ReadWrite_Switch < 0 || ReadWrite_Switch > 1) { // Checks if the value is valid
    lcd.print(" Invalid Choice!");
    delay(1000);
    lcd.clear();
    return;
  }

  lcd.clear();
  noexitloop();

  switch (ReadWrite_Switch) {

    case 0: // Read

      lcd.setCursor(0, 0);
      lcd.print("    Read");
      delay(1000);
      lcd.clear();
      lcd.print("PIN D");
      lcd.print(Pin);
      lcd.print(" : ");

      Serial.println("Press b To exit");

      while (!exitloop) {

        pinMode(Pin, INPUT);
        lcd.setCursor(10, 0);
        lcd.print(digitalRead(Pin));

        if (Serial.available()) {
          chr = Serial.read();

          if (chr == 'b') {
            lcd.clear();
            exitloop = true;
          }
        }
      }
    break;

    case 1: // Write

      lcd.setCursor(0, 0);
      lcd.print("    Write");
      delay(1000);
      lcd.clear();
      pinMode(Pin, OUTPUT);
      lcd.print("PIN D");
      lcd.print(Pin);
      lcd.print(" : ");

      while (!exitloop) {
        
        lcd.setCursor(10, 0);
                
        switch (chr) {
          case '0':
            lcd.print("LOW ");
            digitalWrite(Pin, LOW);
          break;

          case '1':
            lcd.print("HIGH");
            digitalWrite(Pin, HIGH);
          break;
        }
        if (Serial.available()) {
          chr = Serial.read();

          if (chr == 'b') {
            lcd.clear();
            exitloop = true;
          }
        }
      }
    break;
  }

}  

void AnalogTool() {

  int Pin = PinSelect(1); // Pin Variable for Analog/Digital Tools
  lcd.clear();

  if (Pin == -1) {
    Serial.println("Canceled");
    return;
  }

  if (Pin < 0 || Pin > 5) { // Checks if the Pin number is valid
    lcd.print("   Invalid PIN");
    lcd.setCursor(4, 1);
    lcd.print(" number!");
    delay(1000);
    lcd.clear();
    return;
  }

  noexitloop();

  Serial.println("Press b To exit");

  while (!exitloop) {
    lcd.setCursor(0, 0);
    lcd.print("PIN A");
    lcd.print(Pin);
    lcd.print(" : ");

    switch (Pin) { // Checks what Pin was selected
      case 0:
        lcd.print(" ");
        lcd.print(analogRead(A0));
        lcd.print(" ");
        delay(50);
        break;
      case 1:
        lcd.print(" ");
        lcd.print(analogRead(A1));
        delay(50);
        lcd.print(" ");
        break;
      case 2:
        lcd.print(" ");
        lcd.print(analogRead(A2));
        delay(50);
        lcd.print(" ");
        break;
      case 3:
        lcd.print(" ");
        lcd.print(analogRead(A3));
        delay(50);
        lcd.print(" ");
        break;
      case 4:
        lcd.print(" ");
        lcd.print(analogRead(A4));
        delay(50);
        lcd.print(" ");
        break;
      case 5:
        lcd.print(" ");
        lcd.print(analogRead(A5));
        delay(50);
        lcd.print(" ");
        break;
    }

    if (Serial.available()) {
      chr = Serial.read();

      if (chr == 'b') { // Exits if b is sent
        lcd.clear();
        exitloop = true;
      }
    }
  }
}

void noexitloop() {
  exitloop = false; // Init the Stay in loop
  Resume = false; // Init the Wait For action
}

void runLCDutility() {
  Serial.print("// LCDutility");
  ReturnToline();
  Serial.print("What do you want to do?");
  ReturnToline();
  Serial.print(" 1 - Init the LCD screen. 2 - Turn the Backlight on/off.");
  ReturnToline();
  Serial.print(" 3 - Enable/Disable Cursor. 4 - Enable/Disable Blink. : ");
  noexitloop();
  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();
      Serial.print(chr);
      Serial.write(CR);
      Serial.write(NL);
      Resume = true;
    }
  }
  noexitloop();

  if (chr == '1') { // 1 = LCDinit
    lcd.init();
    lcd.noBacklight();
    delay(500);
    lcd.backlight();

    for (int i = 0; i < 10; i++) {
      delay(150);
      lcd.print("TESTING DISPLAY");
    }

    lcd.clear();
  } else if (chr == '2') { // 2 = Backlight
    Serial.write(CR);
    Serial.write(NL);
    Serial.print("ON or OFF? 1 - ON / 2 - OFF : ");
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
      lcd.backlight();
    } else if (chr == '2') {
      lcd.noBacklight();
    }
  } else if (chr == '3') { // 3 = Cursor
    Serial.write(CR);
    Serial.write(NL);
    Serial.print("Enable or Disable? 1 - Enable / 2 - Disable : ");
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
  } else if (chr == '4') { // 3 = Cursor
    Serial.write(CR);
    Serial.write(NL);
    Serial.print("Enable or Disable? 1 - Enable / 2 - Disable : ");
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
      lcd.blink_on();
    } else if (chr == '2') {
      lcd.blink_off();
    }
  }

}

void runEEPROM() {

  int address = 0;
  int value = 0;
  lcd.print("  EEPROM Tool");
  delay(2000);
  noexitloop();

  while (!Resume) {
    lcd.setCursor(0, 0);
    lcd.print(" Read or Write?");
    lcd.setCursor(4, 1);
    lcd.print("R=0  W=1 ");

    if (Serial.available()) {
      Resume = true;
      ReadWrite_Switch = Serial.read() - 48;
      lcd.print(ReadWrite_Switch); // Already explained in Digital
      delay(500);

        if (ReadWrite_Switch == 1) {

          noexitloop();
          lcd.clear();

          while (!Resume) {
            lcd.setCursor(0, 0);
            lcd.print("Clear or Write?");
            lcd.setCursor(4, 1);
            lcd.print("C=2  W=1 ");

            if (Serial.available()) {
              Resume = true;
              ReadWrite_Switch = Serial.read() - 48;
              lcd.print(ReadWrite_Switch);
              delay(500);
              lcd.clear();
            }
          }

        }

        else {
          lcd.clear();
        }
      }

      if (ReadWrite_Switch < 0 || ReadWrite_Switch > 2) { // Already explained in Digital/Analog
        lcd.print(" Invalid Choice!");
        delay(1000);
        lcd.clear();
        return;
      }
    }

    noexitloop();

    if (ReadWrite_Switch == 0) {

      while (!Resume) { // Select the format
        lcd.setCursor(0, 0);
        lcd.print("INT, HEX or BIN?  ");
        lcd.setCursor(0, 1);
        lcd.print(" I=0  H=1  B=2 ");

        if (Serial.available()) {
          Resume = true;
          EVF = Serial.read() - 48; 
          lcd.print(EVF);
          delay(500);
          lcd.clear();
        }
      }
    }
    if (EVF < 0 || EVF > 2) { // Already explained in Digital/Analog
      lcd.print(" Invalid Choice!");
      delay(1000);
      lcd.clear();
      return;
    }
    noexitloop();

    if (ReadWrite_Switch != 2) {
      int c = 0;
        // Reads the Address
      while (!Resume) {
        lcd.setCursor(0, 0);
        lcd.print(" Waiting : Addr");

        if (Serial.available()) {

          chr = Serial.read();
          
          if (chr == NL || chr == CR) {
            ReturnToline();
            Resume = true;
          } else {
            if (chr <= '9' && chr >= '0') {

              if (c == 0 && chr == '0') {
                Serial.print(chr);  
              } else {
                if (c < 5) {
                  Serial.print(chr);
                  c++;
                  address = address * 10 + chr - 48;
                }
              }
            }
          }
        }
      }
    }
    if (address > EEPROM.length()) { // Compares the address to the EEPROM length
      lcd.clear();
      lcd.print("Invalid Address!");
      delay(3000);
      lcd.clear();
      return;
    }

  noexitloop();

  lcd.clear();

  switch (ReadWrite_Switch) {
    case 0: // Read

      value = EEPROM.read(address);
      lcd.print("  The value of ");
      
      switch (EVF) {
        case 0:
          lcd.setCursor(0, 1);
          lcd.print("Addr ");
          lcd.print(address);
          lcd.print(" is ");
          lcd.print(value);
          break;

        case 1:
          lcd.setCursor(0, 1);
          lcd.print("Addr ");
          lcd.print(address);
          lcd.print(" is ");
          lcd.print(value, HEX);
          break;

        case 2:
          lcd.setCursor(4, 1);
          lcd.print("Addr ");
          lcd.print(address);
          lcd.print(" is ");
          delay(1000);
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.print(value, BIN);
          break;
      }
      delay(2000);
      lcd.clear();
      break;

    case 1: // Write

      while (!Resume) {
        lcd.setCursor(0, 0);
        lcd.print("   Waiting : ");
        lcd.setCursor(6, 1);
        lcd.print("value");

        if (Serial.available()) {

          chr = Serial.read();
          Serial.print(chr);

          if (chr == NL || chr == CR) {
            ReturnToline();
            Resume = true;
          }

          else {

            if (c != 0 && chr != '0') {
              c++;
            }

            if (c < 4) {
              value = value * 10 + chr - 48;
            }
          }
        }
      }

      if (value < 0 || value > 255) {
        lcd.clear();
        lcd.print("Invalid Value!");
        delay(3000);
        lcd.clear();
        return;
      }

      noexitloop();
      EEPROM.write(address, value);
      if (value == EEPROM.read(address)) {
        lcd.clear();
        lcd.print("Value ");
        lcd.print(value);
        lcd.print(" Was ");
        lcd.setCursor(0, 1);
        lcd.print("   written to ");
        delay(2000);
        lcd.clear();
        lcd.print("  Address ");
        lcd.print(address);
        delay(1000);
        lcd.clear();
      } else {
        lcd.clear();
        lcd.print("ERROR WRITING VALUE!!");
        delay(500);
      }
    break;

    case 2:
      for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
        lcd.setCursor(0, 0);
        lcd.print("Address : ");
        lcd.print(i);
        lcd.setCursor(5, 1);
        if (EEPROM.read(i) != 0) {
          lcd.clear();
          lcd.print("ERROR!");
          delay(500);
          break;
        }
      }

      delay(200);
      lcd.clear();
      lcd.print("EEPROM is clear!");
      delay(3000);
      lcd.clear();
      break;
  }
}

void DHT11() {

  Serial.println(DHTtext[3]);
  Serial.println(DHTtext[4]);
  
  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();

      if (chr == NL || chr == CR) {
        lcd.clear();
        ReturnToline();
        Resume = true;
      }
    }
  }
  Serial.println("Press b To exit");

  while (!exitloop) {

    delay(500);
    DHT.readata(DHT11_PIN);
    switch (DHT.humidity) {
    case -1:
      lcd.clear();
      lcd.print("  ERROR!");
      Serial.println(DHTtext[2]);
      Serial.print("Error Code : ");
      Serial.println(DHT.humidity);
      delay(1000);
      lcd.clear();
      return;
    break;
    case -2:
      lcd.clear();
      lcd.print("  ERROR!");
      Serial.println(DHTtext[0]);
      Serial.println(DHTtext[1]);
      Serial.print("Error Code : ");
      Serial.println(DHT.humidity);
      delay(1000);
      lcd.clear();
      return;
    break;
    }
    lcd.setCursor(0, 0);
    lcd.print("Temp : ");
    lcd.print(DHT.temperature);
    lcd.print(" C ");
    lcd.setCursor(0, 1);
    lcd.print("Humid : ");
    lcd.print(DHT.humidity);
    lcd.print(" % ");

    if (Serial.available()) { 
      chr = Serial.read();

      if (chr == 'b') {
        lcd.clear();
        exitloop = true;
      }
    }
  }
}

void ReturnToline() {
  Serial.write(CR);
  Serial.write(NL);
}

void runRave() {

  Serial.println("Press b To exit");

  while (!exitloop) {
    for (int i = 0; i < 16; i++) {
      lcd.write(255);
      lcd.print(" ");
    }

    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++) {
      lcd.print(" ");
      lcd.write(255);
    }

    lcd.setCursor(0, 0);
    delay(200);

    for (int i = 0; i < 16; i++) {
      lcd.print(" ");
      lcd.write(255);
    }

    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++) {
      lcd.write(255);
      lcd.print(" ");
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

int PinSelect(int MaxDigits) {
  int Pin = 0;
  int c = 0; // Char Counter
  while (!Resume) {
    lcd.setCursor(0, 0);
    lcd.print(" Waiting : Pin");
    if (Serial.available()) {
      chr = Serial.read();
      if (chr == NL || chr == CR) {
        if (c > 0) {
          ReturnToline();
          return Pin;
        }
      } else if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) {
          if(c > 0){
            c = 0;
            Pin = 0;
            lcd.setCursor(7 + c , 1);
            lcd.print("   ");
          }
      } else if (chr == 'b') {
        return Pin = -1;
      } else {
        if (chr >= '0' && chr <= '9') {
          if (c < MaxDigits) {
            c++;
            Pin = (Pin * 10) + chr - 48;
            lcd.setCursor(7, 1);
            lcd.print(Pin);
          }
        }
      }
    }
  }
}