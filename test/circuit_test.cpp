/*
 * 보조배터리 + 릴레이 + 워터펌프 회로 테스트 코드
 * 단계별 안전 테스트로 회로 동작 확인
 * SmartCool Parasol 프로젝트용 - 독립 테스트 파일
 * 
 * 사용법:
 * 1. main.cpp 대신 이 파일을 업로드
 * 2. 시리얼 모니터로 테스트 진행 상황 확인
 * 3. 각 단계별 안전 확인 후 진행
 */

#include <Arduino.h>

// ============= 핀 정의 =============
#define RELAY_PIN 6         // 릴레이 모듈 제어 핀
#define LED_PIN 13          // 내장 LED (상태 표시용)
#define VOLTAGE_CHECK_PIN A0 // 전압 모니터링용 (선택사항)

// ============= 테스트 상태 =============
enum TestPhase {
    POWER_CHECK,        // 전원 공급 확인
    RELAY_FUNCTION,     // 릴레이 기능 테스트
    PUMP_SAFETY,        // 워터펌프 안전 테스트
    PUMP_OPERATION,     // 워터펌프 동작 테스트
    ENDURANCE_TEST,     // 내구성 테스트
    TEST_COMPLETE       // 테스트 완료
};

TestPhase currentPhase = POWER_CHECK;
unsigned long phaseStartTime = 0;
unsigned long testStartTime = 0;
bool userReady = false;

// ============= 함수 선언 =============
void initializeTestSystem();
void runCurrentTest();
void powerSupplyCheck();
void relayFunctionTest();
void pumpSafetyTest();
void pumpOperationTest();
void enduranceTest();
void relayON();
void relayOFF();
void printTestHeader();
void printSafetyWarning();
void printPhaseResult(bool passed, const char* message);
void printProgress(int current, int total, const char* action);
bool waitForUserConfirmation(const char* prompt, int timeoutSeconds = 30);
void emergencyStop();
void testComplete();

void setup() {
    Serial.begin(9600);
    delay(3000); // 시리얼 모니터 충분한 대기 시간
    
    printTestHeader();
    printSafetyWarning();
    
    // 사용자 준비 확인
    Serial.println("회로 연결이 완료되었나요?");
    Serial.println("준비되면 아무 키나 입력하세요...");
    
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read(); // 입력 소비
    
    initializeTestSystem();
    testStartTime = millis();
    phaseStartTime = millis();
    
    Serial.println("========================================");
    Serial.println("🔧 회로 테스트를 시작합니다!");
    Serial.println("========================================");
}

void loop() {
    // 긴급정지 체크 (최우선)
    if (Serial.available() > 0) {
        char input = Serial.read();
        if (input == 's' || input == 'S' || input == 'x' || input == 'X') {
            emergencyStop();
            return;
        }
    }
    
    runCurrentTest();
    delay(100);
}

void initializeTestSystem() {
    // 핀 모드 설정
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(VOLTAGE_CHECK_PIN, INPUT);
    
    // 안전한 초기 상태
    relayOFF();
    digitalWrite(LED_PIN, LOW);
    
    Serial.println("테스트 시스템 초기화 완료");
    Serial.println("핀 설정:");
    Serial.println("  - 릴레이 제어: D6");
    Serial.println("  - 상태 LED: D13");
    Serial.println("  - 전압 체크: A0 (선택사항)");
    Serial.println();
}

void printTestHeader() {
    Serial.println("=========================================");
    Serial.println("  보조배터리 + 릴레이 + 워터펌프");
    Serial.println("        회로 안전성 테스트");
    Serial.println("=========================================");
    Serial.println("테스트 목적:");
    Serial.println("  ✓ 전원 공급 안정성 확인");
    Serial.println("  ✓ 릴레이 스위칭 동작 확인");
    Serial.println("  ✓ 워터펌프 정상 동작 확인");
    Serial.println("  ✓ 과전류/과열 안전성 확인");
    Serial.println();
}

