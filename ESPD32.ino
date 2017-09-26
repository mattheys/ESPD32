#include <SPI.h>
#include <epd1in54.h>
#include <epdpaint.h>
#include "imagedata.h"
#include "ssid.h"
#include <WiFi.h>

 
 
unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8
Epd epd;
unsigned long time_start_ms;
unsigned long time_now_s;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (epd.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }
 
WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      paint.DrawStringAt(30, 4, "Connecting", &Font16, 1);
      epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
      epd.DisplayFrame();
  }
  paint.DrawStringAt(30, 4, "Connected", &Font16, 1);
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
 
  epd.SetFrameMemory(myBitmap,8,8,96,96);
  epd.DisplayFrame();
 
  WiFiClient client;
    const int httpPort = 80;
    if (!client.connect("matt-heys.co.uk", httpPort)) {
        Serial.println("connection failed");
        return;
    }
 
    // This will send the request to the server
    client.print(String("GET /testpic.php HTTP/1.1\r\n") +
                 "Host: matt-heys.co.uk\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
 
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        if(line == "\r\n" || line == "" || line == "\r" || line == "\n")
        {
          unsigned char arr[5000];
          client.readBytes(arr,5000);
          epd.SetFrameMemory(arr);
          epd.DisplayFrame();
          return;
        }
    }
 
}
 
void loop() {
 
 
 
 
 
  //Serial.println("Loop");
  //Serial.println("Loop");
  // put your main code here, to run repeatedly:
  //time_now_s = (millis() - time_start_ms) / 1000;
  //char time_string[] = {'0', '0', ':', '0', '0', '\0'};
  //time_string[0] = time_now_s / 60 / 10 + '0';
  //time_string[1] = time_now_s / 60 % 10 + '0';
  //time_string[3] = time_now_s % 60 / 10 + '0';
  //time_string[4] = time_now_s % 60 % 10 + '0';
 
  //paint.SetWidth(32);
  //paint.SetHeight(96);
  //paint.SetRotate(ROTATE_270);
 
  //paint.Clear(UNCOLORED);
  //paint.DrawStringAt(0, 4, time_string, &Font24, COLORED);
  //epd.SetFrameMemory(paint.GetImage(), 80, 72, paint.GetWidth(), paint.GetHeight());
  //epd.DisplayFrame();
}

