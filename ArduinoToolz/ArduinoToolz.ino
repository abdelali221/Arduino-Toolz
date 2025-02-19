#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>
#include <DFRobot_DHT11.h>
#include <EEPROM.h>
#include <Wire.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 4

LiquidCrystal_I2C lcd(39,  16, 2); // 39 = 0x27

// Time Variables :
int sec = 0;
int min = 0;
int hrs = 0;
int hrs12 = 12;

// Serial buffer :
String data;
int chr;
int irpin;
int exitloop;
int ok = 0;
int ampmflag = 0;
int curs = 4;
int c;
int Pin;
int pingPin;
int pongPin;
const int pinA = 9;
const int pinB = 8;
const int pinC = 7;
const int pinD = 6;
const int pinE = 5;
const int pinF = 4;
const int pinG = 3;
const int pinH = 2;
int VpinE;
int VpinF;
int VpinG;
int VpinH;
int value;
int address;
int RW;
int hby;

byte ball[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B11011,
  B11011,
  B00000
};

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, ball);
  lcd.clear();
  lcd.print("  Arduino Toolz");
  lcd.setCursor(4, 3);
  lcd.print("beta 0.9.1");
  delay(2000);
  lcd.clear();
}

void loop() {

  if (Serial.available()) {
    c = 0;
    ok = 0;
    exitloop = 0;
    address = 0;
    value = 0;
    Pin = 0;
    pingPin = 0;
    irpin = 0;
    lcd.clear();
    CommandSet();
  }
  
  lcd.setCursor(0, 0);
  lcd.print("  Waiting for a");
  lcd.setCursor(5, 3);
  lcd.print("command");
  
}