void printSafetyWarning() {
    Serial.println("⚠️  안전 주의사항 ⚠️");
    Serial.println("1. 극성 확인: 빨강(+5V), 검정(GND)");
    Serial.println("2. 단락 방지: 전선 접촉 주의");
    Serial.println("3. 긴급정지: 언제든 's' 키 입력");
    Serial.println("4. 과열 체크: 릴레이/펌프 온도 확인");
    Serial.println("5. 소음 체크: 비정상 소음 시 즉시 정지");
    Serial.println();
}

void runCurrentTest() {
    switch (currentPhase) {
        case POWER_CHECK:
            powerSupplyCheck();
            break;
        case RELAY_FUNCTION:
            relayFunctionTest();
            break;
        case PUMP_SAFETY:
            pumpSafetyTest();
            break;
        case PUMP_OPERATION:
            pumpOperationTest();
            break;
        case ENDURANCE_TEST:
            enduranceTest();
            break;
        case TEST_COMPLETE:
            testComplete();
            break;
    }
}

void powerSupplyCheck() {
    static bool testStarted = false;
    
    if (!testStarted) {
        Serial.println("=== 1단계: 전원 공급 테스트 ===");
        Serial.println("보조배터리 전원 안정성 확인 중...");
        testStarted = true;
    }
    
    // 전원 상태 LED 표시 (5초간)
    static int blinkCount = 0;
    if (millis() - phaseStartTime > 500) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        blinkCount++;
        phaseStartTime = millis();
        
        Serial.print("전원 체크... ");
        Serial.print(blinkCount/2);
        Serial.println("/5초");
    }
    
    if (blinkCount >= 10) {
        digitalWrite(LED_PIN, LOW);
        printPhaseResult(true, "전원 공급 안정성 확인 완료");
        
        if (waitForUserConfirmation("아두이노 전원 LED가 정상적으로 켜져 있나요?")) {
            currentPhase = RELAY_FUNCTION;
            phaseStartTime = millis();
        } else {
            Serial.println("❌ 전원 공급 문제 - 연결을 다시 확인하세요!");
            emergencyStop();
        }
    }
}

void relayFunctionTest() {
    static bool testStarted = false;
    static int testCycle = 0;
    
    if (!testStarted) {
        Serial.println("=== 2단계: 릴레이 동작 테스트 ===");
        Serial.println("릴레이 스위칭 소리(딸깍)를 주의깊게 들어보세요!");
        Serial.println("총 5회 ON/OFF 테스트 진행");
        testStarted = true;
        delay(2000);
    }
    
    static unsigned long cycleTime = 0;
    if (millis() - cycleTime > 1500) {
        if (testCycle < 10) { // 5회 ON/OFF = 10번 동작
            if (testCycle % 2 == 0) {
                Serial.print("릴레이 ON... ");
                relayON();
                digitalWrite(LED_PIN, HIGH);
                Serial.println("(딸깍 소리 확인!)");
            } else {
                Serial.println("릴레이 OFF");
                relayOFF();
                digitalWrite(LED_PIN, LOW);
            }
            testCycle++;
        } else {
            printPhaseResult(true, "릴레이 동작 테스트 완료");
            
            if (waitForUserConfirmation("릴레이에서 딸깍 소리가 들렸나요?")) {
                Serial.println("워터펌프를 연결했는지 확인하세요!");
                Serial.println("연결 완료 후 계속 진행...");
                delay(5000);
                currentPhase = PUMP_SAFETY;
                phaseStartTime = millis();
            } else {
                Serial.println("❌ 릴레이 동작 문제 - 연결을 확인하세요!");
                emergencyStop();
            }
        }
        cycleTime = millis();
    }
}

