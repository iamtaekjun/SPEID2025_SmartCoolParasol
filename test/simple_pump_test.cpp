/*
 * 간단한 워터펌프 릴레이 테스트 코드
 * 5V DC 워터펌프 + 릴레이 모듈 기본 동작 확인용
 * SmartCool Parasol 프로젝트 - 기본 테스트 버전
 * 
 * 사용법:
 * 1. main.cpp를 백업
 * 2. 이 파일 내용을 src/main.cpp에 복사
 * 3. 업로드 후 시리얼 모니터로 테스트
 */

#include <Arduino.h>

// ============= 핀 정의 =============
#define RELAY_PIN 6         // 릴레이 모듈 제어 핀 (D6)
#define STATUS_LED 13       // 아두이노 내장 LED

// ============= 전역 변수 =============
bool pumpRunning = false;
unsigned long pumpStartTime = 0;

// ============= 함수 선언 =============
void printConnectionGuide();
void printMenu();
void handleCommand(char cmd);
void pumpON();
void pumpOFF();
void quickTest();
void longTest();
void pulseTest();
void printStatus();
void emergencyStop();

void setup() {
    // 시리얼 통신 시작
    Serial.begin(9600);
    delay(2000); // 시리얼 모니터 안정화 대기
    
    Serial.println("=========================================");
    Serial.println("   5V DC 워터펌프 릴레이 제어 테스트");
    Serial.println("=========================================");
    Serial.println("프로젝트: SmartCool Parasol");
    Serial.println("보드: Arduino UNO");
    Serial.println("제어: 릴레이 모듈 (5V)");
    Serial.println("=========================================");
    
    // 핀 모드 설정
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);
    
    // 초기 상태: 릴레이 OFF
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(STATUS_LED, LOW);
    
    Serial.println("하드웨어 초기화 완료");
    Serial.println();
    
    // 연결 확인 안내
    printConnectionGuide();
    
    // 기본 메뉴 출력
    printMenu();
}

void loop() {
    // 시리얼 명령어 처리
    if (Serial.available() > 0) {
        char command = Serial.read();
        handleCommand(command);
    }
    
    // 펌프 동작 중 상태 출력 (3초마다)
    if (pumpRunning) {
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate > 3000) {
            unsigned long runTime = (millis() - pumpStartTime) / 1000;
            Serial.print("펌프 가동 시간: ");
            Serial.print(runTime);
            Serial.println("초");
            lastUpdate = millis();
        }
    }
    
    delay(100);
}

void printConnectionGuide() {
    Serial.println("연결 가이드:");
    Serial.println("   릴레이 모듈 ← → Arduino UNO");
    Serial.println("   VCC ←----------→ 5V");
    Serial.println("   GND ←----------→ GND");
    Serial.println("   IN  ←----------→ D6");
    Serial.println();
    Serial.println("   워터펌프 ← → 릴레이 모듈");
    Serial.println("   + (빨강) ←------→ NO (상시열림)");
    Serial.println("   - (검정) ←------→ Arduino GND");
    Serial.println("   파워 ←----------→ COM ← Arduino 5V");
    Serial.println();
}

void printMenu() {
    Serial.println("테스트 명령어:");
    Serial.println("   1 = 펌프 ON");
    Serial.println("   0 = 펌프 OFF"); 
    Serial.println("   t = 빠른 테스트 (3초 ON)");
    Serial.println("   l = 긴 테스트 (10초 ON)");
    Serial.println("   p = 반복 테스트 (1초 ON/OFF x 5회)");
    Serial.println("   s = 현재 상태 확인");
    Serial.println("   h = 도움말");
    Serial.println("   x = 긴급 정지");
    Serial.println("=========================================");
    Serial.println();
}

void handleCommand(char cmd) {
    Serial.print("명령어 수신: ");
    Serial.println(cmd);
    
    switch(cmd) {
        case '1':
            pumpON();
            break;
            
        case '0':
            pumpOFF();
            break;
            
        case 't':
        case 'T':
            quickTest();
            break;
            
        case 'l':
        case 'L':
            longTest();
            break;
            
        case 'p':
        case 'P':
            pulseTest();
            break;
            
        case 's':
        case 'S':
            printStatus();
            break;
            
        case 'h':
        case 'H':
            printMenu();
            break;
            
        case 'x':
        case 'X':
            emergencyStop();
            break;
            
        default:
            Serial.println("알 수 없는 명령어");
            Serial.println("'h' 입력으로 도움말 확인");
            break;
    }
    Serial.println();
}

void pumpON() {
    Serial.println("워터펌프 ON");
    Serial.println("   릴레이: HIGH");
    
    // 릴레이 활성화 (워터펌프 전원 공급)
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(STATUS_LED, HIGH);
    
    pumpRunning = true;
    pumpStartTime = millis();
    
    // 안전 주의 메시지
    Serial.println("주의: 과전류나 과열 확인하세요!");
    Serial.println("   이상 발생 시 즉시 'x' 입력!");
}

