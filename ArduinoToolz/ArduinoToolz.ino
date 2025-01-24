#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>
#include <DHT.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 4

LiquidCrystal_I2C lcd(0x27,  16, 2);

// Time Variables :
int sec = 0;
int min = 0;
int hrs = 0;
int hrs12 = 12;

// Serial buffer :
int data = 0;

// Flags :
int ampmflag = 0;
int curs = 4;
int c = 0;
int Apin = 0;
int Dpin = 0;
int pingPin = 0;
int pongPin = 0;
int pinA = 2;
int pinB = 3;
int pinC = 4;
int pinD = 5;
int pinE = 6;
int pinF = 7;
int pinG = 8;
int pinH = 9;
int VpinE = 0;
int VpinF = 0;
int VpinG = 0;
int VpinH = 0;


byte ball[] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000
};

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, ball);
  lcd.clear();
  lcd.print("  Arduino Tools");
  lcd.setCursor(4, 3);
  lcd.print("beta 0.5");
  delay(2000);
  lcd.clear();
}

void loop() {

  if (Serial.available() > 0) {
    lcd.clear();
    CommandSet();
  }
  
  
  lcd.setCursor(0, 0);
  lcd.print("  Waiting for a                             command");
 
  loop();
}

void CommandSet() {

  char data = Serial.read();

  switch (data) {
        case 'h':
          lcd.clear();
          lcd.print("HEIL HITLER");
          delay(500);
        break;

        case 'T':
          lcd.clear();
          lcd.print("   Terminal");
          delay(1000);
          lcd.clear();
          monitorMode();
        break;

        case 'C':
          lcd.clear();
          clock();
        break;

        case 'b':
          lcd.clear();
          lcd.write(byte(0));
          delay(2000);
        break;

        case 'A':
          Apin = Serial.read() - 48;
          lcd.print("   Analog PIN");
          lcd.setCursor(5, 3);
          lcd.print("reader");
          delay(2000);
          lcd.clear();

          if (Apin < 0) {
            lcd.print("   Invalid PIN");
            lcd.setCursor(4, 3);
            lcd.print(" number!");
            delay(1000);
            lcd.clear();
            loop();
          }

          if (Apin > 5) {
            lcd.print("   Invalid PIN");
            lcd.setCursor(4, 3);
            lcd.print(" number!");
            delay(1000);
            lcd.clear();
            loop();
          }

          analog();
        break;

        case 'D':
          Dpin = Serial.read() - 48;

          if (Serial.available()) {
            Dpin = Dpin*10 + Serial.read() - 48;
          }

          lcd.print("   Digital PIN");
          lcd.setCursor(5, 3);
          lcd.print("reader");
          delay(2000);
          lcd.clear();

        if (Dpin < 0) {
          lcd.print("   Invalid PIN");
          lcd.setCursor(4, 3);
          lcd.print(" number!");
          delay(1000);
          lcd.clear();
          loop();
        }

        if (Dpin > 13) {
          lcd.print("   Invalid PIN");
          lcd.setCursor(4, 3);
          lcd.print(" number!");
          delay(1000);
          lcd.clear();
          loop();
        }

          digital();
        break;

        case 'I':
          #define IR_RECEIVE_PIN 2
          lcd.setCursor(0, 0);
          lcd.print("IR Receiver");
          delay(2000);
          lcd.clear();
          IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
          ir();
        break;

        case 'S':
          S();
        break;

        case 'U':
          pingPin = Serial.read() - 48;

          if (Serial.available()) {
            pingPin = pingPin*10 + Serial.read() - 48;
          }
          pongPin = pingPin + 1;  

          if (pingPin < 2) {
            lcd.print("   Invalid PIN");
            lcd.setCursor(4, 3);
            lcd.print(" number!");
            delay(1000);
            lcd.clear();
            loop();
          }

          if (pingPin > 12) {
            lcd.print("   Invalid PIN");
            lcd.setCursor(4, 3);
            lcd.print(" number!");
            delay(1000);
            lcd.clear();
            loop();
          }

          ultrasonic();
        break;
        

        case 'K':
        
          pinMode(pinA, OUTPUT);
          pinMode(pinB, OUTPUT);
          pinMode(pinC, OUTPUT);
          pinMode(pinD, OUTPUT);
          pinMode(pinE, INPUT_PULLUP);
          pinMode(pinF, INPUT_PULLUP);
          pinMode(pinG, INPUT_PULLUP);
          pinMode(pinH, INPUT_PULLUP);
          keypad();

        break;
  }
  
          lcd.print("   Unknown");
          delay(500);
  

}


