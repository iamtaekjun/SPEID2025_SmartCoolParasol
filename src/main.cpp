/*
 * SmartCool Parasol 시스템 - 아날로그 수위센서 적용
 * 수위 임계값 600 기반 워터펌프 제어
 * 시연용 버전 (캘리브레이션 기능 제거)
 * PlatformIO 환경용
 */

#include <Arduino.h>
#include <Servo.h>

// ============= 핀 정의 =============
// 센서 레이어 (Sensor Layer)
#define LM35_PIN A4         // LM35 출력 Vout 연결 핀(아날로그)
#define RAIN_SENSOR_PIN A0  // 빗물 감지 센서 (아날로그)
#define WATER_LEVEL_PIN A3  // 물탱크 수위 센서 (아날로그)

// 액추에이터 레이어 (Actuator Layer)
#define SERVO_PIN 9         // 파라솔 구동 서보모터
#define WATER_PUMP_IN1 6    // 워터펌프 IN1 (미스트 분사용)
#define WATER_PUMP_IN2 7    // 워터펌프 IN2 (미스트 분사용)

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
    int waterLevelRaw;        // 원시 아날로그 값 (0-1023)
    float waterLevelPercent;  // 백분율 (0-100%)
    bool waterLevelOK;        // 임계값 이상 여부
    bool isValid;
} sensors;

// 시스템 상태 구조체
struct SystemStatus {
    bool parasolDeployed;
    bool pumpActive;          // 워터펌프 동작 상태 (미스트 분사)
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
const int WATER_THRESHOLD = 600;      // 워터펌프 구동 최소 수위 임계값

// 수위 센서 기본 설정값 (시연용 고정값)
const int WATER_EMPTY_VALUE = 100;    // 물 없을 때 예상 센서값
const int WATER_FULL_VALUE = 900;     // 만수위일 때 예상 센서값

// ============= 함수 선언 =============
void initializeSystem();
void initializePins();
void initializeSensors();
void initializeActuators();
void performHardwareTest();
int readWaterLevelRaw();
float calculateWaterPercent(int rawValue);
void readAllSensors();
void executeBasicLogic();
void checkRainDetection();
void checkHeatDetection();
void checkWaterLevel();
void startWaterPump();
void stopWaterPump();
void deployParasol();
void retractParasol();
void printSystemStatus();

void setup() {
    Serial.begin(9600);
    Serial.println("=== SmartCool Parasol 시스템 초기화 ===");
    Serial.println("워터펌프 기반 미스트 분사 - 시연용 버전");
    Serial.println("수위 임계값: 600");
    Serial.println();

    // 시스템 초기화
    initializeSystem();

    // 핀 모드 설정
    initializePins();

    // 액추에이터 초기화
    initializeActuators();

    // 초기 하드웨어 테스트
    performHardwareTest();

    Serial.println("시스템 초기화 완료!");
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
    status.pumpActive = false;
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
    sensors.waterLevelRaw = 0;
    sensors.waterLevelPercent = 0.0;
    sensors.waterLevelOK = false;
    sensors.isValid = false;

    Serial.println("시스템 상태 초기화 완료");
}

void initializePins() {
    Serial.println("핀 모드 설정...");

    // 아날로그 핀은 별도 pinMode 설정 불필요

    // 워터펌프 핀 (출력)
    pinMode(WATER_PUMP_IN1, OUTPUT);
    pinMode(WATER_PUMP_IN2, OUTPUT);

    // 워터펌프 초기 상태 OFF
    digitalWrite(WATER_PUMP_IN1, LOW);
    digitalWrite(WATER_PUMP_IN2, LOW);

    Serial.println("핀 설정 완료");
}

void initializeActuators() {
    Serial.println("액추에이터 초기화...");

    // 서보모터 초기화
    parasolServo.attach(SERVO_PIN);
    parasolServo.write(0); // 파라솔 접힌 상태 (0도)
    Serial.println("  - 서보모터: 초기 위치 (0도)");
    
    // 워터펌프 초기화
    stopWaterPump();
    Serial.println("  - 워터펌프: 정지 상태");
}

void startWaterPump() {
    // 워터펌프 구동 (미스트 분사)
    digitalWrite(WATER_PUMP_IN1, HIGH);
    digitalWrite(WATER_PUMP_IN2, LOW);
    status.pumpActive = true;
}

void stopWaterPump() {
    // 워터펌프 정지
    digitalWrite(WATER_PUMP_IN1, LOW);
    digitalWrite(WATER_PUMP_IN2, LOW);
    status.pumpActive = false;
}

int readWaterLevelRaw() {
    // 안정적인 읽기를 위해 여러 번 측정 후 평균
    long sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += analogRead(WATER_LEVEL_PIN);
        delay(10);
    }
    return sum / 5;
}

