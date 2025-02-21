#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Constants
const int BACK_SPACE = 127;
const int NL = 10; // NewLine command
const int CR = 13; // Carriage Return command
const int ESC = 27;
const int BACK_SPACE1 = 8;
const int LCD_ROWS = 4; // LCD Rows
const int LCD_COLUMNS = 16; // LCD Columns
const int LCD_ADDRESS = 0x27; // LCD Address
const String commandlist[] = {"Analog", "Digital", "EEPROM", "Help", "LCD", "Terminal"}; // Command list
const String welcome[] = {"// Arduino Toolz", "Proudly developped by Abdelali221", "Ver 2.0 (New Release/Entirely rewritten)", "Github : https://github.com/abdelali221/", "There is a list of the commands :"}; // Welcome Text

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Soon :
/*// Time Variables
int sec = 0; // Seconds 
int min = 0; // Minutes
int hrs = 0; // Hours
int hrs12 = 12; // Hours (AM/PM)
*/

// System Variables
char chr; // Incoming characters
int chrcount; // String char counter
int c; // Char Counter
int Pin; // Pin Variable for Analog/Digital Tools
int ReadWrite_Switch;
int value; // EEPROM Read/Write
int address; // EEPROM Address
int EVF; // EEPROM Value Format Switch

// Booleans
bool exitloop = false;
bool Resume = false;

void setup() {
  // Initialization sequence
  Serial.begin(9600); // Serial begin at 9600bps
  Serial.write(ESC); // ESC command (Required to send the Clear Screen instruction)
  Serial.print("[2J");
  serialwelcome(); // Clear screen
  lcd.init(); // LCD Screen Init
  lcd.backlight(); // Backlight On
  lcd.clear(); // Clear the screen
  lcd.print("  Arduino Toolz");
  lcd.setCursor(3, 1);
  lcd.print("Release 2.0");
  delay(2000);
  lcd.clear();
  Serial.print("$>"); // For the Shell
}

void loop() {
  if (Serial.available()) { // Wait for incoming data
    zeroing(); // Sets all System booleans for loop and wait to false
    lcd.clear();
    lcd.print("$>"); 
    CommandSet(); // Recieve and manage the data
  }
}

void CommandSet() {
  
  String data = StringRead(); // Read the incoming data

    // Compares the data to the available commands :
  if (data == "Analog") {
    AnalogTool();
  } else if (data == "Digital") {
    DigitalTool();
  } else if (data == "EEPROM") {
    runEEPROM();
  } else if (data == "Help") {
    runHelp();
  } else if (data == "LCD") {
    runLCDutility();
  } else if (data == "Terminal") {
    runTerminal();
  } 
  Serial.print("$>");
}

String StringRead() {

  String data;
  data = ""; // Reset the String
  chrcount = 0; // Reset the char counter

  while (!Resume) { // Resume is the wait flag
    if (Serial.available()) {
      while (Serial.available() > 0) {
        char chr = Serial.read(); // Read the data char by char

          // Comparing the data (receive char) to some commands
        if (chr == CR || chr == NL) { // Return button
          Serial.write(CR); // Sends Cariage return to the client terminal
          Serial.write(NL); // Sends New Line to the client terminal
          lcd.clear();
          return data; // Returns the String to compare
        } else if (chr == BACK_SPACE || chr == BACK_SPACE1) { // Backspace
          if (chrcount > 0) { // This is to make sure you don't delete the $>, and so the charcount doesn't become negative
            Serial.print("\b \b"); // Backspace command
            chrcount--; 
            data.remove(chrcount, 1); // Remove the last char of the string
            chr = 0; // Resets the value of char
            lcd.clear();
            lcd.print("$>");
            lcd.print(data); // Re-print the String on the lcd screen
          }
        } else {
          if (chrcount < 14 && chrcount >= 0) { // Chars can be added to the String
            chrcount++;
            Serial.print(chr);
            data.concat(chr); // Adds the latest char to the string
          } 
        }

        if (chrcount <= 14) { // Makes sure you're within the limit of the LCD screen
          lcd.setCursor(2, 0);
          lcd.print(data);
          lcd.setCursor(0, 1);
        }
      }
    }
  }
}

