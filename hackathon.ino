#include <M5Core2.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "M5_ENV.h"
#include <string>

SHT3X sht30;
QMP6988 qmp6988;
float tmp      = 0.0;

#define WIFI_SSID "RTX2070"
#define WIFI_PASSWORD "fujita0625"

const char* line_notify_host     = "notify-api.line.me";
const char* line_notify_token = "KlJnTW74dpYQuWJMC6ll8OIbLyHcYCfp10DIFOkGXPl";
unsigned long displayTime = 0;
bool state = false;

void initWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  M5.Lcd.print("Connecting to wifi");
  // loading
  int _cursorX = 0;
  M5.Lcd.setTextFont(4);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 0);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    M5.Lcd.setCursor(0 + 5 * _cursorX, 30);
    M5.Lcd.print(".");
    delay(300);
    _cursorX++;
    if (_cursorX > 320) {
      _cursorX = 0;
    }
  }
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("connected with IP");
  M5.Lcd.println(WiFi.localIP());
}

void setup() {
  M5.begin();
  initWifi();
  Wire.begin();
  qmp6988.init();
   M5.Lcd.fillScreen(GREEN);
}
void _sendLINENotification(String message) {
  //LINE
  WiFiClientSecure wifiClient;
  wifiClient.setInsecure();

 if (!wifiClient.connect(line_notify_host,443)) {
    M5.Lcd.println("Connection failed");
    return;
  }

  String query = String("message=") + message;
  String request = String("")
              + "POST /api/notify HTTP/1.1\r\n"
              + "Host: " + line_notify_host + "\r\n"
              + "Authorization: Bearer " + line_notify_token + "\r\n"
              + "Content-Length: " + String(query.length()) +  "\r\n"
              + "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
              + query + "\r\n";
  wifiClient.print(request);
}

unsigned long previousMilles = 0;


void loop() {
  M5.update();
  if (sht30.get() == 0) {
        tmp = sht30.cTemp;
  } else {
        tmp = 0;
  }
  M5.lcd.fillRect(0, 20, 100, 60,BLACK);
  M5.lcd.setCursor(0, 20);
  M5.Lcd.printf("Temp: %2.1f",tmp);

  if (tmp<40 && state == true){
    M5.Lcd.fillScreen(GREEN);
    String msg = "1号機:プリントが完了しました。受け取りに来てください";
    _sendLINENotification(msg);
    state = false;9
  }
  if (tmp>40 && state == false){
    M5.Lcd.fillScreen(RED);
    state = true;
  }
}