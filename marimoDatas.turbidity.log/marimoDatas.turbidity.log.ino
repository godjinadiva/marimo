#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiEsp.h>
#include <SoftwareSerial.h>
<<<<<<< Updated upstream
#include <LCD5110_Graph.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

// WiFi 정보 설정
const char ssid[] = "LGWiFi_DD04";
const char pass[] = "YourWiFiPassword"; // WiFi 비밀번호

=======
#include <Adafruit_NeoPixel.h>

// WiFi 정보 설정
const char ssid[] = "LGWiFi_DD04";
const char pass[] = "-------";
>>>>>>> Stashed changes
int status = WL_IDLE_STATUS;

// 웹 서버 주소 및 포트 번호 설정
const char* serverAddress = "3.39.175.221"; // 웹 서버 주소
const int serverPort = 8080; // 웹 서버 포트

// 핀번호 설정
<<<<<<< Updated upstream
const int rxPin = 3;
const int txPin = 2;
const int ONE_WIRE_BUS = 4;
const int NEOPIXEL_PIN = 6;
const int BUTTON_PIN = 7; // 버튼을 7번 핀에 연결
const int NEOPIXEL_NUM_LEDS = 300;
const int CDS_PIN = A0;
const int TURBIDITY_PIN = A1;

// 여러가지 관련 설정
SoftwareSerial mySerial(rxPin, txPin); // 통신 관련 설정
Adafruit_NeoPixel neopixels(NEOPIXEL_NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800); // 네오픽셀 관련 설정
LiquidCrystal_I2C lcd(0x3F, 16, 2); // 테스트 LCD
=======
#define rxPin 3
#define txPin 2
#define ONE_WIRE_BUS 4
#define NEOPIXEL_PIN1 6 // 상태 알려주는 네오픽셀
#define NEOPIXEL_PIN2 7 // 어항 밑 네오픽셀
#define BUTTON_pin 8 // 버튼을 8번 핀에 연결
#define NEOPIXEL_NUM_LEDS1 150
#define NEOPIXEL_NUM_LEDS2 150
#define NEOPIXEL_TEMPERATURE_LED 4  
#define NEOPIXEL_LIGHT_LED 8      
#define NEOPIXEL_TURBIDITY_LED 12
const int CDS_PIN = A0;
const int TURBIDITY_PIN = A1;

// Define NeoPixel colors
#define RED_COLOR 0xFF0000  // Red color
#define YELLOW_COLOR 0xFFFF00  // Yellow color
#define BLUE_COLOR 0x0000FF  // Blue color
#define GREEN_COLOR 0x00FF00  // Green color

// 여러가지 관련 설정
SoftwareSerial mySerial(txPin,rxPin); // 통신 관련 설정
Adafruit_NeoPixel neopixels1 = Adafruit_NeoPixel(NEOPIXEL_NUM_LEDS1, NEOPIXEL_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel neopixels2 = Adafruit_NeoPixel(NEOPIXEL_NUM_LEDS2, NEOPIXEL_PIN2, NEO_GRB + NEO_KHZ800);
>>>>>>> Stashed changes
OneWire oneWire(ONE_WIRE_BUS); // DS18B20 온도센서 관련 설정
DallasTemperature sensors(&oneWire);

// 이전 온도 값을 저장할 변수
float prevTemperature = 0;
unsigned long lastLoggedDayTurb = 0;
int turbidityErrorLogCount = 0;
int temperatureChangeLogCount = 0;
bool neopixelState = false; // Neopixel 상태 저장 변수
bool lastNeopixelState = false; // 이전 Neopixel 상태 저장 변수
bool LED_state = true; // LED 상태 저장 변수 (초기에 켜진 상태)

// 시간 간격 (1시간)
const unsigned long interval = 3600000; // milliseconds
unsigned long previousMillis = 0;

// 탁도센서 오류를 로깅하는 시간 간격 (5초)
const unsigned long turbidityLogInterval = 5000; // milliseconds
unsigned long turbidityLogPreviousMillis = 0;

// 온도 변화를 관찰하는 시간 간격 (1분)
const unsigned long temperatureChangeInterval = 60000; // milliseconds
unsigned long temperatureChangePreviousMillis = 0;

// 하루에 허용되는 탁도 최대 오류 로그 횟수
const int maxErrorLogsPerDay = 2;
<<<<<<< Updated upstream
=======

// 하루에 허용되는 최대 온도 변화 로그 횟수
const int maxTemperatureChangeLogsPerDay = 2;
>>>>>>> Stashed changes


// 조도, 탁도, 온도 임계값 설정
const int thresholdLight = 200; // 조도 임계값
const int thresholdTurbidity = 350; // 탁도 임계값
const float thresholdTemperature = 26.0; // 온도 임계값

<<<<<<< Updated upstream
// 표정 이미지 데이터
const byte sadFace[5] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10000001
};
const byte smileFace[5] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B01111110
};
=======