void runTerminal() {
  if (Serial.available()) {
    if (Serial.available() > 0) {
      char chr = Serial.read(); // Read the data character by character

      if (chr != CR && chr != NL) { // Verify if there is no NL or CR
        if (chr == BACK_SPACE || chr == BACK_SPACE1) {
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
  Serial.print("// Command list :");
  for (int i = 0; i < sizeof(commandlist) / sizeof(commandlist[0]); i++) { // Cycle through each Value of the array
    Serial.write(CR);
    Serial.write(NL);
    Serial.print(commandlist[i]);
  }
  Serial.write(CR);
  Serial.write(NL);
}

void DigitalTool() {
  while (!Resume) {
    Pin = 0; // Reset Pin
    lcd.setCursor(0, 0);
    lcd.print(" Waiting : PIN");

    if (Serial.available()) {
      char chr = Serial.read();
      Serial.print(chr);

      if (chr == NL || chr == CR) {
        Serial.write(CR);
        Serial.write(NL);
        Resume = true; // Exit from wait state
      } else {
        if (c != 0 && chr != '0') { // Doesn't add 1 to c if the first value sent is 0
          c++;
        } else if (c < 3) {
          Pin = Pin * 10 + chr - 48; // Multiply the old value by 10 then add the new value
        }
      }
    }
  }

  lcd.clear();
  
  if (Pin < 0 || Pin > 13) { // Checks if the Pin number is valid
    lcd.print("   Invalid PIN");
    lcd.setCursor(4, 1);
    lcd.print(" number!");
    delay(1000);
    lcd.clear();
    return;
  }

  zeroing();

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
  zeroing();

  switch (ReadWrite_Switch) {

    case 0: // Read

      while (!exitloop) {

        lcd.setCursor(0, 0);
        lcd.print("PIN D");
        lcd.print(Pin);
        lcd.print(" : ");
        pinMode(Pin, INPUT);
        lcd.print(digitalRead(Pin));

        if (Serial.available()) {
          char chr = Serial.read();

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

      while (!Resume) {
        lcd.setCursor(0, 0);
        lcd.print("Waiting : State");

        if (Serial.available()) {
          char chr = Serial.read();
          Serial.print(chr);

          if (chr == NL || chr == CR) {
            Serial.write(CR);
            Serial.write(NL);
            lcd.clear();
            Resume = true;
          } else if (chr == BACK_SPACE || chr == BACK_SPACE1) {
            c--;
            Pin = 0;
            Serial.print("\b \b");
          } else {
            if (c != 0 && chr != '0') {
            c++;
            } else if (c < 3) {
              Pin = Pin * 10 + chr - 48; // Multiply the old value by 10 then add the new value
            }
          }
        }
      }

      switch (chr) {
        case '0':
          digitalWrite(Pin, LOW);
        break;

        case '1':
          digitalWrite(Pin, HIGH);
        break;
      }

    break;
  }
}  

void AnalogTool() {
  while (!Resume) {
    Pin = 0;
    lcd.setCursor(0, 0);
    lcd.print(" Waiting : PIN");

    if (Serial.available()) {
      char chr = Serial.read();
      Serial.print(chr);

      if (chr == NL || chr == CR) { // Already explained in Digital
        Serial.write(CR);
        Serial.write(NL);
        Resume = true;
      } else if (chr == BACK_SPACE || chr == BACK_SPACE1) {
        c--;
        Pin = 0;
        Serial.print("\b \b");
      } else {
        if (c != 0 && chr != '0') {
          c++;
        } else if (c < 2) {
          Pin = Pin * 10 + chr - 48; // Multiply the old value by 10 then add the new value
        }
      }
    }
  }

  lcd.clear();

  if (Pin < 0 || Pin > 5) { // Checks if the Pin number is valid
    lcd.print("   Invalid PIN");
    lcd.setCursor(4, 1);
    lcd.print(" number!");
    delay(1000);
    lcd.clear();
    return;
  }

  zeroing();

  while (!exitloop) {
    lcd.setCursor(0, 0);
    lcd.print("PIN A");
    lcd.print(Pin);
    lcd.print(" : ");

    switch (Pin) { // Checks what Pin was selected
      case 0:
        lcd.print(analogRead(A0));
        lcd.print(" ");
        delay(50);
        break;
      case 1:
        lcd.print(analogRead(A1));
        delay(50);
        lcd.print(" ");
        break;
      case 2:
        lcd.print(analogRead(A2));
        delay(50);
        lcd.print(" ");
        break;
      case 3:
        lcd.print(analogRead(A3));
        delay(50);
        lcd.print(" ");
        break;
      case 4:
        lcd.print(analogRead(A4));
        delay(50);
        lcd.print(" ");
        break;
      case 5:
        lcd.print(analogRead(A5));
        delay(50);
        lcd.print(" ");
        break;
    }

    if (Serial.available()) {
      char chr = Serial.read();

      if (chr == 'b') { // Exits if b is sent
        lcd.clear();
        exitloop = true;
      }
    }
  }
}

void zeroing() {
  exitloop = false; // Init the Stay in loop
  Resume = false; // Init the Wait For action
}

void runLCDutility() {
  Serial.print("// LCDutility");
  Serial.write(CR);
  Serial.write(NL);
  Serial.print("What you want to do?");
  Serial.write(CR);
  Serial.write(NL);
  Serial.print(" 1 - Init the LCD screen. 2 - Turn the Backlight on/off. 3 - Enable Cursor.");
  zeroing();
  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();
      Serial.print(chr);
      Serial.write(CR);
      Serial.write(NL);
      Resume = true;
    }
  }
  zeroing();

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
  }

  else if (chr == '2') { // 2 = Backlight
    Serial.write(CR);
    Serial.write(NL);
    Serial.print("ON or OFF? 1 - ON / 2 - OFF");
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
  }

  else if (chr == '3') { // 3 = Cursor toggle
    lcd.cursor_on();
  }

}

void runEEPROM() {
  lcd.print("  EEPROM Tool");
  delay(2000);

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

          zeroing();
          lcd.clear();

          while (!Resume) {
            lcd.setCursor(0, 0);
            lcd.print("Clear or Write?");
            lcd.setCursor(4, 1);
            lcd.print("C=2  W=1 ");

            if (Serial.available()) {
              zeroing();
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

      if (ReadWrite_Switch < 0 || ReadWrite_Switch > 1) { // Already explained in Digital/Analog
        lcd.print(" Invalid Choice!");
        delay(1000);
        lcd.clear();
        return;
      }
    }

    zeroing();

    if (ReadWrite_Switch == 0) {

      while (!Resume) { // Select the format
        lcd.setCursor(0, 0);
        lcd.print("INT, HEX or BIN?  ");
        lcd.setCursor(0, 1);
        lcd.print(" I=0  H=1  B=2 ");

        if (Serial.available()) {
          Resume = true;
          EVF = Serial.read() - 48; 
          lcd.print(ReadWrite_Switch);
          delay(500);
          lcd.clear();
        }
      }
    }

    zeroing();

    if (ReadWrite_Switch != 2) {
        // Reads the Address
      while (!Resume) {
        lcd.setCursor(0, 0);
        lcd.print(" Waiting : Addr");

        if (Serial.available()) {

          char chr = Serial.read();
          Serial.print(chr);

          if (chr == '\n' || chr == '\r') {
            Serial.write(13);
            Serial.write(10);
            Resume = true;
          }

          else {

            if (c != 0 && chr != 48) {
              Serial.print(chr);
              c = c + 1;
            }

            if (c < 5) {
              address = address * 10 + chr - 48;
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

  zeroing();

  lcd.clear();

  switch (ReadWrite_Switch) {
    case 0: // Read

      value = EEPROM.read(address);
      lcd.print("  The value of ");
      lcd.setCursor(0, 1);
      lcd.print("Addr ");
      lcd.print(address);
      lcd.print(" is ");
      switch (EVF) {
        case 0:
          lcd.print(value);
          break;

        case 1:
          lcd.print(value, HEX);
          break;

        case 2:
          delay(2000);
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

          char chr = Serial.read();
          Serial.print(chr);

          if (chr == '\n' || chr == '\r') {
            Resume = true;
          }

          else {

            if (c != 0 && chr != '0') {
              c = c + 1;
            }

            if (c < 4) {
              value = value * 10 + chr - 48;
            }
          }
        }
      }

      EEPROM.write(address, value);
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
      break;

    case 2:
      for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
        lcd.setCursor(0, 0);
        lcd.print("Address : ");
        lcd.print(i);
      }
      lcd.clear();
      lcd.print("EEPROM is clear!");
      delay(3000);
      lcd.clear();
      break;
  }
}

void serialwelcome() {
  // Prints the Welcome Sequence over Serial :
  for (int i = 0; i < sizeof(welcome) / sizeof(welcome[0]); i++) {
    Serial.print(welcome[i]);
    Serial.write(CR);
    Serial.write(NL);
  }  
  Serial.write(CR);
  Serial.write(NL);
  runHelp();
  Serial.write(CR);
  Serial.write(NL);
}