float calculateWaterPercent(int rawValue) {
    // 고정된 기본값을 이용한 백분율 계산 (시연용)
    if (rawValue <= WATER_EMPTY_VALUE) {
        return 0.0;
    } else if (rawValue >= WATER_FULL_VALUE) {
        return 100.0;
    } else {
        float range = WATER_FULL_VALUE - WATER_EMPTY_VALUE;
        float position = rawValue - WATER_EMPTY_VALUE;
        return (position / range) * 100.0;
    }
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
    float tempC = voltage * 100.0;
    Serial.print("LM35 온도 센서: ");
    Serial.print(tempC, 1);
    Serial.println("°C");

    // 빗물 센서 테스트
    int rainVal = analogRead(RAIN_SENSOR_PIN);
    Serial.print("빗물 센서 (A0): ");
    Serial.println(rainVal);

    // 수위 센서 테스트
    int waterRaw = readWaterLevelRaw();
    float waterPercent = calculateWaterPercent(waterRaw);
    Serial.print("수위 센서 (A3): Raw=");
    Serial.print(waterRaw);
    Serial.print(", Percent=");
    Serial.print(waterPercent, 1);
    Serial.print("%, 임계값=");
    Serial.println(waterRaw >= WATER_THRESHOLD ? "충족" : "부족");

    // 액추에이터 테스트
    Serial.println("\n2. 액추에이터 테스트:");
    Serial.println("  서보모터 동작 테스트...");
    parasolServo.write(45);
    delay(1000);
    parasolServo.write(90);
    delay(1000);
    parasolServo.write(135);
    delay(1000);
    parasolServo.write(180);
    delay(1000);
    parasolServo.write(0);
    delay(1000);
    Serial.println("서보모터 테스트 완료");

    // 워터펌프 테스트
    Serial.println("  워터펌프 동작 테스트...");
    startWaterPump();
    delay(1000);
    stopWaterPump();
    Serial.println("워터펌프 테스트 완료");

    status.systemReady = true;
    Serial.println("==========================================");
}

