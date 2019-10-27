#include <QueueArray.h>

#define VBATPIN   A9
#define CLOCK_25  true
#define CLOCK_40  false

#define START_PIN 3
#define MODE_PIN  2
#define MOTOR_PIN 5





class Timer;



bool vibrating = false;
int vibrateUntil = -1;

class Vibration {
  public:
    int time, length;
    Vibration(int t, int l) {
      time = t;
      length = l;
    }
};

void setup() {
  // put your setup code here, to run once:
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);
  pinMode(MOTOR_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(START_PIN), start, FALLING);
}

class Timer {
  protected:
    int start;
    QueueArray<Vibration> vibrations;
  public:
    Timer(int s, QueueArray<Vibration> vibs){
      start = s;
      vibrations = vibs;
    }
    void performIteration(int currentTime) {
      if (this->start + this->vibrations.peek().time < currentTime){
        Vibration vibration = this->vibrations.pop();
        digitalWrite(MOTOR_PIN, HIGH);
        vibrating = true;
        vibrateUntil = currentTime + vibration.length;
      }
    }
};

//class Timer25 : public Timer {
//  public:
//    Timer25(int s){
//      start = s;
//      
//      vibrations.push(*new Vibration(15000,500));
//      vibrations.push(*new Vibration(20000,500));
//      vibrations.push(*new Vibration(21000,500));
//      vibrations.push(*new Vibration(22000,500));
//      vibrations.push(*new Vibration(23000,500));
//      vibrations.push(*new Vibration(24000,500));
//      vibrations.push(*new Vibration(25000,5000));
//      
//    }
//};
//
//class Timer40 : public Timer {
//  public:
//    Timer40(int s){
//      start = s;
//      
//      vibrations.push(*new Vibration(15000,500));
//      vibrations.push(*new Vibration(30000,500));
//      vibrations.push(*new Vibration(35000,500));
//      vibrations.push(*new Vibration(36000,500));
//      vibrations.push(*new Vibration(37000,500));
//      vibrations.push(*new Vibration(38000,500));
//      vibrations.push(*new Vibration(39000,500));
//      vibrations.push(*new Vibration(40000,5000));
//    }
//};

int counter = 0;

Timer* currentTimer = reinterpret_cast<const Timer*>(NULL);
Timer* nextTimer = reinterpret_cast<const Timer*>(NULL);

void loop() {
  //====================================================================
  //STATUS
  //====================================================================
  Serial.println(counter++);
  
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);
  
  Serial.print("MODE: ");Serial.println(digitalRead(MODE_PIN));
  Serial.print("START: ");Serial.println(digitalRead(START_PIN));
  Serial.println();

  //====================================================================
  //LOGIC
  //====================================================================
  int currentTime = millis();
  
  if (vibrating && currentTime > vibrateUntil) {
    digitalWrite(MOTOR_PIN, LOW);
    vibrating = false;
  }

  // start a new timer
  if (nextTimer != NULL){
    currentTimer = nextTimer;
    nextTimer = NULL;

    // start vibrating
    digitalWrite(MOTOR_PIN, HIGH);
    vibrating = true;
    vibrateUntil = currentTime + 500; // vibrate for half a second

    // preempt remaining timer actions
    delay(100);
    return;
  }

  // perform timer actions
  currentTimer->performIteration(currentTime);

  delay(100);
  return;
}

void start() {
  nextTimer = digitalRead(MODE_PIN) ? timer25() : timer40();
}

Timer* timer25(){
  QueueArray<Vibration> vibs;
  vibs.push(*new Vibration(15000,500));
  vibs.push(*new Vibration(20000,500));
  vibs.push(*new Vibration(21000,500));
  vibs.push(*new Vibration(22000,500));
  vibs.push(*new Vibration(23000,500));
  vibs.push(*new Vibration(24000,500));
  vibs.push(*new Vibration(25000,5000));
  
  return new Timer(millis(),vibs);
}

Timer* timer40(){
  QueueArray<Vibration> vibs;
  vibs.push(*new Vibration(15000,500));
  vibs.push(*new Vibration(30000,500));
  vibs.push(*new Vibration(35000,500));
  vibs.push(*new Vibration(36000,500));
  vibs.push(*new Vibration(37000,500));
  vibs.push(*new Vibration(38000,500));
  vibs.push(*new Vibration(39000,500));
  vibs.push(*new Vibration(40000,5000));
        
  return new Timer(millis(),vibs);
}
