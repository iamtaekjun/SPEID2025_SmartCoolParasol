/*
 * ========================================
 * SmartCool Parasol - 발표용 데모 코드
 * ========================================
 * 
 * 시리얼 모니터에서 숫자 키를 입력하여 각 시나리오를 테스트
 * 
 * 1: 더위 대응 모드 (Heat Response Mode)
 * 2: 빗물 수집 모드 (Rain Collection Mode)  
 * 3: 미스트 분사 모드 (Mist Spray Mode)
 * 4: 파라솔 전개/수납 테스트 (Parasol Test)
 * 5: 시스템 상태 체크 (System Status)
 * 0: 대기 모드 (Standby Mode)
 * 
 * PlatformIO 환경용
 */

#include <Arduino.h>
#include <Servo.h>

// ============= 핀 정의 =============
#define TEMP_SENSOR_PIN A4      // KY-013 아날로그 온도센서 핀
#define RAIN_SENSOR_PIN A0      // 빗물 감지 센서 (아날로그)
#define WATER_LEVEL_PIN A3      // 물탱크 수위 센서 (아날로그)
#define SERVO_PIN 9             // 파라솔 구동 서보모터 
#define RELAY_PIN 6             // 릴레이 모듈 제어 핀

// ============= 객체 초기화 =============
Servo parasolServo;

// ============= 전역 변수 =============
bool demoMode = false;
int currentDemo = 0;
unsigned long demoStartTime = 0;
bool systemReady = false;

// 데모용 상태 변수
struct DemoStatus {
    bool parasolDeployed;
    bool pumpActive;
    bool rainCollection;
    bool heatAlert;
    int operationMode; // 0: 대기, 1: 빗물, 2: 더위, 3: 미스트
} demo;

// ============= 함수 선언 =============
void initializeDemo();
void printMenu();
void handleSerialInput();
void executeDemo(int demoNumber);
void demoHeatResponse();
void demoRainCollection();
void demoMistSpray();
void demoParasolTest();
void demoSystemStatus();
void demoStandbyMode();
void relayON();
void relayOFF();
void readRealSensors();
void printSensorReadings();