void CommandSet() {

  data = "";
  StringRead();
  exitloop = 0;
  Serial.write(13);
  Serial.write(10);

  if (data == "Terminal") {
    lcd.clear();
    lcd.print("   Terminal");
    delay(1000);
    lcd.clear();
    Terminal();
  }

  if (data == "Clear") {
    Serial.write(27);
    Serial.print("[2J");
  } 

  else if (data == "Clock") {
    lcd.clear();
    Clock();
  }

  else if (data == "Customchar") {
    lcd.clear();
    lcd.write(byte(0));
    delay(2000);
  }

  else if (data == "LCDinit") {
    LiquidCrystal_I2C lcd(39,  16, 2);
    lcd.init();
    lcd.noBacklight();
    delay(500);
    lcd.backlight();

    for (c = 0; c < 10; c++) {
      delay(150);
      lcd.print("TESTING DISPLAY");
    }

    lcd.clear();
  }

  else if (data == "LCDBacklightOn") {
    lcd.backlight();
  }

  else if (data == "LCDBacklightOff") {
    lcd.noBacklight();
  }

  else if (data == "Analog") {
    lcd.print("   Analog PIN");
    lcd.setCursor(6, 3);
    lcd.print("Tool");
    delay(2000);
    lcd.clear();
    
    
    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print(" Waiting : PIN");

      if (Serial.available()) {

        char chr = Serial.read();
        Serial.print(chr);
        
        if (chr == '\n'  || chr == '\r') {
          ok = 1;
        }

        else {
          
          if (c != 0 && chr != '0') {
            c = c + 1;
          }

          if (c < 2) {
            value = value*10 + chr - 48;
          }

        }
      }  

    }

    ok = 0;

    lcd.clear();
  
    if (Pin < 0 && Pin > 5) {
      lcd.print("   Invalid PIN");
      lcd.setCursor(4, 3);
      lcd.print(" number!");
      delay(1000);
      lcd.clear();
      return;
    }

    analog();
  }

  else if (data == "Digital") {
    lcd.print("  Digital PIN");
    lcd.setCursor(6, 3);
    lcd.print("Tool");
    delay(2000);
    lcd.clear();

    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print(" Waiting : PIN");

      if (Serial.available()) {

        char chr = Serial.read();
        Serial.print(chr);
        
        if (chr == '\n' || chr == '\r') {
          ok = 1;
        }

        else {
          
          if (c != 0 && chr != '0') {
            c = c + 1;
          }

          if (c < 3) {
            Pin = Pin*10 + chr - 48;
          }

        }
      }  

    }


    if (Pin < 0 && Pin > 13) {
      lcd.print("   Invalid PIN");
      lcd.setCursor(4, 3);
      lcd.print(" number!");
      delay(1000);
      lcd.clear();
      return;
    }

    ok = 0;

    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print("Read or Write?  ");
      lcd.setCursor(4, 3);
      lcd.print("R=0  W=1 ");

      if (Serial.available()) {
        ok = 1;
        RW = Serial.read() - 48;
        lcd.print(RW);
        delay(500);
      }
    } 

    if (RW < 0 && RW > 1) {
      lcd.print(" Invalid Choice!");
      delay(1000);
      lcd.clear();
      return;
    }

    lcd.clear();

    digital();
  }

  else if (data == "IR") {

    lcd.setCursor(0, 0);
    lcd.print("IR Receiver");
    delay(2000);
    lcd.clear();

    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print(" Waiting : PIN");

      if (Serial.available()) {

        char chr = Serial.read();
        Serial.print(chr);
        
        if (chr == '\n' || chr == '\r') {
          ok = 1;
        }

        else {
          
          if (c != 0 && chr != '0') {
            c = c + 1;
          }

          if (c < 3) {
            irpin = irpin*10 + chr - 48;
          }

        }

      }

    }
    
    lcd.clear();
    IrReceiver.begin(irpin, ENABLE_LED_FEEDBACK);
    IR();
  }

  else if (data == "Sensor") {
    Sensor();
  }

  else if (data == "UltraR") {

    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print("Waiting : PingPIN");

      if (Serial.available()) {

        char chr = Serial.read();
        Serial.print(chr);
        
        if (chr == '\n' || chr == '\r') {
          ok = 1;
        }

        else {
          
          if (c != 0 && chr != '0') {
            c = c + 1;
          }

          if (c < 3) {
            pingPin = pingPin*10 + chr - 48;
            pongPin = pingPin + 1;
          }

        }

      }

    }


    if (pingPin < 2 || pingPin > 12) {
      lcd.print("   Invalid PIN");
      lcd.setCursor(4, 3);
      lcd.print(" number!");
      delay(1000);
      lcd.clear();
      return;
    }

    ultrasonic();
  }
        

  else if (data == "Keypad") {
        
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinD, OUTPUT);
    pinMode(pinE, INPUT_PULLUP);
    pinMode(pinF, INPUT_PULLUP);
    pinMode(pinG, INPUT_PULLUP);
    pinMode(pinH, INPUT_PULLUP);
    keypad();

  }

  else if (data == "EEPROM") {

    lcd.print("  EEPROM Tool");
    delay(2000);

    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print(" Read or Write?");
      lcd.setCursor(4, 3);
      lcd.print("R=0  W=1 ");

      if (Serial.available()) {
        ok = 1;
        RW = Serial.read() - 48;
        lcd.print(RW);
        delay(500);

        if (RW == 1) {
          
          ok = 0;
          lcd.clear();

          while (ok != 1) {
            lcd.setCursor(0, 0);
            lcd.print("Clear or Write?");
            lcd.setCursor(4, 3);
            lcd.print("C=2  W=1 ");

            if (Serial.available()) {
              ok = 1;
              RW = Serial.read() - 48;
              lcd.print(RW);
              delay(500);
              lcd.clear();
            }
          }
          
        }

        else {
          lcd.clear();
        }

      }

      if (RW < 0 && RW > 1) {
      lcd.print(" Invalid Choice!");
      delay(1000);
      lcd.clear();
      return;
      }

    }

    ok = 0;
    
    if (RW == 0) {

      while (ok != 1) {
        lcd.setCursor(0, 0);
        lcd.print("INT, HEX or BIN?  ");
        lcd.setCursor(0, 3);
        lcd.print(" I=0  H=1  B=2 ");

        if (Serial.available()) {
          ok = 1;
          hby = Serial.read() - 48;
          lcd.print(RW);
          delay(500);
          lcd.clear();
        }
      } 

    }  

    ok = 0;

    if (RW != 2) {

      while (ok != 1) {
        lcd.setCursor(0, 0);
        lcd.print(" Waiting : Addr");

        if (Serial.available()) {

        char chr = Serial.read();
                  
          if (chr == '\n' || chr == '\r') {
            ok = 1;
          }

          else {

            if (c != 0 && chr != 48) {
              Serial.print(chr);
              c = c + 1;
            }

            if (c < 5 ) {
              address = address*10 + chr - 48;
            }

          }
        }  

      }
     
    }
    if (address > EEPROM.length()) {
      lcd.clear();
      lcd.print("Invalid Address!");
      delay(3000);
      lcd.clear();
      return;
    }
        
  EPROM();
  }

  else {
    data = "";
  }

  
}

