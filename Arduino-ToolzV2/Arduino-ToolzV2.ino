#include <LiquidCrystal_I2C.h>

// Constants
const int BACK_SPACE = 127;
const int NL = 10; // NewLine command
const int CR = 13; // Carriage Return command
const int ESC = 27;
const int BACK_SPACE1 = 8;
const int LCD_ROWS = 4; // LCD Rows
const int LCD_COLUMNS = 16; // LCD Columns
const int LCD_ADDRESS = 0x27; // LCD Address
const String commandlist[] = {"Terminal", "Help", "Analog", "LCD"}; // Command list

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

/*// Time Variables
int sec = 0; // Seconds
int min = 0; // Minutes
int hrs = 0; // Hours
int hrs12 = 12; // Hours (AM/PM)
*/

// System Variables
String data; // Serial Receive
char chr; // Incoming characters
int chrcount; // String char counter
int c; // Terminal Counter
int Pin; // Pin Variable for Analog/Digital Tools

// Booleans
bool exitloop = false;
bool Resume = false;

void setup() {
  // Initialization sequence
  Serial.begin(9600); // Serial begin at 9600bps
  Serial.write(ESC); // ESC command (Required to send the Clear Screen instruction)
  Serial.print("[2J"); // Clear screen
  lcd.init(); // LCD Screen Init
  lcd.backlight(); // Backlight On
  lcd.clear(); // Clear the screen
  lcd.print("  Arduino Toolz");
  lcd.setCursor(3, 1);
  lcd.print("Release 1.0!");
  delay(2000);
  lcd.clear();
  Serial.print("$>"); // For the Shell
}

void loop() {
  if (Serial.available()) {
    zeroing(); // Sets all System Variables to 0
    lcd.clear();
    lcd.print("$>");
    CommandSet();
  }
}

void CommandSet() {
  data = ""; // Empty the Serial buffer
  StringRead(); // Read the incoming data

  if (data == "Analog") {
    AnalogTool();
  } else if (data == "Terminal") {
    runTerminal();
  } else if (data == "Help") {
    runHelp();
  } else if (data == "LCD") {
    runLCDutility();
  }

  Serial.print("$>");
}

void StringRead() {

  while (!Resume) {
    if (Serial.available()) {
      while (Serial.available() > 0) {
        char chr = Serial.read();

        if (chr == CR || chr == NL) {
          Serial.write(CR);
          Serial.write(NL);
          lcd.clear();
          Resume = true;
        } else {
          if (chr == BACK_SPACE || chr == BACK_SPACE1) {
            if (chrcount > 0) {
              Serial.print("\b \b");
              chrcount--;
              data.remove(chrcount, 1);
            }
          } else {
            if (chrcount < 14) {
              chrcount++;
              Serial.print(chr);
              data.concat(chr);
            }
          }

          if (chrcount <= 14) {
            lcd.setCursor(2, 0);
            lcd.print(data);
          }
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
            if (c < 16) {
              lcd.setCursor(c, 0);
              lcd.print(" ");
              lcd.setCursor(c, 0);
            } else {
              lcd.setCursor(c - 16, 1);
              lcd.print(" ");
              lcd.setCursor(c - 16, 1);
            }
          }
        } else { // LCD Cursor position is controlled using the c variable (c for cursor)
          if (c < 16) {
            lcd.setCursor(c, 0);
          } else if (c > 15) {
            if (c == 32) {
              c = 0;
              delay(50);
              lcd.clear();
              lcd.setCursor(0, 0);
            } else {
              lcd.setCursor(c - 16, 1);
            }
          }

          c++;
          lcd.write(chr);
          Serial.print(chr);
        }
      } else {
        Serial.write(CR);
        Serial.write(NL);
        c = 0;
        lcd.clear();
      }
    }
  }

  runTerminal();
}

void runHelp() {
  // Print the command list through Serial
  Serial.print("// Command list :");
  for (int i = 0; i < sizeof(commandlist) / sizeof(commandlist[0]); i++) {
    Serial.write(CR);
    Serial.write(NL);
    Serial.print(commandlist[i]);
  }
  Serial.write(CR);
  Serial.write(NL);
}

void AnalogTool() {
  while (!Resume) {
    lcd.setCursor(0, 0);
    lcd.print(" Waiting : PIN");

    if (Serial.available()) {
      char chr = Serial.read();
      Serial.print(chr);

      if (chr == NL || chr == CR) {
        Serial.write(CR);
        Serial.write(NL);
        Resume = true;
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

    switch (Pin) {
      case 0:
        lcd.print(analogRead(A0));
        delay(50);
        break;
      case 1:
        lcd.print(analogRead(A1));
        delay(50);
        break;
      case 2:
        lcd.print(analogRead(A2));
        delay(50);
        break;
      case 3:
        lcd.print(analogRead(A3));
        delay(50);
        break;
      case 4:
        lcd.print(analogRead(A4));
        delay(50);
        break;
      case 5:
        lcd.print(analogRead(A5));
        delay(50);
        break;
    }

    if (Serial.available()) {
      char chr = Serial.read();

      if (chr == 'b') {
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