void setup() {
    Serial.begin(9600);
    delay(1000);
    
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║     SmartCool Parasol 데모 시스템     ║");
    Serial.println("║        발표용 인터랙티브 테스트        ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();
    
    initializeDemo();
    printMenu();
}

void loop() {
    // 시리얼 입력 처리
    handleSerialInput();
    
    // 데모 실행 중일 때 상태 업데이트
    if (demoMode) {
        // 10초 후 자동으로 대기 모드로 복귀
        if (millis() - demoStartTime > 10000) {
            Serial.println("\n데모 시간 종료 - 대기 모드로 복귀");
            demoStandbyMode();
            printMenu();
        }
    }
    
    delay(100);
}

void initializeDemo() {
    Serial.println("하드웨어 초기화 중...");
    
    // 핀 설정
    pinMode(RELAY_PIN, OUTPUT);
    relayOFF();
    
    // 서보모터 초기화
    parasolServo.attach(SERVO_PIN);
    parasolServo.write(40);  // 초기 위치 (수납 상태)
    delay(1000);
    
    // 상태 초기화
    demo.parasolDeployed = false;
    demo.pumpActive = false;
    demo.rainCollection = false;
    demo.heatAlert = false;
    demo.operationMode = 0;
    
    systemReady = true;
    Serial.println("시스템 준비 완료!\n");
}

void printMenu() {
    Serial.println("┌─────────────────────────────────────┐");
    Serial.println("│           데모 메뉴 선택            │");
    Serial.println("├─────────────────────────────────────┤");
    Serial.println("│ 1  더위 대응 모드                   │");
    Serial.println("│ 2  빗물 수집 모드                   │");
    Serial.println("│ 3  미스트 분사 모드                 │");
    Serial.println("│ 4  파라솔 동작 테스트               │");
    Serial.println("│ 5  센서 상태 체크                   │");
    Serial.println("│ 0  대기 모드 (초기화)               │");
    Serial.println("└─────────────────────────────────────┘");
    Serial.println("숫자를 입력하고 Enter를 누르세요:");
}

void handleSerialInput() {
    if (Serial.available() > 0) {
        int input = Serial.parseInt();
        
        if (input >= 0 && input <= 5) {
            executeDemo(input);
        } else {
            Serial.println("잘못된 입력입니다. 0-5 사이의 숫자를 입력하세요.");
        }
        
        // 시리얼 버퍼 비우기
        while (Serial.available() > 0) {
            Serial.read();
        }
    }
}

void executeDemo(int demoNumber) {
    currentDemo = demoNumber;
    demoStartTime = millis();
    demoMode = (demoNumber != 0);
    
    Serial.println("\n" + String("=").substring(0, 50));
    
    switch (demoNumber) {
        case 1:
            demoHeatResponse();
            break;
        case 2:
            demoRainCollection();
            break;
        case 3:
            demoMistSpray();
            break;
        case 4:
            demoParasolTest();
            break;
        case 5:
            demoSystemStatus();
            break;
        case 0:
            demoStandbyMode();
            printMenu();
            break;
        default:
            Serial.println("지원하지 않는 데모입니다.");
            break;
    }
}

void demoHeatResponse() {
    Serial.println("더위 대응 모드 데모 시작");
    Serial.println("시나리오: 온도 상승 감지 → 파라솔 전개 → 미스트 분사");
    Serial.println();
    
    // 1단계: 온도 상승 시뮬레이션
    Serial.println("1 온도 상승 감지 중...");
    Serial.println("   온도: 28°C → 32°C → 35°C");
    delay(1500);
    
    // 2단계: 파라솔 전개
    Serial.println("2 파라솔 전개 중...");
    Serial.println("   차양용 각도로 전개 (90도)");
    parasolServo.write(90);
    demo.parasolDeployed = true;
    demo.operationMode = 2;
    delay(2000);
    
    // 3단계: 미스트 분사 시작
    Serial.println("3 미스트 분사 시작...");
    Serial.println("   워터펌프 가동 (릴레이 ON)");
    relayON();
    demo.pumpActive = true;
    demo.heatAlert = true;
    delay(2000);
    
    Serial.println("더위 대응 모드 활성화 완료!");
    Serial.println("10초 후 자동으로 대기 모드로 복귀합니다...");
}

void demoRainCollection() {
    Serial.println("빗물 수집 모드 데모 시작");
    Serial.println("시나리오: 빗물 감지 → 파라솔 전개 → 빗물 수집");
    Serial.println();
    
    // 1단계: 빗물 감지 시뮬레이션
    Serial.println("1 빗물 감지 중...");
    Serial.println("   빗물 센서: 800 → 650 → 500 (감지!)");
    delay(1500);
    
    // 2단계: 워터펌프 정지 (빗물 수집 우선)
    Serial.println("2 워터펌프 정지...");
    Serial.println("   빗물 수집을 위해 미스트 분사 중단");
    relayOFF();
    demo.pumpActive = false;
    delay(1000);
    
    // 3단계: 파라솔 전개 (빗물 수집용)
    Serial.println("3 파라솔 빗물 수집 모드 전개...");
    Serial.println("   빗물 수집용 각도로 전개 (140도)");
    parasolServo.write(140);
    demo.parasolDeployed = true;
    demo.rainCollection = true;
    demo.operationMode = 1;
    delay(2000);
    
    Serial.println("빗물 수집 모드 활성화 완료!");
    Serial.println("물탱크에 빗물이 수집되고 있습니다...");
    Serial.println("10초 후 자동으로 대기 모드로 복귀합니다...");
}

void demoMistSpray() {
    Serial.println("미스트 분사 모드 데모 시작");
    Serial.println("시나리오: 수위 확인 → 워터펌프 가동 → 미스트 분사");
    Serial.println();
    
    // 1단계: 수위 확인 시뮬레이션
    Serial.println("1 물탱크 수위 확인 중...");
    Serial.println("   현재 수위: 75% (임계값 이상)");
    delay(1500);
    
    // 2단계: 워터펌프 가동
    Serial.println("2 워터펌프 가동...");
    Serial.println("   릴레이 ON - 미스트 노즐 활성화");
    relayON();
    demo.pumpActive = true;
    delay(2000);
    
    // 3단계: 미스트 분사 효과 시뮬레이션
    Serial.println("3 미스트 분사 중...");
    Serial.println("   시원한 미스트가 분사됩니다");
    Serial.println("   체감온도 5°C 하락 효과");
    delay(2000);
    
    Serial.println("미스트 분사 모드 활성화 완료!");
    Serial.println("쿨링 효과를 확인하세요!");
    Serial.println("10초 후 자동으로 대기 모드로 복귀합니다...");
}

void demoParasolTest() {
    Serial.println("파라솔 동작 테스트 시작");
    Serial.println("시나리오: 다양한 각도로 파라솔 동작 테스트");
    Serial.println();
    
    // 수납 상태에서 시작
    Serial.println("1 수납 상태 (40도)");
    parasolServo.write(40);
    delay(2000);
    
    // 차양 모드
    Serial.println("2 차양 모드 (90도)");
    parasolServo.write(90);
    demo.parasolDeployed = true;
    delay(2000);
    
    // 빗물 수집 모드
    Serial.println("3 빗물 수집 모드 (140도)");
    parasolServo.write(140);
    delay(2000);
    
    // 최대 전개
    Serial.println("4 최대 전개 (180도)");
    parasolServo.write(180);
    delay(2000);
    
    // 원래 위치로 복귀
    Serial.println("5 수납 위치로 복귀 (40도)");
    parasolServo.write(40);
    demo.parasolDeployed = false;
    delay(2000);
    
    Serial.println("파라솔 동작 테스트 완료!");
    Serial.println("5초 후 자동으로 대기 모드로 복귀합니다...");
    demoStartTime = millis() - 5000; // 5초 후 종료
}

void demoSystemStatus() {
    Serial.println("시스템 상태 체크");
    Serial.println("실시간 센서 데이터 및 시스템 상태 확인");
    Serial.println();
    
    // 실제 센서 데이터 읽기
    readRealSensors();
    
    // 센서 데이터 출력
    printSensorReadings();
    
    // 시스템 상태 출력
    Serial.println("시스템 상태:");
    Serial.println("   파라솔: " + String(demo.parasolDeployed ? "전개됨" : "수납됨"));
    Serial.println("   워터펌프: " + String(demo.pumpActive ? "가동중" : "정지"));
    Serial.println("   동작모드: " + String(demo.operationMode == 0 ? "대기" : 
                                        demo.operationMode == 1 ? "빗물수집" : 
                                        demo.operationMode == 2 ? "더위대응" : "알수없음"));
    
    Serial.println("\n팁: 다른 데모를 실행해보세요!");
    Serial.println("10초 후 자동으로 메뉴로 돌아갑니다...");
}

void demoStandbyMode() {
    Serial.println("대기 모드로 전환");
    Serial.println("모든 장치를 초기 상태로 복귀");
    Serial.println();
    
    // 모든 장치 정지
    relayOFF();
    parasolServo.write(40);
    
    // 상태 초기화
    demo.parasolDeployed = false;
    demo.pumpActive = false;
    demo.rainCollection = false;
    demo.heatAlert = false;
    demo.operationMode = 0;
    
    demoMode = false;
    
    Serial.println("대기 모드 활성화 완료");
    Serial.println("모든 시스템이 대기 상태입니다");
    Serial.println();
}

void relayON() {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("   워터펌프 ON (릴레이 HIGH)");
}

void relayOFF() {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("   워터펌프 OFF (릴레이 LOW)");
}

void readRealSensors() {
    // 실제 센서값 읽기 (참고용)
    int tempRaw = analogRead(TEMP_SENSOR_PIN);
    int rainRaw = analogRead(RAIN_SENSOR_PIN);
    int waterRaw = analogRead(WATER_LEVEL_PIN);
    
    // 온도 계산 (KY-013)
    float voltage = (tempRaw / 1023.0) * 5.0;
    float temperature = (voltage - 0.5) * 100.0;
    
    Serial.println("실시간 센서 데이터:");
    Serial.println("   온도: " + String(temperature, 1) + "°C (Raw: " + String(tempRaw) + ")");
    Serial.println("   빗물: " + String(rainRaw) + " (임계값: 700 이하)");
    Serial.println("   수위: " + String(waterRaw) + " (임계값: 600 이상)");
    Serial.println();
}

void printSensorReadings() {
    Serial.println("센서 연결 상태:");
    Serial.println("   KY-013 온도센서 (A4핀)");
    Serial.println("   빗물감지센서 (A0핀)"); 
    Serial.println("   수위센서 (A3핀)");
    Serial.println();
    
    Serial.println("액추에이터 상태:");
    Serial.println("   서보모터 (9핀)");
    Serial.println("   워터펌프릴레이 (6핀)");
    Serial.println();
}
