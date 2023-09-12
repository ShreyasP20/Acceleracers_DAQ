#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include <SPI.h>

//#include "Nextion.h"

// Time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;

// WiFi credentials
const char* ssid = "Shubham";         // change SSID
const char* password = "00000000";    // change password

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbxrepWO3unaBwAt2zThHCSH5jPnznNS2QnqkMs_Lbma1CriXaXG_09CsdT_g1T5n9qdXQ";    // change Gscript ID
String GOOGLE_SCRIPT_ID_2 = "AKfycbx-YNa_W6a2cwr62HHkfeQnMkIp-sxShkvQuh-AGH1Xk_dcjUxIiaVDMFw8YbwdHvEs";
int count = 0;

// BLYNK CREDENTIALS
#define BLYNK_TEMPLATE_ID "TMPLm5sYQ2wr"
#define BLYNK_DEVICE_NAME "BMS"
#define BLYNK_AUTH_TOKEN "8DPA8fbqZsZup02Ql7XP5oDmy8VVEOUa"

//Nextion objects
//NexNumber soc = NexNumber(0, 2, "n0");  // SOC Number



int byte4,byte5,byte6,byte7, byte8;
int byte0,byte1,byte2,byte3;
const int SPI_CS_PIN = 5;
long unsigned int rxId;
uint16_t GeneralBroadcastID = 0x7DF;
unsigned char GeneralBroadcastCMD[8] = {0x01, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int canid = 0x6B2;
int canid2 = 0x6B3;

MCP_CAN CAN(SPI_CS_PIN);  


void setup() {
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
  //Init Blynk
  

  
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




/*void sendNextion()
{
  soc.setValue(byte0);
}*/
  




void loop() {
  getcandata();
  
  
  //assignvalues2upload();
//  uploadtosheet();

} 


unsigned char buf[8];
unsigned char buf2[8];

void getcandata(){
  unsigned char len = 8;
    
    
    Serial.println("Entered Loop");
    if (!digitalRead(19)) {          // check if data coming
      Serial.println("Data Recieved");
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      Serial.println("-----------------------------");
      Serial.print("Get data from ID: ");
      rxId = CAN.getCanId();
      Serial.println(rxId, HEX);
  
      for (int i = 0; i < 8; i++) // print the data
      {
        
        Serial.print(buf[i], DEC);
        Serial.print("\t");
      }
      Serial.println();
  
      //re-send first request
      CAN.sendMsgBuf(GeneralBroadcastID, 0, 8, GeneralBroadcastCMD);
      
      if(rxId == canid2){
      assignvalues2upload1();
      }    // send data per 1000ms
      else if(rxId==canid){
        assignvalues2upload2();
        }
      
  }}

void assignvalues2upload1(){
    byte0 = buf[0]/2; //SOC
    byte1 = buf[1]; //High temp
    String packvoltageMSB=String(buf[2],HEX);
    String packvoltageLSB=String(buf[3],HEX);
    String packvoltage="0x"+packvoltageMSB+packvoltageLSB;
    byte2 = strtol(packvoltage.c_str(), NULL, 16); //Pack open Voltage
    String currentMSB=String(buf[4],HEX);
    String currentLSB=String(buf[5],HEX);
    String currentstr = "0x"+currentMSB+currentLSB;    
    int current = strtol(currentstr.c_str(), NULL, 16);
    byte3 = (32767-current)*0.1; // Unsigned current
    String lvMSB=String(buf[6],HEX);
    String lvLSB=String(buf[7],HEX);
    String lv = "0x"+lvMSB+lvLSB;
    byte4 = strtol(lv.c_str(), NULL, 16); //LV Battery voltage
    uploadtosheet1();
}

void assignvalues2upload2(){
    Serial.println("Uploading to Sheet2");
    String celllowMSB=String(buf[0],HEX);
    String celllowLSB=String(buf[1],HEX);
    String celllow="0x"+celllowMSB+celllowLSB;
    byte5 = strtol(celllow.c_str(), NULL, 16); //Low cell Voltage
    String cellhighMSB=String(buf[2],HEX);
    String cellhighLSB=String(buf[3],HEX);
    String cellhigh = "0x"+cellhighMSB+cellhighLSB;    
    byte6 = strtol(cellhigh.c_str(), NULL, 16); //high cell voltage
uploadtosheet2();
}



void uploadtosheet1(){
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
    Serial.print("Time:");
    Serial.println(asString);
    Serial.println(String(count));
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + asString +"&byte0=" + String(byte0)+ "&byte1=" + String(byte1)+ "&byte2=" + String(byte2)+ "&byte3=" + String(byte3)+ "&byte4=" + String(byte4);
//    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + asString +"&byte0=" + "10"+ "&byte1=" + "20"+ "&byte2=" + "30"+ "&byte3=" + "40"+ "&byte4=" + "50";
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
  //sendNextion();
  }







  void uploadtosheet2(){
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
    Serial.print("Time:");
    Serial.println(asString);
    Serial.println(String(count));
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID_2+"/exec?"+"date=" + asString +"&byte0=" + String(byte5)+ "&byte1=" + String(byte6);
//    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + asString +"&byte0=" + "10"+ "&byte1=" + "20"+ "&byte2=" + "30"+ "&byte3=" + "40"+ "&byte4=" + "50";
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
  //sendNextion();
  }
