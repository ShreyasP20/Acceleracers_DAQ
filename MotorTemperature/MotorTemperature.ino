#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>

#define pump 8

float resistor = 9800;
int ttemp;
char acceldata[12];

File myFile;

void setup()
{
  pinMode(pump, OUTPUT);
  Serial.begin(9600);
 Serial.print("Initializing SD card...");
  if (!SD.begin(10)) 
  {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  int no1=0;
  snprintf(acceldata,sizeof(acceldata), "accel%02d.csv", no1);
  while (SD.exists(acceldata)) {
    no1++;
    snprintf(acceldata,sizeof(acceldata), "accel%02d.csv", no1);
  } 
}
int ctr1=0,sum, sum1,ctr2 =0;
int finaltemp =35;
int temparr[100];
void loop()
{
  if(millis()%10==0){
    if(ctr1==100){
      ctr1=0;
      ttemp = sum/100;
      sum=0;
      
      sum1 +=ttemp;
      ctr2++;
      if(ctr2==5){
        finaltemp = sum1/5;
        sum1=0;
        ctr2=0;
        Serial.println(finaltemp);
        }
      }
    temparr[ctr1] = kty();
    ctr1++;
    sum +=temparr[ctr1];
    }
//    digitalWrite(pump, HIGH);
   if(finaltemp >= 40)
   {
    digitalWrite(pump, HIGH);
   }
   else if(finaltemp <= 30)
   {
    digitalWrite(pump, LOW);
   }
   save2sd();

}

void save2sd(){
  myFile = SD.open(acceldata, FILE_WRITE);
  myFile.println(finaltemp);
  myFile.close();
  }

float kty() {
    float sensorValue = analogRead(A0); 
   float resistance = sensorValue / (1023- sensorValue) * resistor;
//   Serial.println(resistance);
//float resistance = 498;
   //160.48
return -183.48 -(7.82524046e-12 * pow(resistance,4)) + (6.34442316e-08 * pow(resistance,3))-(2.01190577e-04 * pow(resistance,2)) + (4.07682012e-01 * resistance);  

}
