/*!
 * @file        Arduino_NTC_Interface.ino
 * @brief       Interfacing NTC Thermistor With Arduino
 * @author      Jobit Joseph
 * @copyright   Copyright (c) 2022 Semicon Media Pvt Ltd (https://circuitdigest.com)
 */
#include <mcp_can.h>
#include <SPI.h>
 
#define T0 A0         // Pin,to which the voltage divider is connected
#define T1 A1
#define T2 A2
#define T3 A3
#define T4 A4
#define T5 A5

#define nominal_resistance 10000       //Nominal resistance at 25⁰C
#define nominal_temeprature 25   // temperature for nominal resistance (almost always 25⁰ C)
#define samplingrate 5    // Number of samples
#define beta 3380  // The beta coefficient or the B value of the thermistor (usually 3000-4000) check the datasheet for the accurate value.
#define Rref 10000   //Value of  resistor used for the voltage divider
int samples = 0;   //array to store the samples

const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);
void setup() {
  Serial.begin(9600);   //initialize serial communication at a baud rate of 9600
  START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}

int maxVal,minVal,avg,sum;
uint8_t temp[8];
uint8_t check;

void loop() {
  gettemparr();
  minmax();
  avgtemp();
  calcchecksum();
  uint8_t msg1[8] ={0x01,minVal,maxVal,avg,0x06,0x51,0x57,check};
  uint8_t msg2[8];
  uint8_t msg3[8] ={0xF3, 0x00, 0x81, 0xF3, 0x01, 0x40, 0x1E, 0x90};

  CAN.sendMsgBuf(0x1839f381,1,8,msg1);
  for(int i=0;i<8;i++){
    Serial.print(msg1[i], DEC);
    Serial.print(" ");
    }
    Serial.println("");

  for(int i=0;i<6;i++){
    msg2[0] = 0x00;
    msg2[1] = 0x51+i;
    msg2[2] = temp[i];
    msg2[3] = i;
    msg2[4] = minVal;
    msg2[5] = maxVal;
    msg2[6] = 0x57;
    msg2[7] = 0x51;    
    CAN.sendMsgBuf(0x1838f381,1,8,msg2);
//  for(int i=0;i<8;i++){Serial.print(msg2[i], DEC);Serial.print(" ");}Serial.println("");
    }
    CAN.sendMsgBuf(0x18EEFF81, 1, 8, msg3);
  
}

void calcchecksum(){
  check = 0x01+minVal+maxVal+avg+0x06+0x51+0x57+0x39+0x08;
  }

void avgtemp(){

  for (int i = 0; i <7; i++) {
    sum=sum+temp[i];
    }
    avg = sum/6;
    sum = 0;
  }

void minmax(){
      maxVal=temp[0];
      minVal=temp[0];
     for (int i = 0; i < 6; i++) {
      if (temp[i] > maxVal) {
         maxVal = temp[i];
      }
      if (temp[i] < minVal) {
         minVal = temp[i];
      }
   }
  
  }


void gettemparr(){
  temp[0]=gettemp(T0);
  temp[1]=gettemp(T1);
  temp[2]=gettemp(T2);
  temp[3]=gettemp(T3);
  temp[4]=gettemp(T4);
  temp[5]=gettemp(T5);
    for(int i=0;i<6;i++){
    Serial.print(temp[i], DEC);
    Serial.print(" ");
    }
    Serial.println("");
  }



uint8_t gettemp(unsigned int pin){
  uint8_t i;
  float average;
  samples = 0;
  // take voltage readings from the voltage divider
  for (i = 0; i < samplingrate; i++) {
    samples += analogRead(pin);
    delay(1);
  }
  average = 0;
  average = samples / samplingrate;
  // Calculate NTC resistance
  average = 1023 / average - 1;
  average = Rref / average;
  float temperature;
  temperature = average / nominal_resistance;     // (R/Ro)
  temperature = log(temperature);                  // ln(R/Ro)
  temperature /= beta;                   // 1/B * ln(R/Ro)
  temperature += 1.0 / (nominal_temeprature + 273.15); // + (1/To)
  temperature = 1.0 / temperature;                 // Invert
  temperature -= 273.15;                         // convert absolute temp to C
  return temperature;
  }
