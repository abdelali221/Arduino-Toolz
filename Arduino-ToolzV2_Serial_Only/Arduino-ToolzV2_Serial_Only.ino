//  Arduino Toolz
// Written by Abdelali221

#include <EEPROM.h>
#include <Wire.h>
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
const int BELL = 7;


const char* commandlist[] = 

{ " Analog",
  " Cls",
  " DHT11",
  " Digital",
  " EEPROM",
  " Help",
  " Intro",
  " UltraR",
  "\0"
}; // Command list

const char* welcome[] = 

{ "// Arduino Toolz",
  " Proudly developped by Abdelali221",
  " Ver 2.1",
  " Github : https://github.com/abdelali221/",
  " This is all the available commands :",
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

// Booleans
bool exitloop = false;
bool Resume = false;

void setup() {
    // Initialization sequence
  Serial.begin(9600); // Serial begin at 9600bps
  ClearScreen();

    // Welcome Screen :
  serialwelcome(); 
  Serial.print("$>"); // For the Shell
}

void loop() {
  if (Serial.available()) { // Wait for incoming data
    noexitloop(); // Sets all System booleans for loop and wait to false
    CommandSet(); // Recieve and manage the data
  }
}

void CommandSet() {
  
  char command[20]; // Array to store the command
  StringRead(command, sizeof(command)); // Read the incoming data
  ReturnToline();

    // Compares the data to the available commands :
  if (strcmp(command, "Analog") == 0) {
    AnalogTool();
  } else if (strcmp(command, "Bell") == 0) {
    Serial.write(BELL);
  } else if (strcmp(command, "Cls") == 0) {
    ClearScreen();
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
  } else if (strcmp(command, "UltraR") == 0) {
    runUltraR();
  } else {
    Serial.println("Invalid command!");
  }
  ReturnToline();
  Serial.print("$>");
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
          ReturnToline();
          return;
        }
      } else if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) { // Backspace
        if (c > 0) {
          Serial.print("\b \b"); // Erase character on serial monitor
          c--;
        }
      } else { 
        if (c < maxLength) {
          buffer[c++] = chr;
          Serial.print(chr);
        }
      }
    }
  }
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

  int ReadWrite_Switch = 0;
  char chr = 0;
  int Pin = PinSelect(2, 13); // Pin Variable for Analog/Digital Tools
  int read;
  int ANSread = 2;

  if (Pin == -1) {
    Serial.println("Canceled.");
    return;
  }
    
  if (Pin < 0 || Pin > 13) { // Checks if the Pin number is valid
    Serial.println("   Invalid PIN number!");
    delay(1000);
    return;
  }

  noexitloop();
  Serial.println("Read or Write?  ");
  Serial.print("R=0  W=1: ");

  while (!Resume) {
    if (Serial.available()) {
      Resume = true;
      ReadWrite_Switch = Serial.read() - 48; // Read the instruction
      Serial.println(ReadWrite_Switch); // Print the selected instruction
      delay(500);
    }
  } 

  if (ReadWrite_Switch < 0 || ReadWrite_Switch > 1) { // Checks if the value is valid
    Serial.println(" Invalid Choice!");
    delay(1000);
    return;
  }

  noexitloop();
  Serial.println("Press b To exit");
  chr = '0';

  switch (ReadWrite_Switch) {

    case 0: // Read

      Serial.println("Read");
      delay(1000);
      Serial.print("PIN D");
      Serial.print(Pin);
      Serial.print(" : ");
            
      while (!exitloop) {

        pinMode(Pin, INPUT);
        read = digitalRead(Pin);
        
        if (read != ANSread) {
          Serial.print(read);
          for (int i = 0; i < 1; i++) {
            Serial.print("\b");
          }
        }
        ANSread = read;

        if (Serial.available()) {
          chr = Serial.read();

          if (chr == 'b') {
            exitloop = true;
          }
        }
      }
    break;

    case 1: // Write

      Serial.println("    Write");
      delay(1000);
      pinMode(Pin, OUTPUT);
      Serial.print("PIN D");
      Serial.print(Pin);
      Serial.print(" : ");
      Serial.print("LOW ");
      
      
      while (!exitloop) {
        
        if (Serial.available()) {
          chr = Serial.read();
        }
                
        switch (chr) {
          case '0':
            for (int i = 0; i < 4; i++) {
              Serial.print("\b \b");
            }
            Serial.print("LOW ");
            digitalWrite(Pin, LOW);
          break;

          case '1':
            for (int i = 0; i < 4; i++) {
              Serial.print("\b \b");
            }
            Serial.print("HIGH");
            digitalWrite(Pin, HIGH);
          break;

          case 'b':
            ReturnToline();
            exitloop = true;
          break;
        }

        chr = 2;
        
      }
    break;
  }
}  

