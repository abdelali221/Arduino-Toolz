#ifndef DHT11_h
#define DHT11_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class DHT11{
public:
  void readata(int pin);
  int humidity;
  int temperature;
};
#endif
