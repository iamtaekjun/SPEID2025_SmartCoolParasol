/*
 * ========================================
 * SmartCool Parasol - 간단 발표용 데모
 * ========================================
 * 
 * 발표 중 빠른 시연을 위한 단순화된 버전
 * 
 * 키 입력:
 * H: Heat (더위 모드)
 * R: Rain (빗물 모드)  
 * M: Mist (미스트 모드)
 * S: Stop (정지)
 */

#include <Arduino.h>
#include <Servo.h>

// 핀 정의
#define SERVO_PIN 9
#define RELAY_PIN 6

Servo parasol;

void setup() {
    Serial.begin(9600);
    
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    
    parasol.attach(SERVO_PIN);
    parasol.write(40); // 수납 위치
    
    Serial.println("=== SmartCool Parasol 간단 데모 ===");
    Serial.println("H: 더위모드 | R: 빗물모드 | M: 미스트 | S: 정지");
    Serial.println("=====================================");
}

void loop() {
    if (Serial.available()) {
        char cmd = Serial.read();
        
        switch(cmd) {
            case 'H':
            case 'h':
                heatMode();
                break;
                
            case 'R': 
            case 'r':
                rainMode();
                break;
                
            case 'M':
            case 'm':
                mistMode();
                break;
                
            case 'S':
            case 's':
                stopMode();
                break;
        }
    }
}

void heatMode() {
    Serial.println("더위 대응 모드");
    Serial.println("→ 파라솔 전개 (90도)");
    parasol.write(90);
    delay(2000);
    
    Serial.println("→ 미스트 분사 시작");
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("더위 모드 활성화!");
}

void rainMode() {
    Serial.println("빗물 수집 모드");
    Serial.println("→ 워터펌프 정지");
    digitalWrite(RELAY_PIN, LOW);
    delay(500);
    
    Serial.println("→ 파라솔 빗물수집 각도 (140도)");
    parasol.write(140);
    Serial.println("빗물 수집 모드 활성화!");
}

void mistMode() {
    Serial.println("미스트 분사 모드");
    Serial.println("→ 워터펌프 가동");
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("미스트 분사 중!");
}

void stopMode() {
    Serial.println("시스템 정지");
    Serial.println("→ 워터펌프 정지");
    digitalWrite(RELAY_PIN, LOW);
    delay(500);
    
    Serial.println("→ 파라솔 수납 (40도)");
    parasol.write(40);
    Serial.println("대기 모드");
}
