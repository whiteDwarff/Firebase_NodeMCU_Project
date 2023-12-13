#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

// 와이파이 설정
#define WIFI_SSID "DIT_FREE_WiFi"
#define WIFI_PASSWORD ""

// -----------------------------------------------------------
// 파이어베이스 연결정보 
// 사용자의 FIREBASE에 맞게 셋팅
#define FIREBASE_HOST FIREBASE_HOST
#define FIREBASE_AUTH FIREBASE_AUTH
// -----------------------------------------------------------

//파이어베이스 데이터 객체 생성
FirebaseData fbdo;

DHT dht(D4, DHT11);
int currentTime = 00;

void setup() {
  
  Serial.begin (115200);
  // 와이파이 연결
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
  // 온습도 센서 연결
  dht.begin(); 

  // 와이파이 연결 완료까지 대기
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print (".");
    delay (300);
  }
  
  //파이어베이스 연결
  Firebase.reconnectWiFi(true);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  // 온습도 데이터 변수 추가 
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();

  // 온습도의 값이 유효한지 확인
  if(isnan(humi) || isnan(temp)) {
    Serial.print("ERROR");
    return;
  }

  // Firebase에 데이터 추가
  Firebase.pushInt(fbdo, "/nodeMCU/time", currentTime);
  Firebase.pushFloat(fbdo, "/nodeMCU/Humi", humi);
  Firebase.pushFloat(fbdo, "/nodeMCU/Temp", temp);

  // 현재 시간 증가
  currentTime += 1;

  // 24시간 기준 00:00시가 되면 모든 데이터 초기화
  if(currentTime == 24) {
    currentTime = 0;
    Firebase.deleteNode(fbdo, "/nodeMCU");
  }

  // 1분마다 데이터를 업로드
  delay(10000);
}
