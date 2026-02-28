class SpaceshipInterface{
  private: 
    int greenPin;
    int redPins[4];
    int trackRedIndex;
    long speedLights;
    long previousMillis;

  public:
    SpaceshipInterface(int gPin, int red1, int red2, int red3, int red4){
      greenPin = gPin;
      redPins[0] = red1;
      redPins[1] = red2;
      redPins[2] = red3;
      redPins[3] = red4;

      trackRedIndex = 0;
      speedLights = 250;
      previousMillis = 0;
    }

    void start(){
      pinMode(greenPin, OUTPUT);
      for(int i = 0; i < 4; i++){
        pinMode(redPins[i], OUTPUT);
      }
    }


    void stop(){
      digitalWrite(greenPin, HIGH);

      for(int i = 0; i < 4; i++){
        digitalWrite(redPins[i], LOW);
      }

      trackRedIndex = 0;
      previousMillis = 0;
    }

    void run(){
      digitalWrite(greenPin, LOW);

      long currentMillis = millis();

      if(currentMillis - previousMillis >= speedLights){

        previousMillis = currentMillis;

        for(int i=0; i<4; i++){
          digitalWrite(redPins[i], LOW);
        }

        digitalWrite(redPins[trackRedIndex], HIGH);

        trackRedIndex++;

        if(trackRedIndex >= 4){
          trackRedIndex = 0;
        }
        
        
      }
    }
};

SpaceshipInterface space(3, 4, 5, 6, 7);

const int PIN_SWITCH = 2;

void setup() {
 pinMode(PIN_SWITCH, INPUT);
 space.start();
 

}

void loop() {
  

  if(digitalRead(PIN_SWITCH) == LOW){
    space.stop();
  }
  else{
    space.run();
  }

}
