/*
 * DHT22 센서 전용 테스트 코드
 * 메인 프로젝트와 별개로 DHT22만 단독 테스트
 * Pin 2 사용
 */

#include <Arduino.h>
#include <DHT.h>

#define DHT_PIN 2
#define DHT_TYPE DHT22
#define LED_PIN 13  // 내장 LED로 상태 표시

DHT dht(DHT_PIN, DHT_TYPE);

// 함수 선언
void testDHTReading();

void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("     DHT22 센서 단독 테스트");
  Serial.println("========================================");
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("DHT22 초기화 중...");
  dht.begin();
  Serial.println("DHT22 초기화 완료");
  
  Serial.println("5초 후 첫 번째 읽기 시작...");
  delay(5000);
  
  Serial.println("첫 번째 센서 읽기 시도:");
  testDHTReading();
  
  Serial.println("========================================");
  Serial.println("2초마다 센서 값을 읽습니다...");
}

void loop() {
  static unsigned long lastRead = 0;
  
  // 2초마다 센서 읽기
  if (millis() - lastRead >= 2000) {
    Serial.println("\n--- DHT22 읽기 ---");
    
    digitalWrite(LED_PIN, HIGH); // 읽기 시작 표시
    
    unsigned long startTime = millis();
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    unsigned long readTime = millis() - startTime;
    
    Serial.print("읽기 시간: ");
    Serial.print(readTime);
    Serial.println("ms");
    
    // 결과 확인
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("❌ DHT22 읽기 실패!");
      Serial.println("   - 센서 연결 확인");
      Serial.println("   - 핀 번호 확인 (Pin 2)");
      Serial.println("   - 전원 연결 확인 (5V, GND)");
      
      // LED 빠르게 깜빡임 (오류)
      for(int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
      }
    } else {
      Serial.println("✅ DHT22 읽기 성공!");
      Serial.print("   온도: ");
      Serial.print(temperature, 1);
      Serial.println("°C");
      Serial.print("   습도: ");
      Serial.print(humidity, 1);
      Serial.println("%");
      
      // 온도가 합리적인 범위인지 확인
      if (temperature >= -40 && temperature <= 80) {
        Serial.println("   📊 온도 값 정상 범위");
      } else {
        Serial.println("   ⚠️ 온도 값 비정상 범위");
      }
      
      // 습도가 합리적인 범위인지 확인  
      if (humidity >= 0 && humidity <= 100) {
        Serial.println("   📊 습도 값 정상 범위");
      } else {
        Serial.println("   ⚠️ 습도 값 비정상 범위");
      }
      
      // LED 천천히 깜빡임 (성공)
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
    }
    
    lastRead = millis();
  }
}

void testDHTReading() {
  Serial.println("1. 기본 읽기 테스트...");
  
  float temp1 = dht.readTemperature();
  float hum1 = dht.readHumidity();
  
  if (isnan(temp1) || isnan(hum1)) {
    Serial.println("   ❌ 첫 번째 읽기 실패");
  } else {
    Serial.print("   ✅ 온도: ");
    Serial.print(temp1, 1);
    Serial.print("°C, 습도: ");
    Serial.print(hum1, 1);
    Serial.println("%");
  }
  
  delay(3000);
  
  Serial.println("2. 두 번째 읽기 테스트...");
  
  float temp2 = dht.readTemperature();
  float hum2 = dht.readHumidity();
  
  if (isnan(temp2) || isnan(hum2)) {
    Serial.println("   ❌ 두 번째 읽기 실패");
  } else {
    Serial.print("   ✅ 온도: ");
    Serial.print(temp2, 1);
    Serial.print("°C, 습도: ");
    Serial.print(hum2, 1);
    Serial.println("%");
    
    // 두 읽기 간 차이 확인
    float tempDiff = abs(temp2 - temp1);
    float humDiff = abs(hum2 - hum1);
    
    Serial.print("   📈 온도 차이: ");
    Serial.print(tempDiff, 1);
    Serial.println("°C");
    Serial.print("   📈 습도 차이: ");
    Serial.print(humDiff, 1);
    Serial.println("%");
    
    if (tempDiff < 5.0 && humDiff < 10.0) {
      Serial.println("   ✅ 센서 값 안정적");
    } else {
      Serial.println("   ⚠️ 센서 값 변화가 큼 - 연결 확인 필요");
    }
  }
}