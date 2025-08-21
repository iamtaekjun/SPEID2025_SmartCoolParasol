/*
 * SmartCool Parasol 시스템 - PP-A285 플로트 스위치 적용
 * 물 수위 임계치 기반 미스트 분사 제어
 * 배수 밸브 제거, 단순화된 물 관리 시스템
 * PlatformIO 환경용
 */

#include <Arduino.h>
#include <Servo.h>

// ============= 핀 정의 =============
// 센서 레이어 (Sensor Layer)
#define LM35_PIN A4         // LM35 출력 Vout 연결 핀(아날로그)
#define RAIN_SENSOR_PIN A0  // 빗물 감지 센서 (아날로그)
#define WATER_LEVEL_PIN A3  // PP-A285 물탱크 수위 센서 (디지털 플로트 스위치)

// 액추에이터 레이어 (Actuator Layer)
#define SERVO_PIN 9      // 파라솔 구동 서보모터
#define WATER_PUMP_PIN 6 // 물 분사 펌프
#define MIST_SPRAY_PIN 5 // 미스트 분사

// 인프라 레이어 제거 - 배수는 구멍으로 자연 배수

// ============= 객체 초기화 =============
Servo parasolServo;

// ============= 전역 변수 =============
// 센서 값 저장 구조체
struct SensorData {
    float temperature;
    float humidity;
    int rainLevel;
    int lightLevel;
    bool waterLevelOK;     // PP-A285 플로트 스위치 상태 (충분한 물 = true)
    bool isValid;
} sensors;

// 시스템 상태 구조체
struct SystemStatus {
    bool parasolDeployed;
    bool coolingActive;
    bool rainCollection;
    bool heatAlert;
    bool systemReady;
    bool waterInsufficientAlert;
    unsigned long lastUpdate;
    int operationMode; // 0: 대기, 1: 빗물감지, 2: 더위감지/냉각
} status;

// 임계값 설정 (실제 환경에 맞게 조정 필요)
const float HEAT_THRESHOLD = 28;      // 더위 경고 온도 (°C) - 테스트용 낮춤
const int RAIN_THRESHOLD = 700;       // 빗물 감지 임계값 - 이 값 이하에서 감지
const int UV_THRESHOLD = 600;         // UV 임계값 (0-1023)

// ============= 함수 선언 =============
void initializeSystem();
void initializePins();
void initializeSensors();
void initializeActuators();
void performHardwareTest();
void readAllSensors();
void executeBasicLogic();
void checkRainDetection();
void checkHeatDetection();
void checkWaterLevel();
void deployParasol();
void retractParasol();
void printSystemStatus();

void setup() {
    Serial.begin(9600);
    Serial.println("=== SmartCool Parasol 시스템 초기화 ===");
    Serial.println("PP-A285 플로트 스위치 기반 물 수위 제어");
    Serial.println();

    // 시스템 초기화
    initializeSystem();

    // 핀 모드 설정
    initializePins();

    // 액추에이터 초기화
    initializeActuators();

    // 초기 하드웨어 테스트
    performHardwareTest();

    Serial.println("✓ 시스템 초기화 완료!");
    Serial.println("센서 모니터링을 시작합니다...");
    Serial.println("==========================================");
}

void loop() {

    // 상태 출력 (20초마다)
    if (millis() - status.lastUpdate > 20000) {
        // 센서 데이터 읽기
        readAllSensors();

        // 기본 시스템 로직 실행
        executeBasicLogic();

        printSystemStatus();
        status.lastUpdate = millis();
    }

    delay(500); // 0.5초 대기
}

void initializeSystem() {
    Serial.println("시스템 상태 초기화...");

    // 시스템 상태 초기화
    status.parasolDeployed = false;
    status.coolingActive = false;
    status.rainCollection = false;
    status.heatAlert = false;
    status.systemReady = false;
    status.waterInsufficientAlert = false;
    status.lastUpdate = millis();
    status.operationMode = 0;

    // 센서 데이터 초기화
    sensors.temperature = 0.0;
    sensors.humidity = 0.0;
    sensors.rainLevel = 0;
    sensors.lightLevel = 0;
    sensors.waterLevelOK = false;
    sensors.isValid = false;

    Serial.println("✓ 시스템 상태 초기화 완료");
}

void initializePins() {
    Serial.println("핀 모드 설정...");

    // PP-A285 플로트 스위치 핀 설정 (INPUT_PULLUP 사용)
    pinMode(WATER_LEVEL_PIN, INPUT_PULLUP);

    // 액추에이터 핀 (출력)
    pinMode(WATER_PUMP_PIN, OUTPUT);
    pinMode(MIST_SPRAY_PIN, OUTPUT);

    // 모든 액추에이터 초기 상태 OFF
    digitalWrite(WATER_PUMP_PIN, LOW);
    digitalWrite(MIST_SPRAY_PIN, LOW);

    Serial.println("✓ 핀 설정 완료");
}

