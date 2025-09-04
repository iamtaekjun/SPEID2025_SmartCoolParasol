// 간단한 시리얼 테스트 코드
#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    delay(2000); // 2초 대기
    
    Serial.println("=== Arduino 시리얼 테스트 ===");
    Serial.println("시리얼 통신이 정상 작동합니다!");
    Serial.println("DHT11 연결 전 기본 테스트");
    
    // 내장 LED 테스트
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("내장 LED 테스트 시작...");
}

void loop() {
    static int count = 0;
    
    count++;
    Serial.print("카운트: ");
    Serial.println(count);
    
    // LED 깜빡임
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}
