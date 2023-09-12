#include "errorchk.h"

#define pin_i 7               // input pin from PTS
#define pin_o 4               // Output pin to buzzer
#define pin A2
float bar;

const int analog1InPin = A0;  // Analog input pin that the potentiometer 1 is attached to
const int analog2InPin = A3;  // Analog input pin that the potentiometer 2 is attached to
const int analogOutPin = 9;  // Analog output pin that the LED is attached to

int sensor1Value = 0;        // value read from the pot1
int sensor2Value = 0;        // value read from the pot2
int Thr1 = 0;        // value output1 to the PWM (analog out)
int Thr2 = 0;        // value output2 to the PWM (analog out)
float pot1;
float pot2;

int ctr = 0;                    // counter for activating APPS


void setup()
{
  Serial.begin(9600);
  pinMode(pin_i, INPUT);
  pinMode(pin_o, OUTPUT);
}

void loop()
{
  int x = digitalRead(pin_i);
  float a = analogRead(pin);
  bar = map(a, 210, 1023, 0, 200);
  //Serial.println(bar);

  sensor1Value = analogRead(analog1InPin);
  sensor2Value = analogRead(analog2InPin);
  //Serial.print(sensor1Value);
  //Serial.print(" ");
  //Serial.println(sensor2Value);
  pot1 = sensor1Value * 5.0/1023;
  pot2 = 2*sensor2Value * 5.0/1023;
  //if (pot1<=1)
  //{
    //sensor1Value = pot1 * 1023/5.0;
  //}
  //Serial.print(pot1);`fd`3
  //Serial.print(" ");
  //Serial.println(pot2);
  float dev = calcError(1, pot1, pot2);
  
  Serial.print("dev: ");
  Serial.print(dev);
  Serial.print(" ");
  Serial.print(pot1);
  Serial.print(" ");
  Serial.print(pot2);
  Serial.println("");

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
    Serial.println("Ready Drive mode enabled!");
    delay(25);
  } 
  if (ctr == 2) 
  {
    //Serial.println("Heelo0");
    if (sensor1Value != sensor2Value)
    {
      //Serial.println("Heelo1");
      if (dev <= 2000)
      {
        // read the analog in value:
        // map it to the range of the analog out:
        Serial.println("Heelo");
        if (sensor1Value > 750)
        {
          sensor1Value = 750;
        }
        Thr1 = map(sensor1Value, 0, 750, 0, 255);
        // change the analog out value:
        analogWrite(analogOutPin, Thr1);
      }
      else
      {
        Serial.println("Error: Deviation of more than 10%");
      }
    }
    else
    {
      Serial.println("Error: Signal lines of both the pot is shorted");
    }
  }
}

float calcError(float x, float y)
{
  float error = (abs(x - y) / x)*100;
  return error;
}