// 네오픽셀 LED 색상 설정
uint32_t sadColor = neopixels.Color(255, 0, 0); // 빨간색 (RGB)
uint32_t happyColor = neopixels.Color(0, 255, 0); // 녹색 (RGB)
>>>>>>> Stashed changes

// 네오픽셀 LED 색상 설정
uint32_t sadColor = neopixels.Color(255, 0, 0); // 빨간색 (RGB)
uint32_t happyColor = neopixels.Color(0, 255, 0); // 녹색 (RGB)

void setup() {
  // 시리얼 통신 초기화
  Serial.begin(9600);
  mySerial.begin(9600);
  // WiFi 모듈 초기화
  WiFi.init(&mySerial);
  // WiFi에 연결 시도
  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to WiFi network...");
    status = WiFi.begin(ssid, pass);
    delay(5000); // 5초마다 연결 시도
  }
   // WiFi 연결 성공 시 정보 출력
   Serial.println("Connected to WiFi");

   // DS18B20 온도센서 초기화
   sensors.begin();

   // Neopixel 관련 코드
   pinMode(BUTTON_pin, INPUT); // 입력으로 설정
   // Neopixel 초기화
   neopixels1.begin();
   neopixels2.begin();
   if (LED_state) {
    // 각각의 네오픽셀을 초기화하고 초록색으로 설정
    for (int i = 0; i < NEOPIXEL_NUM_LEDS1; i++) {
      neopixels1.setPixelColor(i, 0, 255, 0);
    }
    for (int i = 0; i < NEOPIXEL_NUM_LEDS2; i++) {
      neopixels2.setPixelColor(i, 0, 255, 0);
    }

<<<<<<< Updated upstream
  // Neopixel 관련 코드
  pinMode(BUTTON_PIN, INPUT); // 입력으로 설정
  neopixels.begin();

  if (LED_state) {
    // 초록색으로 LED 초기화
    for (int i = 0; i < NEOPIXEL_NUM_LEDS; i++) {
      neopixels.setPixelColor(i, 0, 255, 0);
    }
    neopixels.show(); // 초기에 NeoPixel 표시 활성화
  }

  // 테스트 LCD 초기화
  lcd.init(); // 초기화
  lcd.backlight(); // 배경 불 켜기
  lcd.clear();
=======
    neopixels1.show();
    neopixels2.show();
  }
>>>>>>> Stashed changes
}

void loop() {
  // 현재 시간 계산
  unsigned long currentMillis = millis();
  // 탁도 값 읽기
  int turbidityValue = analogRead(TURBIDITY_PIN);
  Serial.print("탁도 값: ");
  Serial.println(turbidityValue);
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
  // 현재 날짜 구하기
  unsigned long currentDay = currentMillis / 86400000; // 1일은 86,400,000 밀리초
  // 온도 값
  float currentTemperature = sensors.getTempCByIndex(0);

<<<<<<< Updated upstream
  // 오류 로그를 찍을지 결정
  bool shouldLogTurbidityError = false;
=======
  bool isSad = (analogRead(CDS_PIN) < thresholdLight) || (turbidityValue > thresholdTurbidity) || (currentTemperature >= thresholdTemperature);
>>>>>>> Stashed changes

  // 센서 값 읽기 및 상태 판단 로직
  // 어항 밑 네오픽셀 설정 (빨간색 또는 초록색)
  if (isSad) {
    // 어항 밑 네오픽셀 빨간색으로 설정
    setColor(neopixels2, 0, RED_COLOR);
  } else {
    // 어항 밑 네오픽셀 초록색으로 설정
    setColor(neopixels2, 0, GREEN_COLOR);
  }

  // 상태 나타내는 네오픽셀 설정
  // 온도 (빨강), 조도 (노랑), 탁도 (파랑)
  setColor(neopixels1, NEOPIXEL_TEMPERATURE_LED, isTemperatureSad ? RED_COLOR : GREEN_COLOR);
  setColor(neopixels1, NEOPIXEL_LIGHT_LED, isLightSad ? YELLOW_COLOR : GREEN_COLOR);
  setColor(neopixels1, NEOPIXEL_TURBIDITY_LED, isTurbiditySad ? BLUE_COLOR : GREEN_COLOR);

  // 네오픽셀 표시 업데이트
  neopixels1.show();
  neopixels2.show();
  }
  // setColor 함수 정의
