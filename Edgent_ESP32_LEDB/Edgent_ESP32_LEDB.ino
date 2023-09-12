/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
 *************************************************************
  Blynk.Edgent implements:
  - Blynk.Inject - Dynamic WiFi credentials provisioning
  - Blynk.Air    - Over The Air firmware updates
  - Device state indication using a physical LED
  - Credentials reset using a physical Button
 *************************************************************/

/* Fill in information from your Blynk Template here */
/* Read more: https://bit.ly/BlynkInject */
#define BLYNK_TEMPLATE_ID "TMPL6lFyXa0VQ"
#define BLYNK_TEMPLATE_NAME "LEDBLink"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_ESP32_DEV_MODULE
//#define USE_ESP32C3_DEV_MODULE
//#define USE_ESP32S2_DEV_KIT
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7
//#define USE_TTGO_T_OI

#include "BlynkEdgent.h"
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
}
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();
  digitalWrite(15,pinValue);
}
void setup()
{
  pinMode(15, OUTPUT);
  Serial.begin(115200);
  delay(100);
//  Blynk.virtualWrite(VPIN_BUTTON_1, waterLevelPer);
//  Blynk.virtualWrite(VPIN_BUTTON_2, (String(distance) + " cm"));
  BlynkEdgent.begin();
}

void loop() {
  BlynkEdgent.run();
}