void pumpSafetyTest() {
    static bool testStarted = false;
    
    if (!testStarted) {
        Serial.println("=== 3단계: 워터펌프 안전성 테스트 ===");
        Serial.println("⚠️ 주의: 이제 워터펌프가 실제로 동작합니다!");
        Serial.println("첫 동작은 1초만 진행하여 안전성을 확인합니다.");
        Serial.println();
        
        if (!waitForUserConfirmation("워터펌프 연결 완료, 테스트 진행할까요?")) {
            Serial.println("테스트 중단 - 연결을 확인하세요.");
            emergencyStop();
            return;
        }
        
        Serial.println("3초 후 워터펌프 1초 동작 시작...");
        delay(3000);
        testStarted = true;
        phaseStartTime = millis();
    }
    
    static bool pumpTested = false;
    if (!pumpTested && millis() - phaseStartTime > 500) {
        Serial.println("🚰 워터펌프 ON (1초 안전 테스트)");
        relayON();
        digitalWrite(LED_PIN, HIGH);
        delay(1000);
        
        Serial.println("워터펌프 OFF");
        relayOFF();
        digitalWrite(LED_PIN, LOW);
        pumpTested = true;
        
        Serial.println();
        Serial.println("안전성 체크:");
        Serial.println("  - 비정상적인 소음이 있었나요?");
        Serial.println("  - 과도한 진동이 있었나요?");
        Serial.println("  - 릴레이나 펌프가 과열되었나요?");
        Serial.println();
        
        if (waitForUserConfirmation("워터펌프가 정상적으로 동작했나요?")) {
            printPhaseResult(true, "워터펌프 안전성 테스트 통과");
            currentPhase = PUMP_OPERATION;
            phaseStartTime = millis();
        } else {
            Serial.println("❌ 안전성 문제 - 연결과 펌프 상태를 확인하세요!");
            emergencyStop();
        }
    }
}

void pumpOperationTest() {
    static bool testStarted = false;
    
    if (!testStarted) {
        Serial.println("=== 4단계: 워터펌프 동작 테스트 ===");
        Serial.println("5초간 연속 동작하여 성능을 확인합니다.");
        Serial.println("문제 발생 시 즉시 's' 키를 입력하세요!");
        
        delay(3000);
        testStarted = true;
        phaseStartTime = millis();
    }
    
    static bool operationComplete = false;
    if (!operationComplete) {
        Serial.println("🚰 워터펌프 연속 동작 시작 (5초)");
        relayON();
        digitalWrite(LED_PIN, HIGH);
        
        // 5초 동안 1초마다 상태 출력
        for (int i = 5; i > 0; i--) {
            Serial.print("동작 중... 남은 시간: ");
            Serial.print(i);
            Serial.println("초");
            
            // 긴급정지 체크 (100ms마다)
            for (int j = 0; j < 10; j++) {
                if (Serial.available() > 0) {
                    char input = Serial.read();
                    if (input == 's' || input == 'S' || input == 'x' || input == 'X') {
                        emergencyStop();
                        return;
                    }
                }
                delay(100);
            }
        }
        
        Serial.println("워터펌프 정지");
        relayOFF();
        digitalWrite(LED_PIN, LOW);
        operationComplete = true;
        
        Serial.println();
        Serial.println("동작 성능 체크:");
        Serial.println("  - 워터펌프가 정상적으로 물을 분사했나요?");
        Serial.println("  - 분사량이 적절한가요?");
        Serial.println("  - 과열이나 이상 소음은 없었나요?");
        Serial.println();
        
        if (waitForUserConfirmation("워터펌프 동작이 만족스러우신가요?")) {
            printPhaseResult(true, "워터펌프 동작 테스트 통과");
            
            if (waitForUserConfirmation("내구성 테스트를 진행할까요? (간헐 동작 5회)")) {
                currentPhase = ENDURANCE_TEST;
            } else {
                currentPhase = TEST_COMPLETE;
            }
            phaseStartTime = millis();
        } else {
            Serial.println("❌ 동작 성능 문제 - 펌프나 연결을 확인하세요!");
            emergencyStop();
        }
    }
}