void Clock() {

  while (exitloop != 1) {

    lcd.setCursor(3, 4);
    lcd.print("Clock");
    delay(979);
    clockcounter();
    lcd.setCursor(curs, 0);

    if (ampmflag == 0) {
      if (hrs < 10) {
        lcd.print("0");
        lcd.print(hrs);
      }

      else {
        lcd.print(hrs);
      }
            
    }
              
    else if (ampmflag == 1) {
      if (hrs12 < 10) {
        lcd.print("0");
        lcd.print(hrs12);
      }

      else {
        lcd.print(hrs12);
      } 

    }

    lcd.print(":"); 
   
    if (min < 10) {
      lcd.print("0");
      lcd.print(min);
    }

    else {
      lcd.print(min);
    }  

    lcd.print(":"); 

    if (sec < 10) {
      lcd.print("0");
      lcd.print(sec);
    }

    else {
      lcd.print(sec);
    }

    if (ampmflag == 1) {
      if (hrs < 12) {
        lcd.print("  AM");
      }

      else {
        lcd.print("  PM");
      }
    
    }

    GetClock();
  
  }

}


void GetClock() {

  if (Serial.available()) {
    char data = Serial.read();

    if (data == 'H') {
      hrs = 0;
      hrs = Serial.read() - 48;
      hrs = hrs*10 + (Serial.read() - 48 );
          
      if (hrs >= 13) {
        hrs12 = hrs - 12;
      }

      else if (hrs < 13) {
        hrs12 = hrs;
      }

    }
         
    else if (data == 'M') {
      min = 0;
      min = Serial.read() - 48;
      min = min*10 + (Serial.read() - 48 );
    }

    else if (data == 'S') {
      sec = 0;
      sec = Serial.read() - 48;
      sec = sec*10 + (Serial.read() - 48 );
    }

    else if (data == 'T') {
      ampmflag = 0;
      ampmflag = Serial.read() - 48;
      lcd.clear();
    }

    else if (data == 'b') {
      lcd.clear();
      exitloop = 1;
    }

    else if (data == 'L') {
      lcd.noBacklight();
    }

    else if (data == 'O') {
      lcd.backlight();
    }

  }

  if (hrs >= 24 || hrs < 0) {
    Serial.print("Invalid clock!");
    Serial.print("Format Hhh");
    hrs = 0;
    hrs12 = 12;
    GetClock();
  } 

  if (hrs12 >= 13) {
    Serial.print("Invalid clock!");
    Serial.print("Format Hhh");
    hrs12 = 12;
    GetClock();
  }

  if (min >= 60 || min < 0) {
    Serial.print("Invalid clock!");
    Serial.print("Format Mmm");
    min = 0;
    GetClock();
  }

  if (sec >= 60 || sec < 0) {
    Serial.print("Invalid clock!");
    Serial.print("Format Sss");
    sec = 0;
    GetClock();
  }

  if (ampmflag > 1 || ampmflag < 0) {
    Serial.print("Invalid setting!");
    Serial.print("O = 12h / 1 = 24h");
    Serial.print("Default will be set (24h)");
    ampmflag = 0;
    GetClock();
  }

}