void initializeActuators() {
    Serial.println("액추에이터 초기화...");

    // 서보모터 초기화
    parasolServo.attach(SERVO_PIN);
    parasolServo.write(0); // 파라솔 접힌 상태 (0도)
    Serial.println("  - 서보모터: 초기 위치 (0도)");
}

void performHardwareTest() {
    Serial.println("==========================================");
    Serial.println("하드웨어 연결 상태 테스트");
    Serial.println("==========================================");

    // 센서 연결 상태 확인
    Serial.println("1. 센서 연결 상태:");

    // LM35 테스트
    int raw = analogRead(LM35_PIN);
    float vref = 5.0;
    float voltage = raw * (vref / 1023.0);
    float tempC = voltage * 100.0; // LM35: 10mV/°C → 0.01V = 1°C
    Serial.print("  ✓ LM35 온도 센서: ");
    Serial.print(tempC, 1);
    Serial.println("°C");

    // 빗물 센서 테스트
    int rainVal = analogRead(RAIN_SENSOR_PIN);
    Serial.print("  ✓ 빗물 센서 (A0): ");
    Serial.println(rainVal);

    // PP-A285 플로트 스위치 테스트
    bool waterOK = !digitalRead(WATER_LEVEL_PIN); // LOW = 물 충분
    Serial.print("  ✓ PP-A285 플로트 스위치: ");
    Serial.println(waterOK ? "물 충분" : "물 부족");

    // 액추에이터 테스트
    Serial.println("\n2. 액추에이터 테스트:");
    Serial.println("  서보모터 동작 테스트...");
    parasolServo.write(45); // 45도
    delay(1000);
    parasolServo.write(90); // 90도
    delay(1000);
    parasolServo.write(135); // 135도
    delay(1000);
    parasolServo.write(180); // 180도
    delay(1000);
    parasolServo.write(0); // 원위치
    delay(1000);
    Serial.println("  ✓ 서보모터 테스트 완료");

    // 미스트 분사 시스템 간단 테스트
    Serial.println("  미스트 분사 시스템 테스트...");
    digitalWrite(MIST_SPRAY_PIN, HIGH);
    delay(500);
    digitalWrite(MIST_SPRAY_PIN, LOW);
    Serial.println("  ✓ 미스트 분사 테스트 완료");

    status.systemReady = true;
    Serial.println("==========================================");
}

void readAllSensors() {
    // LM35 온도
    int raw = analogRead(LM35_PIN);
    float vref = 5.0; // 3.3V 보드면 3.3으로
    float voltage = raw * (vref / 1023.0);
    sensors.temperature = voltage * 100.0; // °C

    // 습도는 사용 안 함
    sensors.humidity = NAN;

    // 빗물 센서
    sensors.rainLevel = analogRead(RAIN_SENSOR_PIN);
    
    // PP-A285 플로트 스위치 (LOW = 물 충분, HIGH = 물 부족)
    sensors.waterLevelOK = !digitalRead(WATER_LEVEL_PIN);

    // 유효성: LM35는 값이 항상 들어오므로 true
    sensors.isValid = true;
}

void executeBasicLogic() {
    if (!status.systemReady)
        return;

    // Activity 1: 빗물 감지 로직
    checkRainDetection();

    // Activity 2: 더위 감지 로직
    checkHeatDetection();

    // 물 수위 체크
    checkWaterLevel();
}

void checkRainDetection() {
    // 빗물 센서 로직 수정: 값이 낮아지면 빗물 감지
    if (sensors.rainLevel < RAIN_THRESHOLD && !status.rainCollection) {
        // 빗물 감지 시작
        Serial.println("🌧 빗물 감지! 수집 모드 시작");
        status.rainCollection = true;
        status.operationMode = 1;
        status.coolingActive = 0;

        deployParasol();

    } else if (sensors.rainLevel >= RAIN_THRESHOLD && status.rainCollection) {
        // 빗물 감지 종료
        Serial.println("☀ 빗물 종료, 수집 모드 정지");
        status.rainCollection = false;

        if (!status.heatAlert) {
            status.operationMode = 0;
            retractParasol();
        } else {
            status.operationMode = 2;
        }
    }
}