void pumpOFF() {
    Serial.println("워터펌프 OFF");
    Serial.println("   릴레이: LOW");
    
    // 릴레이 비활성화 (워터펌프 전원 차단)
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(STATUS_LED, LOW);
    
    if (pumpRunning) {
        unsigned long runTime = (millis() - pumpStartTime) / 1000;
        Serial.print("   총 가동 시간: ");
        Serial.print(runTime);
        Serial.println("초");
    }
    
    pumpRunning = false;
}

void quickTest() {
    Serial.println("빠른 테스트 시작 (3초)");
    
    pumpON();
    
    // 3초 대기 (긴급정지 체크)
    for (int i = 3; i > 0; i--) {
        Serial.print("   ");
        Serial.print(i);
        Serial.println("초 남음...");
        
        // 100ms마다 긴급정지 체크
        for (int j = 0; j < 10; j++) {
            if (Serial.available() > 0 && Serial.read() == 'x') {
                emergencyStop();
                return;
            }
            delay(100);
        }
    }
    
    pumpOFF();
    Serial.println("빠른 테스트 완료");
}

void longTest() {
    Serial.println("긴 테스트 시작 (10초)");
    Serial.println("   긴급정지: 'x' 입력");
    
    pumpON();
    
    // 10초 대기 (긴급정지 체크)
    for (int i = 10; i > 0; i--) {
        Serial.print("   ");
        Serial.print(i);
        Serial.println("초 남음...");
        
        // 100ms마다 긴급정지 체크
        for (int j = 0; j < 10; j++) {
            if (Serial.available() > 0 && Serial.read() == 'x') {
                emergencyStop();
                return;
            }
            delay(100);
        }
    }
    
    pumpOFF();
    Serial.println("긴 테스트 완료");
}

void pulseTest() {
    Serial.println("반복 테스트 시작 (1초 ON/OFF x 5회)");
    Serial.println("   긴급정지: 'x' 입력");
    
    for (int cycle = 1; cycle <= 5; cycle++) {
        Serial.print("  사이클 ");
        Serial.print(cycle);
        Serial.println("/5");
        
        // 1초 ON
        Serial.println("     펌프 ON (1초)");
        pumpON();
        
        // 긴급정지 체크
        for (int i = 0; i < 10; i++) {
            if (Serial.available() > 0 && Serial.read() == 'x') {
                emergencyStop();
                return;
            }
            delay(100);
        }
        
        // 1초 OFF
        Serial.println("     펌프 OFF (1초)");
        pumpOFF();
        
        // 긴급정지 체크
        for (int i = 0; i < 10; i++) {
            if (Serial.available() > 0 && Serial.read() == 'x') {
                emergencyStop();
                return;
            }
            delay(100);
        }
    }
    
    Serial.println("반복 테스트 완료");
}

void printStatus() {
    Serial.println("현재 시스템 상태:");
    
    // 릴레이 상태
    bool relayState = digitalRead(RELAY_PIN);
    Serial.print("   릴레이 (D6): ");
    Serial.println(relayState ? "HIGH (ON)" : "LOW (OFF)");
    
    // 펌프 상태
    Serial.print("   워터펌프: ");
    if (pumpRunning) {
        unsigned long runTime = (millis() - pumpStartTime) / 1000;
        Serial.print("동작 중 (");
        Serial.print(runTime);
        Serial.println("초)");
    } else {
        Serial.println("정지");
    }
    
    // LED 상태
    bool ledState = digitalRead(STATUS_LED);
    Serial.print("   상태 LED: ");
    Serial.println(ledState ? "켜짐" : "꺼짐");
    
    // 시스템 정보
    Serial.print("   업타임: ");
    Serial.print(millis() / 1000);
    Serial.println("초");
    
    // 안전 체크 안내
    if (pumpRunning) {
        Serial.println("펌프 동작 중 - 과열/과전류 주의!");
    }
}

void emergencyStop() {
    Serial.println();
    Serial.println("긴급 정지!");
    
    // 즉시 모든 출력 정지
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(STATUS_LED, LOW);
    pumpRunning = false;
    
    Serial.println("   릴레이 긴급 차단 완료");
    Serial.println("   워터펌프 전원 차단 완료");
    Serial.println();
    Serial.println("점검 사항:");
    Serial.println("   1. 워터펌프 과열 여부");
    Serial.println("   2. 연결선 단락 여부");
    Serial.println("   3. 보조배터리 출력 전압");
    Serial.println("   4. 비정상적인 소음이나 진동");
    Serial.println();
    Serial.println("문제 해결 후 아두이노를 리셋하세요.");
    
    // 경고 LED 깜빡임
    while (true) {
        digitalWrite(STATUS_LED, HIGH);
        delay(200);
        digitalWrite(STATUS_LED, LOW);
        delay(200);
        
        // 리셋을 위한 추가 입력 체크
        if (Serial.available() > 0) {
            char input = Serial.read();
            if (input == 'r' || input == 'R') {
                Serial.println("시스템 재시작...");
                delay(1000);
                break; // while 루프 종료하여 정상 동작 복귀
            }
        }
    }
}
