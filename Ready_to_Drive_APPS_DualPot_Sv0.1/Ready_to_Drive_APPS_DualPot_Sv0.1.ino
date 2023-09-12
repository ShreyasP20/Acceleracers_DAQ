#include "errorchk.h"

#define pin_i 7  // input pin from PTS
#define pin_o 4  // Output pin to buzzer
#define pin A2
float bar;

const int analog1InPin = A0;  // Analog input pin that the potentiometer 1 is attached to
const int analog2InPin = A1;  // Analog input pin that the potentiometer 2 is attached to
const int analogOutPin = 9;   // Analog output pin that the LED is attached to

float sensor1Value = 0;  // value read from the pot1
float sensor2Value = 0;  // value read from the pot2
int Throttle1 = 0;          // value output1 to the PWM (analog out)
int Throttle2 = 0;          // value output2 to the PWM (analog out)
float pot1;
float pot2;

int t = 1, n = 0, sum = 0;  //to check if deviation <=10%
float avg;
long time_Check1;
bool flag = true;
int ctr = 0;  // counter for activating APPS
long timexwastrue;

void setup() {
  Serial.begin(9600);
  pinMode(pin_i, INPUT);
  pinMode(pin_o, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
}

void loop() {
  int x = digitalRead(pin_i);
  float a = analogRead(pin);
  bar = map(a, 210, 1023, 0, 200);
  //Serial.println(bar);


  sensor1Value = analogRead(analog1InPin);
  sensor2Value = analogRead(analog2InPin);
  /*Serial.println("Before Mapping:");
  Serial.print("Throttle1:");
  Serial.print(sensor1Value);
  Serial.print("\tThrottle2:");
  Serial.println(sensor2Value);*/
  float dev = calcError(1, sensor1Value,  sensor2Value*2);
  Serial.print("Dev:");
  Serial.println(dev);
       
  Throttle1 = map(sensor1Value, 0, 830, 0, 255);  //SENSOR1 100%->3.2V
  Throttle2 = map(sensor2Value, 0, 400, 0, 255);  //SENSOR2 1W00%->1.6V

  //float dev = calcError(Throttle1, Throttle2);
  //Serial.print("Dev:");
  //Serial.println(dev);
  

  if (digitalRead(2) == LOW or digitalRead(3) == LOW) {
    //ctr = 0;
    //analogWrite(analogOutPin, 0);
  }


  //25% pedal travel + hard braking rule check


  //  if(pot1>1.8 && bar>=15 ){
  //    while(pot1>0.8){
  //      analogWrite(analogOutPin, 0);
  //      sensor1Value = analogRead(analog1InPin);
  //      pot1 = sensor1Value * 5.0/1023;
  //      Serial.println("25%");
  //      }
  //    }



  if (dev > 10)  //deviation check
  {
    n++;
    if (t == 1) 
    {
      time_Check1 = millis();
      t = 0;
    }
    sum = sum + dev;
    long time_Check2 = millis();
    if (time_Check2 - time_Check1 >= 100)
    {
      avg = sum / n;
      if (avg > 10)
      {
        ctr = 0;
        Serial.println("dev>10");
        analogWrite(analogOutPin, 0);
      }
    }
  } 
  else
  {
    sum=0;
    avg = 0;
    t = 1;
    n = 0;
  }

  //Serial.print("dev: ");
  //Serial.println(dev);


  if (ctr == 0) {

    if (x == HIGH ) {

      ctr = 1;
      //Serial.println("Heelo3");
    }
  }
  if (ctr == 1) {
    digitalWrite(pin_o, HIGH);
    delay(1500);
    digitalWrite(pin_o, LOW);
    ctr = 2;
    Serial.println("Ready Drive mode enabled!");
    delay(25);
  }
  if (ctr == 2)
  {
    //    Serial.println("Heelo0");
    if (sensor1Value != sensor2Value) 
    {
      //Serial.println("Heelo1");
      // read the analog in value:
      // map it to the range of the analog out:

      //        Serial.println("Heelo");
      sensor1Value = analogRead(analog1InPin);
      if (sensor1Value > 1000) 
      {
        sensor1Value = 1000;
      }
      // change the analog out value:

      //O/p the Lower Value 
      if(Throttle1>Throttle2)
        {
          analogWrite(analogOutPin, Throttle2);
          Serial.println(Throttle2);
        }
        else
        {
          Serial.println(Throttle1);
          analogWrite(analogOutPin, Throttle1);
        }
        Serial.println("After Mapping:");
        Serial.print("Throttle1:");
        Serial.print(Throttle1);
        Serial.print("\tThrottle2:");
        Serial.println(Throttle2);
        
                
      if (digitalRead(2) == LOW or digitalRead(3) == LOW) {
        ctr = 0;
        analogWrite(analogOutPin, 0);
        Serial.println("2,3 low");
      }
    } else {
        Serial.println("Error: Signal lines of both the pot is shorted");
    }
  }
}

 float calcError(float x, float y) {
  float error = (abs(x - y) / x) * 100;
  return error;
}