void setColor(Adafruit_NeoPixel& strip, int pixelIndex, uint32_t color) {
  if (pixelIndex >= 0 && pixelIndex < strip.numPixels()) {
    strip.setPixelColor(pixelIndex, color);
  }
}
  delay(1000);

  // 온도 변화를 관찰하여 온도가 3도 이상 내려갈 때 로그를 보냄
  if (currentMillis - temperatureChangePreviousMillis >= temperatureChangeInterval) {
    temperatureChangePreviousMillis = currentMillis;
    sensors.requestTemperatures();
<<<<<<< Updated upstream
    float currentTemperature = sensors.getTempCByIndex(0);
=======
>>>>>>> Stashed changes

    if (currentTemperature - prevTemperature >= 3) {
      if (temperatureChangeLogCount < maxTemperatureChangeLogsPerDay) {
        logTemperatureChange(prevTemperature, currentTemperature);
        temperatureChangeLogCount++;
      }
      prevTemperature = currentTemperature;
    }
  }

  // 스위치를 이용하여 Neopixel 조명 상태 확인
<<<<<<< Updated upstream
  neopixelState = digitalRead(NEOPIXEL_PIN) == HIGH;
=======
  neopixelState = digitalRead(NEOPIXEL_PIN2) == HIGH;
>>>>>>> Stashed changes

  // Neopixel 상태가 변경되었을 때 로그 전송
  if (neopixelState != lastNeopixelState) {
    if (neopixelState) {
      // 조명이 켜진 경우 로그 전송
      logNeopixelOn();
    } else {
      // 조명이 꺼진 경우 로그 전송
      logNeopixelOff();
    }
    lastNeopixelState = neopixelState; // 상태 업데이트
  }

  // 1시간마다 전체 마리모 데이터를 전송
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // DS18B20 온도 값을 읽어옴
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    Serial.print("온도 값(C): ");
    Serial.println(tempC);
    Serial.print("Temp: ");
    Serial.print(tempC);
    Serial.println("'C");
    // JSON 데이터 형식 구성
    String jsonData = "{\"marimoId\": 1, \"stat1\": " + String(analogRead(CDS_PIN)) + ", \"stat2\": " + String(turbidityValue) + ", \"stat3\": " + String(tempC) + "}";
    // HTTP POST 요청 준비
    String url = "/marimoData/sensor"; // 마리모 데이터를 전송할 엔드포인트
    String payload = jsonData;
    // 웹 서버로 HTTP POST 요청 전송
    if (client.connect(serverAddress, serverPort)) {
      client.print("POST " + url + " HTTP/1.1\r\n");
      client.print("Host: " + String(serverAddress) + ":" + String(serverPort) + "\r\n");
      client.print("Content-Type: application/json\r\n");
      client.print("Content-Length: ");
      client.print(payload.length());
      client.print("\r\n\r\n");
      client.print(payload);
      Serial.println("Data Sent to Server");
    } else {
      Serial.println("Connection to server failed");
    }
  }
<<<<<<< Updated upstream

  // 표정 선택
  bool isSad = (analogRead(CDS_PIN) < thresholdLight) || (turbidityValue > thresholdTurbidity) || (tempC >= thresholdTemperature);

  // 화면 지우기
  myLCD.clear();

  // 표정 표시
  if (isSad) {
    displayImage(sadFace);
  } else {
    // 기본 표정 표시 (웃는 얼굴 등)
    displayImage(smileFace);
  }

  // 잠시 딜레이
  delay(1000);
=======
>>>>>>> Stashed changes
}

void logTemperatureChange(float prevTemp, float currentTemp) {
  // 온도 변화 로그를 서버로 전송하는 코드를 작성
  // JSON 데이터 형식으로 로그 데이터 생성
  String jsonData = "{\"eventType\": \"temperatureChange\", \"prevTemp\": " + String(prevTemp) + ", \"currentTemp\": " + String(currentTemp) + "}";
  sendLogToServer(jsonData);
}

void logNeopixelOn() {
  // Neopixel 켜짐 로그를 서버로 전송하는 코드를 작성
  // JSON 데이터 형식으로 로그 데이터 생성
  String jsonData = "{\"eventType\": \"neopixelOn\"}";
  sendLogToServer(jsonData);
}

void logNeopixelOff() {
  // Neopixel 꺼짐 로그를 서버로 전송하는 코드를 작성
  // JSON 데이터 형식으로 로그 데이터 생성
  String jsonData = "{\"eventType\": \"neopixelOff\"}";
  sendLogToServer(jsonData);
}

// 로그 데이터를 서버로 전송하는 함수
void sendLogToServer(String logData) {
  if (client.connect(serverAddress, serverPort)) {
    // HTTP POST 요청 준비
    String url = "/log"; // 로그 엔드포인트 URL
    String payload = logData;
    
    client.print("POST " + url + " HTTP/1.1\r\n");
    client.print("Host: " + String(serverAddress) + ":" + String(serverPort) + "\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(payload.length());
    client.print("\r\n\r\n");
    client.print(payload);
    
    Serial.println("Log Data Sent to Server");
  } else {
    Serial.println("Failed to connect to server");
  }
}