void clock() {

  lcd.setCursor(3, 4);
  lcd.print("Clock");
  delay(988);
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

  getClock();
  clock();
}


void getClock() {

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
      loop();
    }

  }

  if (hrs >= 24) {
    Serial.print("Invalid clock!");
    Serial.print("Format Hhh");
    hrs = 0;
    getClock();
  } 

  if (hrs12 >= 13) {
    Serial.print("Invalid clock!");
    Serial.print("Format Hhh");
    hrs12 = 12;
    getClock();
  }

  else if (min >= 60) {
    Serial.print("Invalid clock!");
    Serial.print("Format Mmm");
    min = 0;
    getClock();
  }

  else if (sec >= 60) {
    Serial.print("Invalid clock!");
    Serial.print("Format Sss");
    sec = 0;
    getClock();
  }

  else if (ampmflag > 1) {
    Serial.print("Invalid setting!");
    Serial.print("O = 12h / 1 = 24h");
    Serial.print("Default will be set (24h)");
    ampmflag = 0;
    getClock();
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

void monitorMode() {

  if (Serial.available()) {

    if (Serial.available() > 0) {
      lcd.write(Serial.read());
      delay(10);
      c = c + 1;
    }


    if (c == 16) {
      lcd.setCursor(0, 3);
      monitorMode();
      }
 
    if (c == 32) {
      c = 0;
      delay(50);
      lcd.clear();
      lcd.setCursor(0, 0);}
      monitorMode();
    }

    else {
      monitorMode();
    }

}

void analog() {

  lcd.setCursor(0, 0);
  lcd.print("PIN A");
  lcd.print(Apin);
  lcd.print(" : ");

  switch (Apin) {
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
          loop();
        }
    }

  analog();
}

void digital() {

  lcd.setCursor(0, 0);
  lcd.print("PIN D");
  lcd.print(Dpin);
  lcd.print(" : ");
  pinMode(Dpin, INPUT);
  lcd.print(digitalRead(Dpin));

    if (Serial.available()) {
      char data = Serial.read();

      if (data == 'b') {
        lcd.clear();
        loop();
      }
    }

  digital();

}

void ir() {
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
      loop();
    }
  }

  ir();
}

void S() {

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
      loop();
    }
  }

  S();
}

void ultrasonic() {

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
      loop();
    }
  }

  ultrasonic();
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

void keypad() {

  digitalWrite(pinA, LOW);
  VpinE = digitalRead(pinE);
  VpinF = digitalRead(pinF);
  VpinG = digitalRead(pinG);
  VpinH = digitalRead(pinH);
  digitalWrite(pinA, HIGH);

    if (VpinE == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("1");
    }
    else if (VpinF == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("2");
    }
    else if (VpinG == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("3");
    }
    else if (VpinH == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("A");
    }

  digitalWrite(pinB, LOW);
  VpinE = digitalRead(pinE);
  VpinF = digitalRead(pinF);
  VpinG = digitalRead(pinG);
  VpinH = digitalRead(pinH);
  digitalWrite(pinB, HIGH);
    if (VpinE == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("4");
    }
    else if (VpinF == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("5");
    }
    else if (VpinG == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("6");
    }
    else if (VpinH == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("B");
    }
  digitalWrite(pinC, LOW);
  VpinE = digitalRead(pinE);
  VpinF = digitalRead(pinF);
  VpinG = digitalRead(pinG);
  VpinH = digitalRead(pinH);  
  digitalWrite(pinC, HIGH);
    if (VpinE == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("7");
    }
    else if (VpinF == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("8");
    }
    else if (VpinG == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("9");
    }
    else if (VpinH == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("C");
    } 
  digitalWrite(pinD, LOW);
  VpinE = digitalRead(pinE);
  VpinF = digitalRead(pinF);
  VpinG = digitalRead(pinG);
  VpinH = digitalRead(pinH);
  digitalWrite(pinD, HIGH);

    if (VpinE == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("*");
    }

    else if (VpinF == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("0");
    }

    else if (VpinG == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("#");
    }

    else if (VpinH == LOW) {
      delay(300);
      lcd.setCursor(0, 0);
      lcd.print("D");
    }  

  if (Serial.available()) {
    char data = Serial.read();

    if (data == 'b') {
      lcd.clear();
      loop();
    }
  }
  keypad();
}
