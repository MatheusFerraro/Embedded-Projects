#ifndef PHOTOSENSOR_H
#define PHOTOSENSOR_H

#include <Arduino.h>

class PhotoSensor{
  private:
    int _pin;
    const char* _name;
    int _minVal;
    int _maxVal;

    public:
      PhotoSensor(const char *name, int pin);

      int getRawValue();
      int getValue(int nRange);
      void dump();

};

#endif