#include <LiquidCrystal.h>
#include "HourglassUtils.h"


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int tiltPin = 8;
const int buzzerPin = 9;
const int potPin = A0;
const int ledPins[] = {13, 10, 7};
const int numLeds = 3;


bool isRunning = false;
unsigned long totalStartSeconds = 0;
unsigned long remainingMillis = 0;
unsigned long previousMillis = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(tiltPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  for (int i = 0; i < numLeds; i++) pinMode(ledPins[i], OUTPUT);
}

void loop() {
  if (!isRunning) {
    // STOPPED STATE
    int potVal = analogRead(potPin);
    totalStartSeconds = map(potVal, 0, 1023, 10, 600); 
    
    lcd.setCursor(0, 0);
    lcd.print("STOPPED       ");
    lcd.setCursor(0, 1);
    formatTime(totalStartSeconds); // Using external utility function
    lcd.print("Set: ");
    lcd.print(timeString);
    lcd.print("      ");

    if (digitalRead(tiltPin) == HIGH) {
      isRunning = true;
      remainingMillis = totalStartSeconds * 1000ULL;
      previousMillis = millis();
      delay(300); 
    }
  } 
  else {
    // RUNNING STATE
    unsigned long currentMillis = millis();
    unsigned long delta = currentMillis - previousMillis;
    previousMillis = currentMillis;

    remainingMillis = (remainingMillis > delta) ? (remainingMillis - delta) : 0;

    lcd.setCursor(0, 0);
    lcd.print("RUNNING...     ");
    lcd.setCursor(0, 1);
    formatTime(remainingMillis / 1000);
    lcd.print("Time: ");
    lcd.print(timeString);

    //  LEDs
    float progress = (float)remainingMillis / (totalStartSeconds * 1000.0);
    digitalWrite(ledPins[0], progress > 0.66);
    digitalWrite(ledPins[1], progress > 0.33);
    digitalWrite(ledPins[2], progress > 0.01);

    if (remainingMillis == 0) {
      playFinishTune();
      isRunning = false;
    }
  }
}

void playFinishTune() {
  lcd.clear();
  lcd.print("FINISHED!");
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 1500);
    delay(200);
    noTone(buzzerPin);
    delay(100);
  }
}