void clockcounter() {

  if (sec <= 59) {
    sec = sec + 1;
  }

  if (sec == 60) {
    sec = 0;
    min = min + 1;
  }

  if (min == 60) {
    min = 0;
    hrs = hrs + 1;
    hrs12 = hrs12 + 1;
  }

  if (hrs == 24) {
    hrs = 0;
  }

  if (hrs12 == 13) {
    hrs12 = 1;
  }

  if (hrs12 == 0) {
    hrs12 = 12;
  }  
            
  if (ampmflag == 1) {
    curs = 2;
  }

  else {
    curs = 4;
  }
  
}

void Terminal() {

  if (Serial.available()) {

    if (Serial.available() > 0) {

      c = c + 1;

      if (c == 16) {
        lcd.setCursor(0, 3);
      }
 
      if (c == 32) {
        c = 0;
        delay(50);
        lcd.clear();
        lcd.setCursor(0, 0);
      }

      char data = Serial.read();
      
      if (data != '\r' && data != '\n') {
        lcd.write(data);
      }

      else {
        c = 0;
        delay(800);
        lcd.clear();
      }

      Serial.write(data);
      delay(10);
      }


  }

  Terminal();

}

void analog() {


  while (exitloop != 1) {
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
      char data = Serial.read();

      if (data == 'b') {
        lcd.clear();
        exitloop = 1;
      }

    }

  }


}

void digital() {

  ok = 0;

  switch (RW) {

  case 0:

    while (exitloop != 1) {
    
      lcd.setCursor(0, 0);
      lcd.print("PIN D");
      lcd.print(Pin);
      lcd.print(" : ");
      pinMode(Pin, INPUT);
      lcd.print(digitalRead(Pin));

      if (Serial.available()) {
        char data = Serial.read();

        if (data == 'b') {
          lcd.clear();
          exitloop = 1;
        }

      }

    }
  break;

  case 1:

    lcd.setCursor(0, 0);
    lcd.print("    Write");
    delay(1000);
    lcd.clear();
    pinMode(Pin, OUTPUT);

    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print("Waiting : State");

      if (Serial.available()) {
        ok = 1;
        value = Serial.read() - 48;
      }    

    }

    switch (value) {
      case 0:
        digitalWrite(Pin, LOW);
      break;

      case 1:
        digitalWrite(Pin, HIGH);
      break;
    }

  break;

  }

}

void IR() {

  while (exitloop =! 1) {

    if (IrReceiver.decode()) {
      lcd.setCursor(0, 0);
      lcd.println(IrReceiver.decodedIRData.decodedRawData); // Print "old" raw data
      delay(1000);
      lcd.clear();
      IrReceiver.resume(); // Enable receiving of the next value
    }

    if (Serial.available()) {
      char data = Serial.read();

      if (data == 'b') {
        lcd.clear();
        exitloop = 1;
      }

    }

  }

}

void Sensor() {

  while (exitloop =! 1) {

    delay(500);
    DHT.read(DHT11_PIN);
    lcd.setCursor(0, 0);  
    lcd.print("Temperature : ");
    lcd.print(DHT.temperature);
    lcd.setCursor(0, 3);
    lcd.print("Humidity :");
    lcd.println(DHT.humidity);

    if (Serial.available()) {
      char data = Serial.read();

      if (data == 'b') {
        lcd.clear();
        exitloop = 1;
      }

    }

  }

}

void ultrasonic() {

  while (exitloop =! 1) {

    long duration, inches, cm;

    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);

    pinMode(pongPin, INPUT);
    duration = pulseIn(pongPin, HIGH);


    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);

    lcd.setCursor(0, 0);
    lcd.print(inches);
    lcd.print("in, ");
    lcd.print(cm);
    lcd.print("cm     ");
    lcd.setCursor(0, 3);
    lcd.print("PING:D");
    lcd.print(pingPin);
    lcd.setCursor(8, 3);
    lcd.print("PONG:D");
    lcd.print(pongPin);

    delay(100);

    if (Serial.available()) {
      char data = Serial.read();

      if (data == 'b') {
        lcd.clear();
        exitloop = 1;
      }

    }

  }

}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