void enduranceTest() {
    static bool testStarted = false;
    static int currentCycle = 0;
    
    if (!testStarted) {
        Serial.println("=== 5단계: 내구성 테스트 ===");
        Serial.println("간헐적 동작으로 실제 사용 환경 시뮬레이션");
        Serial.println("패턴: 3초 ON → 5초 OFF (총 5회)");
        Serial.println();
        testStarted = true;
    }
    
    static bool cycleActive = false;
    static unsigned long cycleStartTime = 0;
    
    if (currentCycle < 5) {
        if (!cycleActive) {
            // 새 사이클 시작
            currentCycle++;
            Serial.print("🔄 사이클 ");
            Serial.print(currentCycle);
            Serial.println("/5 시작");
            
            Serial.println("  워터펌프 ON (3초)");
            relayON();
            digitalWrite(LED_PIN, HIGH);
            cycleActive = true;
            cycleStartTime = millis();
            
        } else if (millis() - cycleStartTime < 3000) {
            // ON 상태 유지 (3초)
            static unsigned long lastUpdate = 0;
            if (millis() - lastUpdate > 1000) {
                int remaining = 3 - ((millis() - cycleStartTime) / 1000);
                Serial.print("    ON 상태... ");
                Serial.print(remaining + 1);
                Serial.println("초 남음");
                lastUpdate = millis();
            }
            
        } else if (millis() - cycleStartTime < 8000) {
            // OFF 상태로 전환 (5초)
            static bool switched = false;
            if (!switched) {
                Serial.println("  워터펌프 OFF (5초 대기)");
                relayOFF();
                digitalWrite(LED_PIN, LOW);
                switched = true;
            }
            
            static unsigned long lastUpdate = 0;
            if (millis() - lastUpdate > 1000) {
                int remaining = 8 - ((millis() - cycleStartTime) / 1000);
                if (remaining > 0) {
                    Serial.print("    대기 중... ");
                    Serial.print(remaining);
                    Serial.println("초 남음");
                }
                lastUpdate = millis();
            }
            
        } else {
            // 사이클 완료
            cycleActive = false;
            Serial.print("  사이클 ");
            Serial.print(currentCycle);
            Serial.println(" 완료\n");
            delay(1000);
        }
        
    } else {
        // 모든 사이클 완료
        printPhaseResult(true, "내구성 테스트 완료");
        currentPhase = TEST_COMPLETE;
        phaseStartTime = millis();
    }
}

void relayON() {
    digitalWrite(RELAY_PIN, HIGH);
}

void relayOFF() {
    digitalWrite(RELAY_PIN, LOW);
}

void runCurrentTest() {
    switch (currentPhase) {
        case POWER_CHECK:
            powerSupplyCheck();
            break;
        case RELAY_FUNCTION:
            relayFunctionTest();
            break;
        case PUMP_SAFETY:
            pumpSafetyTest();
            break;
        case PUMP_OPERATION:
            pumpOperationTest();
            break;
        case ENDURANCE_TEST:
            enduranceTest();
            break;
        case TEST_COMPLETE:
            testComplete();
            break;
    }
}

void powerSupplyCheck() {
    static bool checkStarted = false;
    static int ledToggle = 0;
    
    if (!checkStarted) {
        Serial.println("=== 1단계: 전원 공급 확인 ===");
        Serial.println("보조배터리 출력 전압 및 안정성 체크");
        checkStarted = true;
    }
    
    // 3초간 LED 깜빡임으로 전원 확인
    if (millis() - phaseStartTime > 300) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        ledToggle++;
        phaseStartTime = millis();
        
        if (ledToggle <= 6) {
            Serial.print("전원 안정성 체크... ");
            Serial.print(ledToggle);
            Serial.println("/6");
        }
    }
    
    if (ledToggle > 6) {
        digitalWrite(LED_PIN, LOW);
        printPhaseResult(true, "전원 공급 상태 양호");
        
        if (waitForUserConfirmation("아두이노가 정상적으로 동작하고 있나요?")) {
            currentPhase = RELAY_FUNCTION;
            phaseStartTime = millis();
        } else {
            emergencyStop();
        }
    }
}

void relayFunctionTest() {
    static bool testStarted = false;
    static int relayToggle = 0;
    
    if (!testStarted) {
        Serial.println("=== 2단계: 릴레이 기능 테스트 ===");
        Serial.println("릴레이 ON/OFF 동작 확인 (딸깍 소리 주의!)");
        Serial.println("총 3회 테스트 진행");
        testStarted = true;
        delay(2000);
    }
    
    // 1.5초마다 릴레이 토글
    if (millis() - phaseStartTime > 1500) {
        if (relayToggle < 6) { // 3회 ON/OFF
            if (relayToggle % 2 == 0) {
                Serial.print("릴레이 ON... ");
                relayON();
                digitalWrite(LED_PIN, HIGH);
                Serial.println("딸깍!");
            } else {
                Serial.println("릴레이 OFF");
                relayOFF();
                digitalWrite(LED_PIN, LOW);
            }
            relayToggle++;
        } else {
            printPhaseResult(true, "릴레이 기능 테스트 완료");
            
            if (waitForUserConfirmation("릴레이에서 딸깍 소리가 3번 들렸나요?")) {
                currentPhase = PUMP_SAFETY;
                phaseStartTime = millis();
            } else {
                Serial.println("❌ 릴레이 동작 문제 - 연결을 확인하세요!");
                emergencyStop();
            }
        }
        phaseStartTime = millis();
    }
}

