/*
 * LED 테스트용 코드
 * Pin 10에 LED 연결해서 신호 확인
 * 부저 대신 LED로 동작 검증
 */

#include <Arduino.h>

#define LED_PIN 10         // 외부 LED (Pin 10)
#define BUILTIN_LED 13     // 내장 LED

void setup() {
  Serial.begin(9600);
  Serial.println("======================================");
  Serial.println("        LED 테스트 프로그램");
  Serial.println("======================================");
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  
  // 초기 상태
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUILTIN_LED, LOW);
  
  Serial.println("3초 후 테스트 시작...");
  delay(3000);
  
  // 테스트 1: 기본 ON/OFF
  Serial.println("\n[테스트 1] 기본 LED ON/OFF");
  Serial.println("Pin 10 LED: 3초간 켜기");
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUILTIN_LED, HIGH);  // 내장 LED도 같이
  delay(3000);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUILTIN_LED, LOW);
  Serial.println("LED 끄기");
  delay(2000);
  
  // 테스트 2: 깜빡임
  Serial.println("\n[테스트 2] 빠른 깜빡임 테스트");
  for(int i = 0; i < 10; i++) {
    Serial.print("깜빡 ");
    Serial.println(i + 1);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUILTIN_LED, LOW);
    delay(200);
  }
  
  Serial.println("\n초기 테스트 완료! 반복 테스트 시작...");
}

void loop() {
  Serial.println("\n===== LED 패턴 테스트 =====");
  
  // 패턴 1: SOS 신호 (... --- ...)
  Serial.println("패턴 1: SOS 신호");
  // S (짧은 신호 3번)
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(150);
  }
  delay(300);
  
  // O (긴 신호 3번)
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(150);
  }
  delay(300);
  
  // S (짧은 신호 3번)
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(150);
  }
  delay(2000);
  
  // 패턴 2: PWM 밝기 조절 (analogWrite)
  Serial.println("패턴 2: PWM 밝기 조절");
  Serial.println("어두움 → 밝음 → 어두움");
  
  // 어두움에서 밝음으로
  for(int brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(LED_PIN, brightness);
    delay(30);
  }
  
  // 밝음에서 어두움으로
  for(int brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(LED_PIN, brightness);
    delay(30);
  }
  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  
  // 패턴 3: 하트비트 (심장박동)
  Serial.println("패턴 3: 하트비트 패턴");
  for(int i = 0; i < 5; i++) {
    // 첫 번째 박동
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
    // 두 번째 박동
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(500);  // 긴 대기
  }
  
  Serial.println("다음 패턴까지 5초 대기...");
  delay(5000);
}