//Include required libraries
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include <mcp_can.h>
#include <SPI.h>
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "Shubham";         // change SSID
const char* password = "00000000";    // change password
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbxfEFZG3w7g37WQvv2x41407Iw7CcDtGwa9Qm6MVtXsCwnUF9cuivmc1vYDKNqQRoPVDQ";    // change Gscript ID
int count = 0;

int LSBspeed,MSBspeed,LSBmotorcurrent,MSBmotorcurrent,LSBbatteryvoltage,MSBbatteryvoltage,LSBerrorcode,MSBerrorcode;
float spd,motorcurrent,batteryvoltage,errorcode1,errorcode2;

int throttlesignal, controllertemp;

int canid1 = 0xCF11E05,canid2 = 0xCF11F05;

const int SPI_CS_PIN = 5;


uint16_t GeneralBroadcastID = 0x7DF;
unsigned char GeneralBroadcastCMD[8] = {0x01, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int controllerfan =2;

MCP_CAN CAN(SPI_CS_PIN);  

void setup() {
  pinMode(32,OUTPUT);
  delay(1000);
  Serial.begin(115200);
  delay(1000);
  
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
        CAN.sendMsgBuf(GeneralBroadcastID, 0, 8, GeneralBroadcastCMD);
        Serial.println("Message sent");
        delay(500);
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}



void loop() {
  getcandata();
} 

void uploadtosheet(){
  if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50]; //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String asString(timeStringBuff);
    asString.replace(" ", "-");
//    Serial.print("Time:");
//    Serial.println(asString);
//    Serial.println(String(count));
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + asString +"&spd=" + String(spd)+ "&motorcurrent=" + String(motorcurrent)+ "&batteryvoltage=" + String(batteryvoltage)+ "&errorcode1=" + String(errorcode1)+ "&errorcode2=" + String(errorcode2)+ "&throttlesignal=" + String(throttlesignal)+ "&controllertemp=" + String(controllertemp);
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  }
  count++;  
  }

unsigned char buf[8];
void getcandata(){
  unsigned char len = 8;
    long unsigned int rxId;
    Serial.println("Entered function");
    if (!digitalRead(19)) {          // check if data coming
      Serial.print("Entered loop");
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      Serial.println("-----------------------------");
      rxId = CAN.getCanId();
      Serial.println(rxId);
        if(rxId == canid1){
        LSBspeed = buf[0];
        MSBspeed = buf[1];
        LSBmotorcurrent = buf[2];
        MSBmotorcurrent = buf[3];
        LSBbatteryvoltage = buf[4];
        MSBbatteryvoltage = buf[5];
        LSBerrorcode = buf[6];
        MSBerrorcode = buf[7];
        }
      if(rxId == canid2){
        throttlesignal = buf[0];
        controllertemp = buf[1] -50;
        }
        

      spd = (MSBspeed*256)+LSBspeed;
      motorcurrent = ((MSBmotorcurrent*256)+LSBmotorcurrent)/10;              //0-400A
      batteryvoltage = ((MSBbatteryvoltage*256)+LSBbatteryvoltage)/10;             //0-180V
      errorcode1 = LSBerrorcode;
      errorcode2 = MSBerrorcode;

      
      if(rxId==canid1 or rxId == canid2){
      Serial.print(rxId, HEX);
      Serial.print("Motor Speed in RPM: ");
      Serial.println(spd, DEC);
      Serial.print("Motor Current in Amps: ");
      Serial.println(motorcurrent, DEC);
      Serial.print("Battery Voltage in Volts: ");
      Serial.println(batteryvoltage, DEC);
      Serial.print("Error Codes: ");
      Serial.print(errorcode1, DEC);
      Serial.print("  ,  ");
      Serial.println(errorcode2, DEC);
      Serial.print("Throttle signal: ");
      Serial.println(throttlesignal, DEC);
      Serial.print("Controller Temperature: ");
      Serial.println(controllertemp, DEC);
      Serial.println();
      uploadtosheet(); //need to upload at 1Hz(Google API limitations)
      if(controllertemp>40)
      {
       digitalWrite(controllerfan,HIGH); 
      }
      else
      {
        digitalWrite(controllerfan,LOW);
      }
      }
      //re-send first request
      CAN.sendMsgBuf(GeneralBroadcastID, 0, 8, GeneralBroadcastCMD);
  }
   setoutput();
 //     digitalWrite(32,HIGH);
  }

  void setoutput(){
   if(controllertemp>=45) {
      digitalWrite(32, HIGH);
     }
    else if(controllertemp<=40){
      digitalWrite(32, LOW);
      }
   }

  
