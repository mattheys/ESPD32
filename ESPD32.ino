#include <SPI.h>
#include <epd1in54.h>
#include <WiFi.h>
#include "icons.h"
#define uS_TO_S_FACTOR 1000000 
#include "time.h"
#include "ssid.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>


unsigned char image[1024];
Paint paint(image, 0, 0);
Epd epd;

RTC_DATA_ATTR int bootCount = 0;

const size_t bufferSize = JSON_ARRAY_SIZE(3) + 4*JSON_OBJECT_SIZE(6) + 450;
StaticJsonBuffer<bufferSize> jsonBuffer;
JsonVariant root;

void setup() {

  ++bootCount;
  
  Serial.begin(115200);

  //unsigned long timeout = millis() + 10000;
  WiFi.begin(ssid, password);
  //while (WiFi.status() != WL_CONNECTED && millis()< timeout ) { delay(100); }
  while (WiFi.status() != WL_CONNECTED) { delay(100); }
  if(WiFi.status() == WL_CONNECTED) {
    configTime(0, 3600, "pool.ntp.org");
  }

  if(bootCount == 1) {
    if (epd.Init(lut_full_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
    }
    epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    epd.DisplayFrame();
    epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
    epd.DisplayFrame();
  }

  if (epd.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  getJson();

  paint.SetRotate(ROTATE_0);
  printTime();
  printTrain();
  weather0(0,52);
  weather3(64,52);
  weather6(128,52);
  bin(0,128);
  sunup(64,128);
  sundown(128,128);
  
  epd.DisplayFrame();

}

void loop() {
  goToSleep(300);
}

void goToSleep(int seconds) {
  Serial.println("Going to sleep");
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
    
  //char d[19] = "Town:15:56 Early";
  //char t[19] = "Stra:15:56 Early";
  const char* ToTown = root["ToTown"];
  const char* ToStrat = root["ToStrat"];
  
  paint.DrawStringAt(2,0,ToTown,&Font16,0);
  paint.DrawStringAt(2,16,ToStrat,&Font16,0);

  epd.SetFrameMemory(paint, 0, 18);
}

void weather0(int x, int y) {
  JsonArray& Weather = root["Weather"];

  JsonObject& Weather0 = Weather[0];
  long Weather0_dt = Weather0["dt"]; // 1506718800
  const char* Weather0_time = Weather0["time"]; // "22:00"
  const char* Weather0_main = Weather0["main"]; // "Clear"
  const char* Weather0_description = Weather0["description"]; // "clear sky"
  const char* Weather0_temp = Weather0["temp"]; // "11.07"
  int Weather0_id = Weather0["id"]; // 800

  switch(Weather0_id) {
    case 200 ... 299: Serial.println("Thunder");epd.SetFrameMemory(thunder,x,y,64,64);  break;
    case 300 ... 399: Serial.println("Drizzle");epd.SetFrameMemory(rain,x,y,64,64);  break;
    case 500 ... 599: Serial.println("Rain");epd.SetFrameMemory(rain,x,y,64,64);  break;
    case 600 ... 699: Serial.println("Snow");epd.SetFrameMemory(snow,x,y,64,64);  break;
    case 800:         Serial.println("Clear");epd.SetFrameMemory(cleard,x,y,64,64);  break;
    case 801 ... 804: Serial.println("Clouds");epd.SetFrameMemory(cloudy,x,y,64,64);  break;
    default: epd.SetFrameMemory(tornado,x,y,64,64);  break;
  }
  
  //epd.SetFrameMemory(pic,x,y,64,64);
  paint.SetWidth(66);
  paint.SetHeight(16);
  paint.Clear(1);
  paint.DrawStringAt(0,0,Weather0_time,&Font16,0);
  epd.SetFrameMemory(paint,x+4,y+60);
}

void weather3(int x, int y) {
  JsonArray& Weather = root["Weather"];
  
  JsonObject& Weather1 = Weather[1];
  long Weather1_dt = Weather1["dt"]; // 1506729600
  const char* Weather1_time = Weather1["time"]; // "01:00"
  const char* Weather1_main = Weather1["main"]; // "Clear"
  const char* Weather1_description = Weather1["description"]; // "clear sky"
  const char* Weather1_temp = Weather1["temp"]; // "9.23"
  int Weather1_id = Weather1["id"]; // 800
  switch(Weather1_id) {
    case 200 ... 299: Serial.println("Thunder");epd.SetFrameMemory(thunder,x,y,64,64);  break;
    case 300 ... 399: Serial.println("Drizzle");epd.SetFrameMemory(rain,x,y,64,64);  break;
    case 500 ... 599: Serial.println("Rain");epd.SetFrameMemory(rain,x,y,64,64);  break;
    case 600 ... 699: Serial.println("Snow");epd.SetFrameMemory(snow,x,y,64,64);  break;
    case 800:         Serial.println("Clear");epd.SetFrameMemory(cleard,x,y,64,64);  break;
    case 801 ... 804: Serial.println("Clouds");epd.SetFrameMemory(cloudy,x,y,64,64);  break;
    default: epd.SetFrameMemory(tornado,x,y,64,64);  break;
  }
  //epd.SetFrameMemory(pic,x,y,64,64);
  paint.SetWidth(66);
  paint.SetHeight(16);
  paint.Clear(1);
  paint.DrawStringAt(0,0,Weather1_time,&Font16,0);
  epd.SetFrameMemory(paint,x+4,y+60);
}

void weather6(int x, int y) {
  JsonArray& Weather = root["Weather"];

  JsonObject& Weather2 = Weather[2];
  long Weather2_dt = Weather2["dt"]; // 1506740400
  const char* Weather2_time = Weather2["time"]; // "04:00"
  const char* Weather2_main = Weather2["main"]; // "Rain"
  const char* Weather2_description = Weather2["description"]; // "light rain"
  const char* Weather2_temp = Weather2["temp"]; // "9.11"
  int Weather2_id = Weather2["id"]; // 500

  switch(Weather2_id) {
    case 200 ... 299: Serial.println("Thunder");epd.SetFrameMemory(thunder,x,y,64,64);  break;
    case 300 ... 399: Serial.println("Drizzle");epd.SetFrameMemory(rain,x,y,64,64);  break;
    case 500 ... 599: Serial.println("Rain");epd.SetFrameMemory(rain,x,y,64,64);  break;
    case 600 ... 699: Serial.println("Snow");epd.SetFrameMemory(snow,x,y,64,64);  break;
    case 800:         Serial.println("Clear");epd.SetFrameMemory(cleard,x,y,64,64);  break;
    case 801 ... 804: Serial.println("Clouds");epd.SetFrameMemory(cloudy,x,y,64,64);  break;
    default: epd.SetFrameMemory(tornado,x,y,64,64);  break;
  }

  //epd.SetFrameMemory(pic,x,y,64,64);
  paint.SetWidth(66);
  paint.SetHeight(16);
  paint.Clear(1);
  paint.DrawStringAt(0,0,Weather2_time,&Font16,0);
  epd.SetFrameMemory(paint,x+4,y+60);
}

void bin(int x, int y) {
  const char* Bins = root["Bins"];  
  const char* letterN = "N";
  
  if(String(Bins) == String(letterN)) {
    Serial.println("Normal Bin");
    epd.SetFrameMemory(normalbin,x,y,64,64);
  } else {
    Serial.println("Recycle Bin");
    epd.SetFrameMemory(recyclebin,x,y,64,64);
  }
}

void sundown(int x, int y) {
  const char* SunSet = root["SunSet"]; // "18:48"

  epd.SetFrameMemory(sunset,x,y,64,64);
  paint.SetWidth(66);
  paint.SetHeight(16);
  paint.Clear(1);
  paint.DrawStringAt(0,0,SunSet,&Font16,0);
  epd.SetFrameMemory(paint,x+4,y+60);
}

void sunup(int x, int y) {
  const char* SunRise = root["SunRise"]; // "07:06"

  epd.SetFrameMemory(sunrise,x,y,64,64);
  paint.SetWidth(66);
  paint.SetHeight(16);
  paint.Clear(1);
  paint.DrawStringAt(0,0,SunRise,&Font16,0);
  epd.SetFrameMemory(paint,x+4,y+60);
}


void getJson() {
  HTTPClient http;
  http.begin("http://heys.suroot.com/red/epd");
  int httpCode = http.GET();
  if(httpCode > 0) {
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      root = jsonBuffer.parseObject(payload);
    }
  }
  http.end();  
}
