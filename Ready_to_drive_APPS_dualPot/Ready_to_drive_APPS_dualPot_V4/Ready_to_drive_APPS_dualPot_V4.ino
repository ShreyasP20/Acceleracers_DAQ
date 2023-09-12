#include "errorchk.h"

#define pin_i 7               // input pin from PTS
#define pin_o 4               // Output pin to buzzer
#define pin A2
float bar;

const int analog1InPin = A0;  // Analog input pin that the potentiometer 1 is attached to
const int analog2InPin = A1;  // Analog input pin that the potentiometer 2 is attached to
const int analogOutPin = 9;  // Analog output pin that the LED is attached to

int sensor1Value = 0;        // value read from the pot1
int sensor2Value = 0;        // value read from the pot2
int Thr1 = 0;        // value output1 to the PWM (analog out)
int Thr2 = 0;        // value output2 to the PWM (analog out)
float pot1;
float pot2;

int t=1, n=0, sum=0;       //to check if deviation <=10%
float avg;
long time_Check1;
bool flag=true; 
int ctr = 0;                    // counter for activating APPS
long timexwastrue;

void setup()
{
  Serial.begin(9600);
  pinMode(pin_i, INPUT);
  pinMode(pin_o, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
}

void loop()
{
  int x = digitalRead(pin_i);
  float a = analogRead(pin);
  bar = map(a, 210, 1023, 0, 200);
  //Serial.println(bar);


  sensor1Value = analogRead(analog1InPin);
  sensor2Value = analogRead(analog2InPin);
  pot1 = sensor1Value * 5.0/1023;
  pot2 = 2*sensor2Value * 5.0/1023;
//    Serial.print(pot1);
//    Serial.print(",");
//    Serial.println(pot2);
  
  float dev = calcError(1, pot1, pot2);

  if(digitalRead(2)==LOW or digitalRead(3)== LOW){
    ctr=0;
    analogWrite(analogOutPin, 0);
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


  
  if(dev>10)    //deviation check
  {
    n++;
    if(t==1)
    { 
      time_Check1=millis();
      t=0;       
    }
     sum=sum+dev;
    long time_Check2=millis();
    if(time_Check2-time_Check1>=100)
    {
      avg=sum/n;
      if(avg>10)
      {
        ctr=0;
        analogWrite(analogOutPin, 0);
//        Serial.println("dev>10");
      }   
    }
  }
  else
  {
    avg=0;
    t=1;
    n=0;
  }
  
//  Serial.print("dev: ");
//  Serial.println(dev);


  if (ctr == 0) 
  {
        
    if (x == HIGH && bar >= 15) 
    {
      
      ctr = 1;
      //Serial.println("Heelo3");
    }
  }
  if (ctr == 1) 
  {
    digitalWrite(pin_o, HIGH);
    delay(1500);
    digitalWrite(pin_o, LOW);
    ctr = 2;
//    Serial.println("Ready Drive mode enabled!");
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
        Thr1 = map(sensor1Value, 0, 1100, 0, 255);
        // change the analog out value:
        analogWrite(analogOutPin, Thr1);
        if(digitalRead(2)==LOW or digitalRead(3)== LOW){
            ctr=0;
            analogWrite(analogOutPin, 0);
//            Serial.println("2,3 low");
            }
    }
    else
    {
//      Serial.println("Error: Signal lines of both the pot is shorted");
    }
  }
}

float calcError(float x, float y)
{
  float error = (abs(x - y) / x)*100;
  return error;
}
