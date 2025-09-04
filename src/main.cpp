/*
 * SmartCool Parasol - 포텐셔미터 온도 시뮬레이션
 * 0-40도C 범위, 28도C 임계값
 */

#include <Arduino.h>
#include <Servo.h>

// 핀 정의
#define TEMP_POTENTIOMETER_PIN A1
#define RAIN_SENSOR_PIN A0
#define WATER_LEVEL_PIN A3
#define SERVO_PIN 9
#define RELAY_PIN 6

// 객체 초기화
Servo parasolServo;

// 전역 변수
struct SensorData {
    float temperature;
    int rainLevel;
    int waterLevelRaw;
    float waterLevelPercent;
    bool waterLevelOK;
    bool isValid;
} sensors;

struct SystemStatus {
    bool parasolDeployed;
    bool pumpActive;
    bool systemReady;
    unsigned long lastUpdate;
    int operationMode; // 0: 대기, 1: 비모드, 2: 더위모드
} status;

// 상태 변수 (내부 로직용)
bool rainDetected = false;  // 현재 비가 오는지
bool heatDetected = false;  // 현재 더위인지

// 임계값 설정
const float HEAT_THRESHOLD = 28.0;
const int RAIN_THRESHOLD = 500
;
const int WATER_THRESHOLD = 600;

// 수위 센서 기본 설정값
const int WATER_EMPTY_VALUE = 100;
const int WATER_FULL_VALUE = 900;

// 함수 선언
void initializeSystem();
void initializePins();
void initializeActuators();
void performHardwareTest();
int readWaterLevelRaw();
float calculateWaterPercent(int rawValue);
void readAllSensors();
void updateSystemMode();
void controlParasol();
void controlWaterPump();
void printSystemStatus();
void relayON();
void relayOFF();

void setup() {
    Serial.begin(9600);
    Serial.println(F("=== SmartCool Parasol ==="));
    Serial.println(F("포텐셔미터 온도: 0-40도C (임계: 28도C)"));
    Serial.println();

    initializeSystem();
    initializePins();
    initializeActuators();
    performHardwareTest();

    Serial.println(F("시스템 준비 완료!"));
    Serial.println(F("=========================================="));
}

void loop() {
    // 상태 출력 (10초마다)
    if (millis() - status.lastUpdate > 10000) {
        readAllSensors();
        updateSystemMode();
        controlParasol();
        controlWaterPump();
        printSystemStatus();
        status.lastUpdate = millis();
    }
    delay(500);
}

void initializeSystem() {
    Serial.println(F("시스템 초기화..."));

    status.parasolDeployed = false;
    status.pumpActive = false;
    status.systemReady = false;
    status.lastUpdate = millis();
    status.operationMode = 0;

    sensors.temperature = 0.0;
    sensors.rainLevel = 0;
    sensors.waterLevelRaw = 0;
    sensors.waterLevelPercent = 0.0;
    sensors.waterLevelOK = false;
    sensors.isValid = false;

    rainDetected = false;
    heatDetected = false;

    Serial.println(F("초기화 완료"));
}

void initializePins() {
    pinMode(RELAY_PIN, OUTPUT);
    relayOFF();
}

void initializeActuators() {
    parasolServo.attach(SERVO_PIN);
    parasolServo.write(30);
    relayOFF();
}

void relayOFF() {
    digitalWrite(RELAY_PIN, LOW);
    status.pumpActive = false;
}

void relayON() {
    digitalWrite(RELAY_PIN, HIGH);
    status.pumpActive = true;
}

int readWaterLevelRaw() {
    long sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += analogRead(WATER_LEVEL_PIN);
        delay(10);
    }
    return sum / 5;
}

float calculateWaterPercent(int rawValue) {
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
    Serial.println(F("===== 하드웨어 테스트 ====="));

    // 포텐셔미터 테스트
    int tempRaw = analogRead(TEMP_POTENTIOMETER_PIN);
    float tempC = (tempRaw / 1023.0) * 40.0;
    Serial.print(F("온도: "));
    Serial.print(tempC, 1);
    Serial.print(F("°C"));
    Serial.println(tempC > 28.0 ? F(" [더위!]") : F(" [정상]"));

    // 수위 테스트
    int waterRaw = readWaterLevelRaw();
    Serial.print(F("수위: "));
    Serial.print(waterRaw);
    Serial.println(waterRaw >= WATER_THRESHOLD ? F(" [충분]") : F(" [부족]"));

    // 서보 테스트
    Serial.println(F("서보 테스트..."));
    parasolServo.write(30);
    delay(1000);
    parasolServo.write(80);
    delay(1000);
    parasolServo.write(130);
    delay(1000);
    parasolServo.write(80);
    delay(1000);
    parasolServo.write(30);
    delay(1000);

    // 릴레이 테스트
    Serial.println(F("릴레이 테스트..."));
    relayON();
    delay(500);
    relayOFF();

    status.systemReady = true;
    Serial.println(F("============================"));
}