void AnalogTool() {

  int Pin = PinSelect(1, 5); // Pin Variable for Analog/Digital Tools
  int buffer;
  
  if (Pin < 0 || Pin > 5) { // Checks if the Pin number is valid or the user canceled the operation
    if (Pin == -1) {
      Serial.println("Canceled.");
      return;
    } else {
      Serial.print("Invalid PIN number!");
      delay(1000);
      return;
    }
  }

  noexitloop();
  int APIN = Pin + 14;
  Serial.println("Press b To exit");
  Serial.print("PIN A");
  Serial.print(Pin);
  Serial.print(" : ");
  
  while (!exitloop) {
      
    buffer = analogRead(APIN);
    if (buffer < 1000 && buffer >= 100) {
    Serial.print("0");
    } else if (buffer < 100 && buffer >= 10) {
      Serial.print("00");
    } else if (buffer < 10) {
      Serial.print("000");
    }
    Serial.print(buffer);
    delay(50);
    for (int i = 0; i < 4; i++) {
      Serial.print("\b");
    }

    if (Serial.available()) {
      char chr = Serial.read();

      if (chr == 'b') { // Exits if b is sent
        ReturnToline();
        exitloop = true;
      }
    }
  }
}

void noexitloop() {
  exitloop = false; // Init the Stay in loop
  Resume = false; // Init the Wait For action
}

void runEEPROM() {

  int ReadWrite_Switch = 0;
  int value = 0; // EEPROM Read/Write
  int address = 0; // EEPROM Address
  int EVF = 0; // EEPROM Value Format Switch

  Serial.println("  EEPROM Tool");
  ReturnToline();
  delay(2000);
  noexitloop();
  Serial.println(" Read or Write or Clear?");
  Serial.print("R=0  W=1  C=2 : ");

  while (!Resume) { // Select the format
    if (Serial.available()) {
      Resume = true;
      ReadWrite_Switch = Serial.read() - 48; 
      Serial.println(ReadWrite_Switch);
      delay(500);
    }
  }

  if (ReadWrite_Switch < 0 || ReadWrite_Switch > 2) { // Already explained in Digital/Analog
    Serial.println(" Invalid Choice!");
    delay(1000);
    return;
  }

  noexitloop();

  if (ReadWrite_Switch == 0) {
    Serial.println("INT, HEX or BIN?  ");
    Serial.print(" I=0  H=1  B=2 : ");

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
    Serial.println(" Invalid Choice!");
    delay(1000);
    return;
  }
  
  noexitloop();

  if (ReadWrite_Switch != 2) {
      // Reads the Address
    address = AddrSelect(EEPROM.length());
    if (address == -1) {
      Serial.println("Canceled.");
      return;
    }
  }
  
  if (address > EEPROM.length()) { // Compares the address to the EEPROM length
    Serial.println("Invalid Address!");
    delay(3000);
    return;
  }

  noexitloop();

  switch (ReadWrite_Switch) {
    case 0: // Read

      value = EEPROM.read(address);
      Serial.print("  The value of Addr");
      Serial.print(address);
      Serial.print(" is ");
      
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

    value = ValSelect(255);

      if (value < 0 || value > 255) {
        Serial.println("Invalid Value!");
        delay(3000);
        return;
      }

      noexitloop();
      EEPROM.write(address, value);
      if (value == EEPROM.read(address)) {
        Serial.print("Value ");
        Serial.print(value);
        Serial.print(" Was written to Address : ");
        Serial.print(address);
        ReturnToline();
        delay(1000);
      } else {
        Serial.println("ERROR WRITING VALUE!!");
        delay(500);
      }
    break;

    case 2:
      for (int i = 0; i <= EEPROM.length(); i++) {
        EEPROM.write(i, 0);
        Serial.print("Address : ");
        Serial.println(i);
        if (EEPROM.read(i) != 0) {
          Serial.println("ERROR!");
          delay(500);
          break;
        }
      }

      delay(200);
      Serial.println("EEPROM is clear!");
      delay(3000);
      break;
  }
}

