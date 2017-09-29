#include <SPI.h>
#include <epd1in54.h>
#include <WiFi.h>
#include "icons.h"
#define uS_TO_S_FACTOR 1000000 
#include "time.h"
#include "ssid.h"

unsigned char image[1024];
Paint paint(image, 0, 0);
Epd epd;

RTC_DATA_ATTR int bootCount = 0;


void setup() {

  ++bootCount;
  
  Serial.begin(115200);

  if(bootCount == 1) {
    if (epd.Init(lut_full_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
    }
    epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    epd.DisplayFrame();
    epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    epd.DisplayFrame();

    unsigned long timeout = millis() + 5000;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && millis()< timeout ) { delay(100); }

    if(WiFi.status() == WL_CONNECTED) {
      configTime(0, 3600, "pool.ntp.org");
    }
  }

  if (epd.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  //paint.DrawStringAt(4, 4, String("Boot Count " + String(bootCount)).c_str(), &Font16, 0);
  //epd.SetFrameMemory(paint, 0, 0);
  
  paint.SetRotate(ROTATE_0);
  printTime();
  printTrain();
  epd.DisplayFrame();

//  WiFiClient client;
//  if (!client.connect("matt-heys.co.uk", 80)) {
//        printError("connection failed");
//        return;
//  }
//  
//  client.print(String("GET /testpic.php HTTP/1.1\r\n") +
//                 "Host: matt-heys.co.uk\r\n" +
//                 "Connection: close\r\n\r\n");
//  timeout = millis();
//  while (client.available() == 0) {
//    if (millis() - timeout > 5000) {
//      printError(">>> Client Timeout !");
//      client.stop();
//      return;
//    }
//  }
//
//  while(client.available()) {
//    String line = client.readStringUntil('\r');
//    if(line == "\r\n" || line == "" || line == "\r" || line == "\n")
//    {
//      unsigned char arr[5000];
//      client.readBytes(arr,5000);
//      epd.SetFrameMemory(arr);
//      epd.DisplayFrame();
//      return;
//    }
//  }  

}

void loop() {
  goToSleep(15);
}

void goToSleep(int seconds) {
  epd.Sleep();
  esp_sleep_enable_timer_wakeup(seconds * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void printError(String error) {
  Serial.println(error);
}

void printTime() {
  paint.SetWidth(200);
  paint.SetHeight(16);
  paint.Clear(1);
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
  }
  char d[20];
  
  strftime(d, sizeof(d), "%b %d %H:%M", &timeinfo);
  
  
  paint.DrawStringAt(34,0,d,&Font16,0);
  epd.SetFrameMemory(paint, 0, 2);
}

void printTrain() {

  paint.SetWidth(200);
  paint.SetHeight(32);
  paint.Clear(1);
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
  }
  char d[19] = "Town:15:51 On Time";
  char t[19] = "Stra:15:56 Early";
  
  paint.DrawStringAt(2,0,d,&Font16,0);
  paint.DrawStringAt(2,16,t,&Font16,0);

  epd.SetFrameMemory(paint, 0, 18);
}