void readAllSensors() {
    // 포텐셔미터로 온도 시뮬레이션
    long sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += analogRead(TEMP_POTENTIOMETER_PIN);
        delay(500);
    }
    int raw = sum / 5;
    sensors.temperature = (raw / 1023.0) * 40.0;
    
    // 빗물 센서
    sensors.rainLevel = analogRead(RAIN_SENSOR_PIN);
    
    // 수위 센서
    sensors.waterLevelRaw = readWaterLevelRaw();
    sensors.waterLevelPercent = calculateWaterPercent(sensors.waterLevelRaw);
    sensors.waterLevelOK = (sensors.waterLevelRaw >= WATER_THRESHOLD);
    
    sensors.isValid = true;
}

void updateSystemMode() {
    if (!status.systemReady) return;

    // 비와 더위 감지 상태 업데이트
    rainDetected = (sensors.rainLevel < RAIN_THRESHOLD);
    heatDetected = (sensors.temperature > HEAT_THRESHOLD);

    int newMode = status.operationMode;

    // 모드 결정 로직
    if (rainDetected && heatDetected) {
        newMode = 1; // 비모드 우선
    } else if (rainDetected && !heatDetected) {
        newMode = 1; // 비모드
    } else if (!rainDetected && heatDetected) {
        newMode = 2; // 더위모드
    } else {
        newMode = 0; // 대기모드
    }

    // 모드 변경 시 메시지 출력
    if (newMode != status.operationMode) {
        status.operationMode = newMode;
        switch (newMode) {
        case 0: Serial.println(F("대기 모드")); break;
        case 1: Serial.println(F("비 모드")); break;
        case 2: Serial.println(F("더위 모드")); break;
        }
    }

    delay(3000);
}

void controlParasol() {
    switch (status.operationMode) {
    case 0: // 대기 모드 - 수납
        if (status.parasolDeployed) {
            parasolServo.write(30);
            delay(1000);
            status.parasolDeployed = false;
        }
        break;

    case 1: // 비 모드 - 빗물 수집 각도
        parasolServo.write(130);
        delay(1000);
        status.parasolDeployed = true;
        break;

    case 2: // 더위 모드 - 차양 각도
        parasolServo.write(80);
        delay(1000);
        status.parasolDeployed = true;
        break;
    }
}

void controlWaterPump() {
    bool shouldPumpRun = (status.operationMode == 2) && sensors.waterLevelOK;

    if (shouldPumpRun && !status.pumpActive) {
        relayON();
        Serial.println(F("미스트 분사 시작"));
    } else if (!shouldPumpRun && status.pumpActive) {
        relayOFF();
        if (status.operationMode == 2 && !sensors.waterLevelOK) {
            Serial.println(F("수위 부족 - 펌프 정지"));
        }
    }
}

void printSystemStatus() {
    Serial.println(F("===== 시스템 상태 ====="));

    Serial.print(F("온도: "));
    Serial.print(sensors.temperature, 1);
    Serial.print(F("°C "));
    
    Serial.print(F("비: "));
    Serial.print(sensors.rainLevel, 1);
    Serial.print(F("%"));
    Serial.print(heatDetected ? F("[더위감지]") : F("[정상]"));
    Serial.print(F(" | 비: "));
    Serial.print(rainDetected ? F("[감지]") : F("[없음]"));
    Serial.print(F(" | 수위: "));
    Serial.print(sensors.waterLevelPercent, 1);
    Serial.println(sensors.waterLevelOK ? F("% [충분]") : F("% [부족]"));

    Serial.print(F("파라솔: "));
    Serial.print(status.parasolDeployed ? F("전개") : F("수납"));
    Serial.print(F(" | 펌프: "));
    Serial.print(status.pumpActive ? F("ON") : F("OFF"));
    Serial.print(F(" | 모드: "));
    switch (status.operationMode) {
    case 0: Serial.println(F("대기")); break;
    case 1: Serial.println(F("비")); break;
    case 2: Serial.println(F("더위")); break;
    }

    Serial.println(F("=========================="));
}
