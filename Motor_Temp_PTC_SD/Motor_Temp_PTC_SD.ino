#include <SD.h>
#include <SPI.h>

float resistor = 10000;
int ctr1=0,sum;
int temparr[100];
int ttemp,prevtemp;
int no1=0;
int ctr = 0;
int test=1;
char acceldata[20];
File myFile;
int RackTravel = A2;
int DamperTravel_L = A4;
int DamperTravel_R = A6;
void setup() {
  Serial.begin(9600);
  pinMode(53,OUTPUT); //SDcard
  digitalWrite(53,HIGH);
  pinMode(47,OUTPUT);
  prevtemp = kty();
  while(!Serial){
    ;
  }
  Serial.print("\n Intializing SD Card...");
   if (!SD.begin(53)) 
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



 

void loop() {  
  
  if(millis()%10==0){
    if(ctr1==100){
      ctr1=0;
      ttemp = sum/100;
      sum=0;
      if(ttemp>prevtemp+10 || ttemp <prevtemp-10)
      {
        ttemp = prevtemp;
        }
        else{
          prevtemp =ttemp;
          }
      
      
      Serial.println(ttemp);
      myFile=SD.open(acceldata,FILE_WRITE);
      myFile.print("Motor Temp:");
      myFile.println(ttemp);
      myFile.print("Rack Travel");
      myFile.println(analogRead(RackTravel));
      myFile.print("Damper Travel(L)");
      myFile.println(analogRead(DamperTravel_L));
      myFile.print("Damper Travel(R)");
      myFile.println(analogRead(DamperTravel_R));

      Serial.print("Motor Temp:");
      Serial.println(ttemp);
      Serial.print("Rack Travel:");
      Serial.println(analogRead(RackTravel));
      Serial.print("Rack Travel[In Degrees:]");
      Serial.println(map(analogRead(RackTravel),50,842,-90,+90));
      Serial.print("Damper Travel(L):");
      Serial.println(analogRead(DamperTravel_L));
      Serial.print("Damper Travel(R):");
      Serial.println(analogRead(DamperTravel_R));


      myFile.close();
      if(ttemp>50)
      {
        digitalWrite(47, HIGH);
      }
      else 
      {
        digitalWrite(47, LOW);
      }
      }
    temparr[ctr1] = kty();
    ctr1++;
    sum +=temparr[ctr1];
    }
}

float kty() {
   float sensorValue = analogRead(A0);  
//   Serial.print(sensorValue);
   float resistance = sensorValue / (1023- sensorValue) * resistor;

   return -157.48 -(7.82524046e-12 * pow(resistance,4)) + (6.34442316e-08 * pow(resistance,3))-(2.01190577e-04 * pow(resistance,2)) + (4.07682012e-01 * resistance);  
//   return -55 -(7.82524046e-12 * pow(resistance,4)) + (6.34442316e-08 * pow(resistance,3))-(2.01190577e-04 * pow(resistance,2)) + (4.07682012e-01 * resistance);  
}
