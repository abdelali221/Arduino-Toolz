/*
  Abdelali211's DHT11 driver
  based on DFRobot's project
*/

#include "DHT11.h"

void DHT11::readata(int pin) {
  uint8_t bits[5];
  uint8_t counter = 7;
  uint8_t idx = 0;

  for (int i = 0; i < 5; i++) {
    bits[i] = 0;
  }

	delay(200);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(18);
  digitalWrite(pin, HIGH);
  delayMicroseconds(40);
	digitalWrite(pin,LOW);
  pinMode(pin, INPUT);
  unsigned int loopCounter = 10000;
  if (digitalRead(pin) == LOW) {
  	while(digitalRead(pin) == LOW){
	  	if(loopCounter-- == 0) {
	  		break;
		  }
	  }
  } else {
    humidity = -2;
    return;
  }

	loopCounter = 30000;
  if (digitalRead(pin) == HIGH) {
	  while(digitalRead(pin) == HIGH) {
		  if(loopCounter-- == 0){
		  	break;
		  }
	  }
  } else {
    humidity = -1;
    return;
  }
	for(int i = 0; i < 40; i++) {
		loopCounter = 10000;

    if (digitalRead(pin) == LOW) {
		  while(digitalRead(pin) == LOW) {
		  	if(loopCounter-- == 0){
		  		break;
		  	}
		  }
    } else {
      humidity = -2;
      return;
    }
    unsigned long t = micros();
    if (digitalRead(pin) == HIGH) {
      while(digitalRead(pin) == HIGH) {
        if(loopCounter-- == 0){
		   	  break;
		    }
      }
	  } else {
      humidity = -1;
      return;
    }
  
    if ((micros() - t) > 50) bits[idx] |= (1 << counter);

    if (counter == 0) {
      counter = 7;   
      idx++;      
    } else {
			counter--;
		} 
	}

  humidity    = bits[0]; 
  temperature = bits[2]; 

}
