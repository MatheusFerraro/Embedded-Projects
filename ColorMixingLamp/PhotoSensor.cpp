#include "PhotoSensor.h"

PhotoSensor::PhotoSensor(const char *name, int pin){
  _name = name;
  _pin = pin;
  _minVal = 1023;
  _maxVal = 0;
}

int PhotoSensor::getRawValue(){
  int val = analogRead(_pin);

  if (val < _minVal) _minVal = val;
  if (val > _maxVal) _maxVal = val;

  return val;
}

int PhotoSensor::getValue(int nRange){
  int val = analogRead(_pin);

  if (val < _minVal) val = _minVal;
  if (val > _maxVal) val = _maxVal;

  long numerator = (long)(val - _minVal) * nRange;
  long denominator = (_maxVal - _minVal);

  if(denominator == 0) return 0;

  return (int)(numerator / denominator);
}


void PhotoSensor::dump() {
  Serial.print(_name);
  Serial.print(": min=");
  Serial.print(_minVal);
  Serial.print(", max=");
  Serial.println(_maxVal);
}