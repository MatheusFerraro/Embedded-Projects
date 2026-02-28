#include "PhotoSensor.h"

#define RED_SENSOR_PIN A0
#define GREEN_SENSOR_PIN A1
#define BLUE_SENSOR_PIN  A2

#define RED_LED_PIN      9
#define GREEN_LED_PIN    10
#define BLUE_LED_PIN     11


PhotoSensor redSensor("red", RED_SENSOR_PIN);
PhotoSensor greenSensor("green", GREEN_SENSOR_PIN);
PhotoSensor blueSensor("blue", BLUE_SENSOR_PIN);


void setup(){
  Serial.begin(9600);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  Serial.println("Cover sensors, then expose to light");

  unsigned long startTime = millis();
  int lastSecond = -1;

  while(millis() - startTime < 10000){

    int secondsRemaining = 10 - ((millis() - startTime) / 1000);

    if(secondsRemaining != lastSecond){
       Serial.print(secondsRemaining);
       Serial.println(" seconds remaining...");
       lastSecond = secondsRemaining;
    }


    redSensor.getRawValue();
    greenSensor.getRawValue();
    blueSensor.getRawValue();
  }

  redSensor.dump();
  greenSensor.dump();
  blueSensor.dump();
}


void loop(){
  
  int redVal = redSensor.getValue(255);
  int greenVal = greenSensor.getValue(255);
  int blueVal = blueSensor.getValue(255);


  analogWrite(RED_LED_PIN, redVal);
  analogWrite(GREEN_LED_PIN, greenVal);
  analogWrite(BLUE_LED_PIN, blueVal);

  static unsigned long lastPrintTime = 0;

  if (millis() - lastPrintTime > 500) {
    Serial.print("R: "); Serial.print(redVal);
    Serial.print("\t G: "); Serial.print(greenVal);
    Serial.print("\t B: "); Serial.println(blueVal);
    
    lastPrintTime = millis();
  }

}