void DHT11() {

  char chr = 0;
  Serial.println(DHTtext[3]);
  Serial.println(DHTtext[4]);
  
  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();

      if (chr == NL || chr == CR) {
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
      Serial.println(DHTtext[2]);
      Serial.print("Error Code : ");
      Serial.println(DHT.humidity);
      delay(1000);
      return;
    break;
    case -2:
      Serial.println(DHTtext[0]);
      Serial.println(DHTtext[1]);
      Serial.print("Error Code : ");
      Serial.println(DHT.humidity);
      delay(1000);
      return;
    break;
    }
    Serial.print("Temp : ");
    Serial.print(DHT.temperature);
    Serial.print(" C /");
    Serial.print(" Humid : ");
    Serial.print(DHT.humidity);
    Serial.print(" % ");
    Serial.write(CR);

    if (Serial.available()) { 
      chr = Serial.read();

      if (chr == 'b') {
        ReturnToline();
        exitloop = true;
      }
    }
  }
}

void ReturnToline() {
  Serial.write(CR);
  Serial.write(NL);
}

int PinSelect(int MaxDigits, int MaxValue) {
  char chr = 0;
  int Pin = 0;
  int c = 0; // Digits Counter
  Serial.println("Which Pin? ");
  Serial.print("0-");
  Serial.print(MaxValue);
  Serial.print(" : ");

  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();
      if (chr == NL || chr == CR) {
        if (c > 0) {
          ReturnToline();
          Resume = true;
        }
      } else if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) {
          if(c > 0){
            for (int i = 0; i < c; i++) {
              Serial.print("\b \b");
            }
            c = 0;
            Pin = 0;
          }
      } else if (chr == 'b') {
        return Pin = -1;
      } else {
        if (chr >= '0' && chr <= '9') {
          if (c < MaxDigits) {
            for (int i = 0; i < c; i++) {
              Serial.print("\b \b");
            }
            c++;
            Pin = (Pin * 10) + chr - 48;
            Serial.print(Pin);
          }
        }
      }
    }
  }
  return Pin;
}

void runUltraR() {

  int Pin = PinSelect(2, 13);

  if (Pin < 0 || Pin > 12) {
    if (Pin == -1) {
      Serial.println("Canceled.");
      return;
    } else {
        Serial.print("   Invalid PIN number!");
        delay(1000);
      return;
    }
  }

  Serial.println("Press b To exit");

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
        Serial.print("0");
      } else if (cm < 10) {
        Serial.print("00");
      }
      Serial.print(cm);
      Serial.print("cm");
      for (int i = 0; i < 5; i++) {
      Serial.print("\b");
    }
    } else {
      Serial.print("Out Of Range!");
      delay(500);
      for (int i = 0; i < 13; i++) {
      Serial.print("\b \b");
    }
    }

    delay(100);

    if (Serial.available()) {
      char chr = Serial.read();

      if (chr == 'b') {
        ReturnToline();
        exitloop = true;
      }

    }

  }

}

long ms2cm(long microseconds) {
  return microseconds / 29 / 2;
}

int AddrSelect(int MaxValue) {
  char chr = 0;
  int Addr = 0;
  int c = 0; // Char Counter
  Serial.println("Which Address? ");
  Serial.print("0-");
  Serial.print(MaxValue);
  Serial.print(" : ");

  while (!Resume) {
    if (Serial.available()) {
      chr = Serial.read();
      if (chr == NL || chr == CR) {
        if (c > 0) {
          ReturnToline();
          Resume = true;
        }
      } else if (chr == BACK_SPACE || chr == BACK_SPACE_ALT) {
          if(c > 0){
            for (int i = 0; i < c; i++) {
              Serial.print("\b \b");
            }
            c = 0;
            Addr = 0;
          }
      } else if (chr == 'b') {
        return Addr = -1;
      } else {
        if (chr >= '0' && chr <= '9') {
          if (c < 4) {
            for (int i = 0; i < c; i++) {
              Serial.print("\b \b");
            }
            c++;
            Addr = (Addr * 10) + chr - 48;
            Serial.print(Addr);
          }
        }
      }
    }
  }
  noexitloop();
  return Addr;
}

int ValSelect(int MaxValue) {

  int value = 0;
  int c = 0;
  
  Serial.println("What value?");
  Serial.print("0-");
  Serial.print(MaxValue);
  Serial.print(" : ");

  while (!exitloop) {
    if (Serial.available()) {

      char chr = Serial.read();
      Serial.print(chr);

      if (chr == NL || chr == CR) {
        ReturnToline();
        exitloop = true;
      } else {
        if (c != 0 && chr != '0') {
         c++;
        }
        if (c < 4) {
          value = value * 10 + chr - 48;
        }
      }
    }
  }
  noexitloop();
  return value;
}

void ClearScreen() {
  Serial.write(ESC); // ESC command (Required to send the Clear Screen instruction)
  Serial.print("[2J"); // Clears the Terminal
  Serial.write(ESC); // ESC command (Required to send the Clear Screen instruction)
  Serial.print("[0;0f"); // Sets the Cursor to 0;0
}