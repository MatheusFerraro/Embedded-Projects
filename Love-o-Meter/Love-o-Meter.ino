class Average {
  private:
    static const int MAX_SAMPLES = 50;
    float samples[MAX_SAMPLES];
    int index;
    int count;
    float sum;

    public:
    Average(){
      reset();
    }

    void reset(){
      index = 0;
      count = 0;
      sum = 0;

      for(int i = 0; i < MAX_SAMPLES; i++) samples[i] = 0;
    }

    void add(float value){
      if (count == MAX_SAMPLES){
        sum -= samples[index];
      } else{
        count++;
      }

      samples[index] = value;
      sum += value;

      index++;
      if(index >= MAX_SAMPLES){
        index = 0;
      }
    }
     
    

      float getAverage(){
        if(count == 0) return 0;
        return sum/count;
      }
    
};

class TemperatureSensor{
  private:
    int pin;
  public:
    TemperatureSensor(int p){
      pin = p;
    }

   // returns RAW value (0 - 1023) because
   //converting Thermistor to Celsius is complex/slow
    float getTemp(){
      return analogRead(pin);
    }
};

class LedController {
  private :
    int pins[3];

  public:
    LedController(int p1, int p2, int p3){
      pins[0] = p1;
      pins[1] = p2;
      pins[2] = p3;
    }

    void init(){
      for (int i = 0; i < 3; i++) pinMode(pins[i], OUTPUT);
    }

    void setTemperature(float delta){

      for(int i = 0; i < 3; i++) digitalWrite(pins[i], LOW);

      if (delta > 2.0) digitalWrite(pins[0], HIGH); // Level 1
      if (delta > 3.0) digitalWrite(pins[1], HIGH); // Level 2
      if (delta > 4.0) digitalWrite(pins[2], HIGH); // Level 3
    }
};

TemperatureSensor tempSensor(A0);
Average avg;
LedController ledController(3, 4, 5);

const int BTN_PIN = 2;

void setup(){
  Serial.begin(9600);

  pinMode(BTN_PIN, INPUT_PULLUP);
  ledController.init();

  Serial.println("Starting ...");

  Serial.println("Calculating Baseline...");
  for(int i=0; i<50; i++){
    avg.add(tempSensor.getTemp());
    delay(20);
  }
  Serial.print("Baseline Ready: ");
  Serial.println(avg.getAverage());
}

void loop(){
  if(digitalRead(BTN_PIN) == LOW){
    Serial.println("Resetting Baseline...");
    avg.reset();

    for(int i=0; i<50; i++) avg.add(tempSensor.getTemp());
    }
    float temperature = tempSensor.getTemp();
    avg.add(temperature);

    float delta = temperature - avg.getAverage();


//debug
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" Avg: "); Serial.print(avg.getAverage());
  Serial.print(" Delta: "); Serial.println(delta);

    ledController.setTemperature(delta);
    delay(100);
  
}
