#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// WiFi 정보 설정 (본인의 WiFi SSID와 비밀번호로 변경)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// 웹 서버 주소 및 포트 번호 설정
const char* serverAddress = "3.39.175.221";
const int serverPort = 8080;

// 조도센서(CDS) 관련 설정
const int CDS_PIN = A0;

// 수질 탁도센서(AZDM01) 관련 설정
const int AZDM01_PIN = A1;

// DS18B20 온도센서 핀 번호
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SoftwareSerial esp8266(3, 4); // ESP8266의 RX와 TX 핀 설정

Adafruit_ADS1115 ads; // 수질 탁도센서(AZDM01) 객체 생성

void setup() {
  Serial.begin(9600); // 시리얼 통신 속도 설정
  esp8266.begin(9600); // ESP8266 통신 속도 설정

  // WiFi 연결 시도
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");

  // 센서 초기화
  ads.begin();
  sensors.begin();
}

void loop() {
  // 조도 값을 읽어옴
  int cdsValue = analogRead(CDS_PIN);
  Serial.print("CDS 조도 값: "); Serial.println(cdsValue);

  // 수질 탁도 값을 읽어옴
  int tdsValue = ads.readADC_SingleEnded(AZDM01_PIN);
  Serial.print("AZDM01 탁도 값: "); Serial.println(tdsValue);

  // DS18B20 온도 값을 읽어옴
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  Serial.print("온도 값(C): "); Serial.println(tempC);

  // 웹 서버로 JSON 데이터 전송
  sendJSONDataToServer(cdsValue, tdsValue, tempC);

  // 잠시 딜레이
  delay(5000);
}

void sendJSONDataToServer(int cdsValue, int tdsValue, float tempC) {
  if (esp8266.available()) {
    esp8266.find("OK");
  }

  // JSON 데이터 형식 구성
  String jsonData = "{\"marimoId\": 0, \"stat1\": " + String(cdsValue) + ", \"stat2\": " + String(tdsValue) + ", \"stat3\": " + String(tempC) + "}";

  // HTTP POST 요청 준비
  String url = "/marimoData/sensor";
  String payload = jsonData;

  // WiFiClient 객체 생성
  WiFiClient client;

  // 웹 서버로 HTTP POST 요청 전송
  if (client.connect(serverAddress, serverPort)) {
    Serial.println("Connected to server");
    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + String(serverAddress) + ":" + String(serverPort));
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println();
    client.println(payload);
    Serial.println("Data Sent to Server");
  } else {
    Serial.println("Connection to server failed");
  }

  // 연결 종료
  client.stop();
}

}