void keypad() {

  while (exitloop != 1) {

    digitalWrite(pinA, LOW);
    VpinE = digitalRead(pinE);
    VpinF = digitalRead(pinF);
    VpinG = digitalRead(pinG);
    VpinH = digitalRead(pinH);
    digitalWrite(pinA, HIGH);

    if (VpinE == LOW) {
      delay(200);
      lcd.print("1");
    }
    else if (VpinF == LOW) {
      delay(200);
      lcd.print("2");
    } 

    else if (VpinG == LOW) {
      delay(200);
      lcd.print("3");
    }
    
    else if (VpinH == LOW) {
      lcd.setCursor(0, 0);
      lcd.clear();
    }
    
    digitalWrite(pinB, LOW);
    VpinE = digitalRead(pinE);
    VpinF = digitalRead(pinF);
    VpinG = digitalRead(pinG);
    VpinH = digitalRead(pinH);
    digitalWrite(pinB, HIGH);

    if (VpinE == LOW) {
      delay(200);
      lcd.print("4");
    }
  
    else if (VpinF == LOW) {
      delay(200);
      lcd.print("5");
    }
    
    else if (VpinG == LOW) {
      delay(200);
      lcd.print("6");
    }
    
    else if (VpinH == LOW) {
      delay(200);
      lcd.print("B");
    }
  
    digitalWrite(pinC, LOW);
    VpinE = digitalRead(pinE);
    VpinF = digitalRead(pinF);
    VpinG = digitalRead(pinG);
    VpinH = digitalRead(pinH);
    digitalWrite(pinC, HIGH);
    
    if (VpinE == LOW) {
      delay(200);
      lcd.print("7");
    }
    
    else if (VpinF == LOW) {
      delay(200);
      lcd.print("8");
    }
    
    else if (VpinG == LOW) {
      delay(200);
      lcd.print("9");
    }
    
    else if (VpinH == LOW) {
      delay(200);
      lcd.print("C");
    } 
  
    digitalWrite(pinD, LOW);
    VpinE = digitalRead(pinE);
    VpinF = digitalRead(pinF);
    VpinG = digitalRead(pinG);
    VpinH = digitalRead(pinH);
    digitalWrite(pinD, HIGH);

    if (VpinE == LOW) {
      delay(200);
      lcd.print("*");
    }

    else if (VpinF == LOW) {
      delay(200);
      lcd.print("0");
    }

    else if (VpinG == LOW) {
      delay(200);
      lcd.print("#");
    }

    else if (VpinH == LOW) {
      delay(200);
      lcd.print("D");
    }  

    if (Serial.available()) {
      char data = Serial.read();

      if (data == 'b') {
        lcd.clear();
        exitloop = 1;
      }

    }

  }

}

void EPROM() {

  ok = 0;
  lcd.clear();

  switch (RW) {
    case 0:

    value = EEPROM.read(address);    
    lcd.print("  The value of ");
    lcd.setCursor(0, 3);
    lcd.print("Addr ");
    lcd.print(address);
    lcd.print(" is ");
    switch (hby) {
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

    case 1:
    
    while (ok != 1) {
      lcd.setCursor(0, 0);
      lcd.print("   Waiting : ");
      lcd.setCursor(6, 3);
      lcd.print("value");
      
      if (Serial.available()) {

        char chr = Serial.read();
        Serial.print(chr);
        
        if (chr == '\n' || chr == '\r') {
          ok = 1;
        }

        else {

          if (c != 0 && chr != '0') {
            c = c + 1;
          }

          if (c < 4) {
            value = value*10 + chr - 48;
          }
        }
      }  

    }

    EEPROM.write(address, value);
    lcd.clear();
    lcd.print("Value ");
    lcd.print(value);
    lcd.print(" Was ");
    lcd.setCursor(0, 3);
    lcd.print("   written to ");
    delay(2000);
    lcd.clear();
    lcd.print("  Address ");
    lcd.print(address);
    delay(1000);
    lcd.clear();
    break;

    case 2:
      for (int i = 0 ; i < EEPROM.length() ; i++) {
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

void StringRead() {

  while (exitloop != 1) {

    if (Serial.available()) {

      while (Serial.available() > 0) {
        char chr = Serial.read();
        Serial.print(chr);
                
        if (chr == '\n' || chr == '\r') {
          exitloop = 1;
        }

        else {
          data.concat(chr);
        }
    
      }
    }    
  }

}