/*
 * 빗물 감지 센서 전용 테스트 코드
 * Pin A0 사용 (아날로그)
 * 물방울로 테스트 가능
 */

#include <Arduino.h>

#define RAIN_SENSOR_PIN A0
#define LED_PIN 13
#define BUZZER_PIN 10

// 임계값 설정 (테스트하면서 조정)
int RAIN_THRESHOLD = 700;  // 700 이하면 빗물 감지 (값이 낮아질 때)

void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("      빗물 감지 센서 테스트");
  Serial.println("========================================");
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("빗물 센서 핀: A0");
  Serial.print("현재 임계값: ");
  Serial.println(RAIN_THRESHOLD);
  Serial.println();
  Serial.println("테스트 방법:");
  Serial.println("1. 센서에 물방울 떨어뜨리기");
  Serial.println("2. 젖은 손가락으로 센서 터치");
  Serial.println("3. 분무기로 물 뿌리기");
  Serial.println();
  Serial.println("0.5초마다 센서 값을 읽습니다...");
  Serial.println("========================================");
  
  delay(3000);
}

void loop() {
  // 빗물 센서 값 읽기 (0-1023)
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  
  // 현재 빗물 감지 상태 확인
  bool currentRainDetected = (rainValue < RAIN_THRESHOLD);
  
  // 이전 상태와 비교하여 변화 감지
  static bool previousRainDetected = false;
  
  // 상태 변화 시 메시지 출력
  if (currentRainDetected && !previousRainDetected) {
    Serial.println("\n🌧 빗물 감지! 수집 모드 시작");
    tone(BUZZER_PIN, 1000, 300); // 긴 경고음
  } else if (!currentRainDetected && previousRainDetected) {
    Serial.println("\n☀ 빗물 감지 종료! 수집 모드 정지");
    tone(BUZZER_PIN, 500, 200); // 낮은 음으로 종료 알림
  }
  
  // 시리얼 모니터에 값 표시
  Serial.print("빗물 센서: ");
  Serial.print(rainValue);
  Serial.print(" | 임계값: ");
  Serial.print(RAIN_THRESHOLD);
  
  // 임계값과 비교 (값이 낮아지면 빗물 감지)
  if (currentRainDetected) {
    Serial.print(" | 상태: 🌧 빗물 감지 중!");
    
    // LED 켜기
    digitalWrite(LED_PIN, HIGH);
    
  } else {
    Serial.print(" | 상태: ☀ 건조");
    
    // LED 끄기
    digitalWrite(LED_PIN, LOW);
  }
  
  // 센서 상태 분석
  if (rainValue < 50) {
    Serial.println(" [물에 완전 잠김]");
  } else if (rainValue < 200) {
    Serial.println(" [매우 습함]");
  } else if (rainValue < 400) {
    Serial.println(" [습함]");
  } else if (rainValue < 600) {
    Serial.println(" [약간 습함]");
  } else if (rainValue < 800) {
    Serial.println(" [건조]");
  } else {
    Serial.println(" [매우 건조]");
  }
  
  // 이전 상태 업데이트
  previousRainDetected = currentRainDetected;
  
  // 임계값 자동 조정 제안
  static int maxValue = 0;
  static int minValue = 1023;
  
  if (rainValue > maxValue) maxValue = rainValue;
  if (rainValue < minValue) minValue = rainValue;
  
  // 10초마다 범위 정보 표시
  static unsigned long lastInfo = 0;
  if (millis() - lastInfo > 10000) {
    Serial.println("--- 센서 범위 정보 ---");
    Serial.print("최소값: ");
    Serial.print(minValue);
    Serial.print(" | 최대값: ");
    Serial.println(maxValue);
    
    // 추천 임계값 (높은 값에서 낮은 값으로 가는 지점)
    int recommendedThreshold = maxValue - (maxValue - minValue) / 3;
    Serial.print("추천 임계값: ");
    Serial.print(recommendedThreshold);
    Serial.println(" (이 값 이하에서 빗물 감지)");
    Serial.println("=====================");
    
    lastInfo = millis();
  }
  
  delay(500); // 0.5초 대기
}