void checkHeatDetection() {
    if (sensors.temperature > HEAT_THRESHOLD && !status.heatAlert) {
        // 더위 경고 시작
        if (status.operationMode == 1) {
            Serial.println("🔥 더위 경고!");
            status.heatAlert = true;
        } else {
            Serial.println("🔥 더위 경고! 냉각 모드 시작");
            status.heatAlert = true;
            if (status.operationMode != 1) {
                status.operationMode = 2;
            }

            deployParasol();

            // PP-A285로 충분한 물이 있는지 확인
            if (sensors.waterLevelOK && status.operationMode == 2) {
                status.coolingActive = true;
                digitalWrite(MIST_SPRAY_PIN, HIGH);
                Serial.println("  💧 미스트 분사 시작");
            } else if (!sensors.waterLevelOK) {
                Serial.println("  ⚠ 물 부족으로 미스트 분사 불가");
            }
        }
    } else if (sensors.temperature <= HEAT_THRESHOLD && status.heatAlert) {
        // 더위 경고 해제
        Serial.println("❄ 온도 정상화, 냉각 모드 정지");
        status.heatAlert = false;
        status.coolingActive = false;

        digitalWrite(MIST_SPRAY_PIN, LOW);

        if (!status.rainCollection) {
            status.operationMode = 0;
            retractParasol();
        }
    } else if (sensors.temperature > HEAT_THRESHOLD && status.heatAlert && status.rainCollection == 0) {
        Serial.println("🔥비가 그침, 냉각 모드 시작");
        status.heatAlert = true;
        status.coolingActive = true;
        if (status.operationMode != 1) {
            status.operationMode = 2;
        }
    }
}

void checkWaterLevel() {
    // PP-A285 기반 물 수위 체크
    if (!sensors.waterLevelOK && !status.waterInsufficientAlert) {
        Serial.println("⚠ 물탱크 수위 부족 경고!");
        status.waterInsufficientAlert = true;
        
        // 냉각 시스템이 동작 중이면 정지
        if (status.coolingActive) {
            Serial.println("  💧 미스트 분사 정지");
            status.coolingActive = false;
            digitalWrite(MIST_SPRAY_PIN, LOW);
        }
    } else if (sensors.waterLevelOK && status.waterInsufficientAlert) {
        Serial.println("✓ 물탱크 수위 회복");
        status.waterInsufficientAlert = false;
        
        // 더위 대응 모드이고 냉각이 필요하면 미스트 재시작
        if (status.heatAlert && status.operationMode == 2 && !status.coolingActive) {
            status.coolingActive = true;
            digitalWrite(MIST_SPRAY_PIN, HIGH);
            Serial.println("  💧 미스트 분사 재시작");
        }
    }
}

void deployParasol() {
    if (status.operationMode != 0) {
        Serial.println("☂ 파라솔 전개 중...");
        if (status.operationMode == 1) {
            parasolServo.write(110); // 빗물 수집용 각도
        } else if (status.operationMode == 2) {
            parasolServo.write(90);  // 차양용 각도
        }
        
        delay(1000); // 서보모터 동작 완료 대기
        status.parasolDeployed = true;
    }
}

void retractParasol() {
    if (status.operationMode == 0) {
        Serial.println("📦 파라솔 수납 중...");
        parasolServo.write(0); // 0도로 수납
        delay(1000);           // 서보모터 동작 완료 대기
        status.parasolDeployed = false;
    }
}

void printSystemStatus() {
    Serial.println("========== 시스템 상태 ==========");

    // 센서 상태
    Serial.print("온도: ");
    Serial.print(sensors.temperature, 1);
    Serial.print("°C | 빗물: ");
    Serial.print(sensors.rainLevel);
    Serial.print(" | 물탱크: ");
    Serial.print(sensors.waterLevelOK ? "충분" : "부족");
    Serial.println();

    Serial.print("파라솔: ");
    Serial.print(status.parasolDeployed ? "전개" : "수납");
    Serial.print(" | 냉각: ");
    Serial.print(status.coolingActive ? "ON" : "OFF");
    Serial.print(" | 물부족경고: ");
    Serial.println(status.waterInsufficientAlert ? "ON" : "OFF");

    // 동작 모드
    Serial.print("모드: ");
    switch (status.operationMode) {
    case 0:
        Serial.println("대기");
        break;
    case 1:
        Serial.println("빗물수집");
        break;
    case 2:
        Serial.println("더위대응");
        break;
    default:
        Serial.println("알 수 없음");
        break;
    }

    // PP-A285 세부 정보
    Serial.print("PP-A285 상태: ");
    bool currentSwitchState = digitalRead(WATER_LEVEL_PIN);
    Serial.print("Raw=");
    Serial.print(currentSwitchState ? "HIGH" : "LOW");
    Serial.print(" → ");
    Serial.println(sensors.waterLevelOK ? "물 충분" : "물 부족");

    // 센서 상태
    Serial.print("센서 상태: ");
    Serial.println(sensors.isValid ? "정상" : "오류");

    Serial.println("================================");
}