void readAllSensors() {
    // LM35 온도
    int raw = analogRead(LM35_PIN);
    float vref = 5.0;
    float voltage = raw * (vref / 1023.0);
    sensors.temperature = voltage * 100.0;

    // 습도는 사용 안 함
    sensors.humidity = NAN;

    // 빗물 센서
    sensors.rainLevel = analogRead(RAIN_SENSOR_PIN);
    
    // 수위 센서 (아날로그)
    sensors.waterLevelRaw = readWaterLevelRaw();
    sensors.waterLevelPercent = calculateWaterPercent(sensors.waterLevelRaw);
    sensors.waterLevelOK = (sensors.waterLevelRaw >= WATER_THRESHOLD);

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
    if (sensors.rainLevel < RAIN_THRESHOLD && !status.rainCollection) {
        Serial.println("빗물 감지! 수집 모드 시작");
        status.rainCollection = true;
        status.operationMode = 1;
        
        // 빗물 수집 중에는 워터펌프 정지
        if (status.pumpActive) {
            stopWaterPump();
            Serial.println("빗물 수집을 위해 워터펌프 정지");
        }

        deployParasol();

    } else if (sensors.rainLevel >= RAIN_THRESHOLD && status.rainCollection) {
        Serial.println("빗물 종료, 수집 모드 정지");
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
        if (status.operationMode == 1) {
            Serial.println("더위 경고!");
            status.heatAlert = true;
        } else {
            Serial.println("더위 경고! 냉각 모드 시작");
            status.heatAlert = true;
            if (status.operationMode != 1) {
                status.operationMode = 2;
            }

            deployParasol();

            // 수위 임계값 확인 후 워터펌프 구동
            if (sensors.waterLevelOK && status.operationMode == 2) {
                startWaterPump();
                Serial.print("워터펌프 시작 - 미스트 분사 (수위: ");
                Serial.print(sensors.waterLevelPercent, 1);
                Serial.println("%)");
            } else if (!sensors.waterLevelOK) {
                Serial.print("수위 부족으로 워터펌프 구동 불가 (현재: ");
                Serial.print(sensors.waterLevelRaw);
                Serial.print(" < 임계값: ");
                Serial.print(WATER_THRESHOLD);
                Serial.println(")");
            }
        }
    } else if (sensors.temperature <= HEAT_THRESHOLD && status.heatAlert) {
        Serial.println("온도 정상화, 냉각 모드 정지");
        status.heatAlert = false;
        
        if (status.pumpActive) {
            stopWaterPump();
            Serial.println("워터펌프 정지");
        }

        if (!status.rainCollection) {
            status.operationMode = 0;
            retractParasol();
        }
    }
}

void checkWaterLevel() {
    if (!sensors.waterLevelOK && !status.waterInsufficientAlert) {
        Serial.print("물탱크 수위 부족 경고! (현재: ");
        Serial.print(sensors.waterLevelRaw);
        Serial.print(" < 임계값: ");
        Serial.print(WATER_THRESHOLD);
        Serial.println(")");
        status.waterInsufficientAlert = true;
        
        if (status.pumpActive) {
            stopWaterPump();
            Serial.println("워터펌프 정지");
        }
    } else if (sensors.waterLevelOK && status.waterInsufficientAlert) {
        Serial.print("물탱크 수위 회복 (현재: ");
        Serial.print(sensors.waterLevelRaw);
        Serial.print(" >= 임계값: ");
        Serial.print(WATER_THRESHOLD);
        Serial.println(")");
        status.waterInsufficientAlert = false;
        
        if (status.heatAlert && status.operationMode == 2 && !status.pumpActive) {
            startWaterPump();
            Serial.println("워터펌프 재시작");
        }
    }
}

void deployParasol() {
    if (status.operationMode != 0) {
        Serial.println("파라솔 전개 중...");
        if (status.operationMode == 1) {
            parasolServo.write(110); // 빗물 수집용 각도
        } else if (status.operationMode == 2) {
            parasolServo.write(90);  // 차양용 각도
        }
        
        delay(1000);
        status.parasolDeployed = true;
    }
}

void retractParasol() {
    if (status.operationMode == 0) {
        Serial.println("파라솔 수납 중...");
        parasolServo.write(0);
        delay(1000);
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
    Serial.print(" | 수위: ");
    Serial.print(sensors.waterLevelPercent, 1);
    Serial.print("% (");
    Serial.print(sensors.waterLevelRaw);
    Serial.println(")");

    Serial.print("파라솔: ");
    Serial.print(status.parasolDeployed ? "전개" : "수납");
    Serial.print(" | 워터펌프: ");
    Serial.print(status.pumpActive ? "ON" : "OFF");
    Serial.print(" | 수위임계값: ");
    Serial.print(sensors.waterLevelOK ? "충족" : "부족");
    Serial.print(" (기준: ");
    Serial.print(WATER_THRESHOLD);
    Serial.println(")");

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

    // 워터펌프 세부 상태
    if (status.pumpActive) {
        Serial.print("워터펌프 상태: IN1=HIGH, IN2=LOW (미스트 분사 중)");
    } else {
        Serial.print("워터펌프 상태: IN1=LOW, IN2=LOW (정지)");
    }
    Serial.println();

    // 센서 상태
    Serial.print("센서 상태: ");
    Serial.println(sensors.isValid ? "정상" : "오류");

    Serial.println("================================");
}