#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiEsp.h>
#include <SoftwareSerial.h>

// WiFi 정보 설정 
#define rxPin 3 
#define txPin 2 
SoftwareSerial mySerial(txPin, rxPin);

const char ssid[] = "Wifi id";
const char pass[] = "password";
int status = WL_IDLE_STATUS;

// 웹 서버 주소 및 포트 번호 설정
const char* serverAddress = "3.39.175.221";
const int serverPort = 8080;
WiFiEspClient client;

// 조도센서(CDS) 관련 설정
const int CDS_PIN = A0;

// 탁도센서(AZDM01) 관련 설정
const int TURBIDITY_PIN = A1;

// DS18B20 온도센서 핀 번호
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// 이전 온도 값을 저장할 변수
float prevTempC = 0;

// 시간 간격 (1시간)
const unsigned long interval = 3600000; // milliseconds
unsigned long previousMillis = 0;

// 탁도센서 오류를 로깅하는 시간 간격 (5초)
const unsigned long turbidityLogInterval = 5000; // milliseconds
unsigned long turbidityLogPreviousMillis = 0;

// 하루에 허용되는 최대 오류 로그 횟수
const int maxErrorLogsPerDay = 2;

// 오류 로그 횟수와 마지막 로깅된 날짜 저장 변수
int errorLogCount = 0;
unsigned long lastLoggedDay = 0;

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
}

void loop() {
  // 현재 시간 계산
  unsigned long currentMillis = millis();

  // 탁도 값 읽기
  int turbidityValue = analogRead(TURBIDITY_PIN);
  Serial.print("탁도 값: ");
  Serial.println(turbidityValue);
 // 현재 날짜 구하기
  unsigned long currentDay = currentMillis / 86400000; // 1일은 86,400,000 밀리초

  // 오류 로그를 찍을지 결정
  bool shouldLogError = false;

  if (currentDay != lastLoggedDay) {
    // 새로운 날이 시작될 때, 오류 로그 횟수와 마지막 로깅된 날짜 초기화
    errorLogCount = 0;
    lastLoggedDay = currentDay;
  }

  if (errorLogCount < maxErrorLogsPerDay) {
    // 허용된 최대 횟수 내에서만 오류 로그를 찍음
    shouldLogError = true;
  }

  // 5초마다 탁도 오류 로그를 전송
  if (currentMillis - turbidityLogPreviousMillis >= turbidityLogInterval) {
    turbidityLogPreviousMillis = currentMillis;
     if (shouldLogError && (turbidityValue < 100 || turbidityValue > 900)) {
      logTurbidityError(turbidityValue);
      errorLogCount++; // 오류 로그 횟수 증가
    }
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
    String url = "/marimoData/sensor"; // Use the appropriate endpoint for marimo data
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

  // 잠시 딜레이
  delay(1000);
}

void logTurbidityError(int turbidityValue) {
  // JSON 데이터 형식 구성
  String logData = "{\"marimoId\": 1, \"stat\": \"turbidity_error\", \"value\": " + String(turbidityValue) + "}";

  // HTTP POST 요청 준비
  String url = "/log/2"; // Endpoint for turbidity error log
  String payload = logData;

  // 웹 서버로 HTTP POST 요청 전송
  if (client.connect(serverAddress, serverPort)) {
    client.print("POST " + url + " HTTP/1.1\r\n");
    client.print("Host: " + String(serverAddress) + ":" + String(serverPort) + "\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(payload.length());
    client.print("\r\n\r\n");
    client.print(payload);

    Serial.println("Turbidity Error Log Sent to Server");
  } else {
    Serial.println("Connection to server failed");
  }
}