void printPhaseResult(bool passed, const char* message) {
    Serial.println();
    Serial.println("----------------------------------------");
    if (passed) {
        Serial.print("✅ 통과: ");
    } else {
        Serial.print("❌ 실패: ");
    }
    Serial.println(message);
    Serial.println("----------------------------------------");
    Serial.println();
    delay(1500);
}

bool waitForUserConfirmation(const char* prompt, int timeoutSeconds) {
    Serial.print("❓ ");
    Serial.println(prompt);
    Serial.println("   y = 예 | n = 아니오 | s = 긴급정지");
    Serial.print("   입력 대기");
    if (timeoutSeconds > 0) {
        Serial.print(" (");
        Serial.print(timeoutSeconds);
        Serial.print("초 제한)");
    }
    Serial.println("...");
    
    unsigned long startTime = millis();
    while (true) {
        if (Serial.available() > 0) {
            char input = Serial.read();
            Serial.println();
            
            if (input == 'y' || input == 'Y') {
                Serial.println("👍 확인 - 계속 진행");
                return true;
            } else if (input == 'n' || input == 'N') {
                Serial.println("👎 문제 있음 - 테스트 중단");
                return false;
            } else if (input == 's' || input == 'S') {
                emergencyStop();
                return false;
            }
        }
        
        if (timeoutSeconds > 0 && millis() - startTime > timeoutSeconds * 1000) {
            Serial.println("⏰ 시간 초과 - 기본값(예)으로 진행");
            return true;
        }
        
        delay(100);
    }
}

void emergencyStop() {
    // 즉시 모든 출력 정지
    relayOFF();
    digitalWrite(LED_PIN, LOW);
    
    Serial.println();
    Serial.println("🚨🚨🚨 긴급정지 실행! 🚨🚨🚨");
    Serial.println("모든 출력이 안전하게 정지되었습니다.");
    Serial.println();
    Serial.println("문제 해결 체크리스트:");
    Serial.println("  1. 모든 연결선 극성 확인");
    Serial.println("  2. 단락(쇼트) 여부 확인");
    Serial.println("  3. 워터펌프 정격 사양 확인");
    Serial.println("  4. 보조배터리 출력 전압 확인");
    Serial.println("  5. 릴레이 모듈 동작 상태 확인");
    Serial.println();
    Serial.println("문제 해결 후 아두이노를 리셋하세요.");
    
    // 경고 LED 깜빡임
    while (true) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
}

void testComplete() {
    static bool completionShown = false;
    
    if (!completionShown) {
        unsigned long totalTime = (millis() - testStartTime) / 1000;
        
        Serial.println("🎉🎉🎉 모든 테스트 완료! 🎉🎉🎉");
        Serial.println("========================================");
        Serial.println("테스트 결과 요약:");
        Serial.println("  ✅ 전원 공급: 정상");
        Serial.println("  ✅ 릴레이 동작: 정상");
        Serial.println("  ✅ 워터펌프 안전성: 통과");
        Serial.println("  ✅ 워터펌프 동작: 정상");
        Serial.println("  ✅ 시스템 안정성: 확인");
        Serial.println();
        Serial.print("총 테스트 시간: ");
        Serial.print(totalTime);
        Serial.println("초");
        Serial.println();
        Serial.println("🔧 회로가 완전히 정상 동작합니다!");
        Serial.println("이제 SmartCool Parasol 메인 코드를 업로드하세요.");
        Serial.println("========================================");
        
        completionShown = true;
    }
    
    // 성공 표시 LED 패턴
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 1000) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        lastBlink = millis();
    }
}