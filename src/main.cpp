

/*
 * SmartCool Parasol 시스템 - 초기 하드웨어 테스트
 * 센서 레이어, 제어 레이어, 액추에이터 레이어 기본 구현
 * WiFi 및 웹 기능 제외 - 하드웨어 검증 우선
 * PlatformIO 환경용
 */

#include <Arduino.h>
#include <DHT.h>
#include <Servo.h>

// ============= 핀 정의 =============
// 센서 레이어 (Sensor Layer)
#define DHT_PIN 2          // 온습도 센서 (디지털)
#define DHT_TYPE DHT22
#define RAIN_SENSOR_PIN A0 // 빗물 감지 센서 (아날로그)
#define UV_SENSOR_PIN A1   // UV 센서 (아날로그)  
#define LIGHT_SENSOR_PIN A2 // 조도 센서 (아날로그)
#define WATER_LEVEL_PIN A3 // 물탱크 수위 센서 (아날로그)

// 액추에이터 레이어 (Actuator Layer)
#define SERVO_PIN 9        // 파라솔 구동 서보모터
#define LED_PIN 13         // 상태 표시 LED (내장 LED 사용)
#define BUZZER_PIN 10      // 부저
#define WATER_PUMP_PIN 6   // 물 분사 펌프
#define MIST_SPRAY_PIN 5   // 미스트 분사

// 인프라 레이어 (Infrastructure Layer)  
#define DRAIN_VALVE_PIN 4  // 배수 밸브

// ============= 객체 초기화 =============
DHT dht(DHT_PIN, DHT_TYPE);
Servo parasolServo;

// ============= 전역 변수 =============
// 센서 값 저장 구조체
struct SensorData {
  float temperature;
  float humidity;
  int rainLevel;
  int uvLevel;
  int lightLevel;
  int waterLevel;
  bool isValid;
} sensors;

// 시스템 상태 구조체
struct SystemStatus {
  bool parasolDeployed;
  bool coolingActive;
  bool rainCollection;
  bool heatAlert;
  bool systemReady;
  unsigned long lastUpdate;
  int operationMode; // 0: 대기, 1: 빗물감지, 2: 더위감지/냉각
} status;

// 임계값 설정 (실제 환경에 맞게 조정 필요)
const float HEAT_THRESHOLD = 20.0;    // 더위 경고 온도 (°C) - 테스트용 낮춤
const int RAIN_THRESHOLD = 700;       // 빗물 감지 임계값 - 이 값 이하에서 감지
const int UV_THRESHOLD = 600;         // UV 임계값 (0-1023)
const int WATER_LOW_THRESHOLD = 100;  // 물탱크 최저 수위 (0-1023)
const int WATER_FULL_THRESHOLD = 800; // 물탱크 만수위 (0-1023)

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
void manageWaterTank();
void deployParasol();
void retractParasol();
void printSystemStatus();

void setup() {
  Serial.begin(9600);
  Serial.println("=== SmartCool Parasol 시스템 초기화 ===");
  Serial.println("하드웨어 테스트 모드");
  Serial.println();
  
  // 시스템 초기화
  initializeSystem();
  
  // 핀 모드 설정
  initializePins();
  
  // 센서 초기화
  initializeSensors();
  
  // 액추에이터 초기화  
  initializeActuators();
  
  // 초기 하드웨어 테스트
  performHardwareTest();
  
  Serial.println("✓ 시스템 초기화 완료!");
  Serial.println("센서 모니터링을 시작합니다...");
  Serial.println("==========================================");
}


void loop() { 
  // 센서 데이터 읽기
  readAllSensors();
  
  // 기본 시스템 로직 실행
  executeBasicLogic();
  
  // 상태 출력 (3초마다)
  if (millis() - status.lastUpdate > 3000) {
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
  status.lastUpdate = millis();
  status.operationMode = 0;
  
  // 센서 데이터 초기화
  sensors.temperature = 0.0;
  sensors.humidity = 0.0;
  sensors.rainLevel = 0;
  sensors.uvLevel = 0;
  sensors.lightLevel = 0;
  sensors.waterLevel = 0;
  sensors.isValid = false;
  
  Serial.println("✓ 시스템 상태 초기화 완료");
}

void initializePins() {
  Serial.println("핀 모드 설정...");
  
  // DHT_PIN은 DHT 라이브러리가 자동으로 관리하므로 pinMode 설정 안함
  
  // 액추에이터 핀 (출력)
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(MIST_SPRAY_PIN, OUTPUT);
  pinMode(DRAIN_VALVE_PIN, OUTPUT);
  
  // 모든 액추에이터 초기 상태 OFF
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(WATER_PUMP_PIN, LOW);
  digitalWrite(MIST_SPRAY_PIN, LOW);
  digitalWrite(DRAIN_VALVE_PIN, LOW);
  
  Serial.println("✓ 핀 설정 완료");
}

void initializeSensors() {
  Serial.println("센서 초기화...");
  
  // DHT22 온습도 센서 시작
  dht.begin();
  Serial.println("  - DHT22 온습도 센서 초기화");
  
  // 센서 안정화 대기 제거 - 바로 넘어감
  // delay(2000); // 이 줄 제거
  
  // 초기 센서 값 읽기 테스트 제거 - loop에서 수행
  // 첫 읽기는 loop()에서 할 예정
  sensors.isValid = false; // 일단 무효로 설정
  
  Serial.println("✓ 센서 초기화 완료");
}

void initializeActuators() {
  Serial.println("액추에이터 초기화...");
  
  // 서보모터 초기화
  parasolServo.attach(SERVO_PIN);
  parasolServo.write(0); // 파라솔 접힌 상태 (0도)
  Serial.println("  - 서보모터: 초기 위치 (0도)");
  
  // LED 테스트 (깜빡임)
  Serial.println("  - LED 테스트...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  
  // 부저 테스트 (짧은 삐소리)
  Serial.println("  - 부저 테스트...");
  tone(BUZZER_PIN, 1000, 300);
  delay(500);
  
  Serial.println("✓ 액추에이터 초기화 완료");
}

void performHardwareTest() {
  Serial.println("==========================================");
  Serial.println("하드웨어 연결 상태 테스트");
  Serial.println("==========================================");
  
  // 센서 연결 상태 확인
  Serial.println("1. 센서 연결 상태:");
  
  // DHT22 테스트
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (!isnan(temp) && !isnan(hum)) {
    Serial.println("  ✓ DHT22 온습도 센서: 정상");
    Serial.print("    현재 온도: ");
    Serial.print(temp, 1);
    Serial.print("°C, 습도: ");
    Serial.print(hum, 1);
    Serial.println("%");
  } else {
    Serial.println("  ✗ DHT22 온습도 센서: 오류");
  }
  
  // 아날로그 센서들 테스트
  int rainVal = analogRead(RAIN_SENSOR_PIN);
  int uvVal = analogRead(UV_SENSOR_PIN);
  int lightVal = analogRead(LIGHT_SENSOR_PIN);
  int waterVal = analogRead(WATER_LEVEL_PIN);
  
  Serial.print("  빗물 센서 (A0): ");
  Serial.println(rainVal);
  Serial.print("  UV 센서 (A1): ");
  Serial.println(uvVal);
  Serial.print("  조도 센서 (A2): ");
  Serial.println(lightVal);
  Serial.print("  수위 센서 (A3): ");
  Serial.println(waterVal);
  
  // 액추에이터 테스트
  Serial.println("\n2. 액추에이터 테스트:");
  Serial.println("  서보모터 동작 테스트...");
  parasolServo.write(45);  // 45도
  delay(1000);
  parasolServo.write(90);  // 90도
  delay(1000);
  parasolServo.write(0);   // 원위치
  delay(1000);
  Serial.println("  ✓ 서보모터 테스트 완료");
  
  status.systemReady = true;
  Serial.println("==========================================");
}

void readAllSensors() {
  // DHT22 온습도 센서
  sensors.temperature = dht.readTemperature();
  sensors.humidity = dht.readHumidity();
  
  // 아날로그 센서들
  sensors.rainLevel = analogRead(RAIN_SENSOR_PIN);
  sensors.uvLevel = analogRead(UV_SENSOR_PIN);
  sensors.lightLevel = analogRead(LIGHT_SENSOR_PIN);
  sensors.waterLevel = analogRead(WATER_LEVEL_PIN);
  
  // 센서 유효성 검사
  if (isnan(sensors.temperature) || isnan(sensors.humidity)) {
    sensors.isValid = false;
    // 기본값 설정
    sensors.temperature = 25.0;
    sensors.humidity = 50.0;
  } else {
    sensors.isValid = true;
  }
}

void executeBasicLogic() {
  if (!status.systemReady) return;
  
  // Activity 1: 빗물 감지 로직
  checkRainDetection();
  
  // Activity 2: 더위 감지 로직  
  checkHeatDetection();
  
  // 물탱크 관리
  manageWaterTank();
}

void checkRainDetection() {
  // 빗물 센서 로직 수정: 값이 낮아지면 빗물 감지
  if (sensors.rainLevel < RAIN_THRESHOLD && !status.rainCollection) {
    // 빗물 감지 시작
    Serial.println("🌧 빗물 감지! 수집 모드 시작");
    status.rainCollection = true;
    status.operationMode = 1;
    
    deployParasol();
    digitalWrite(LED_PIN, HIGH); // LED 켜기
    
  } else if (sensors.rainLevel >= RAIN_THRESHOLD && status.rainCollection) {
    // 빗물 감지 종료
    Serial.println("☀ 빗물 종료, 수집 모드 정지");
    status.rainCollection = false;
    
    if (!status.heatAlert) {
      retractParasol();
      digitalWrite(LED_PIN, LOW);
      status.operationMode = 0;
    }
  }
}

void checkHeatDetection() {
  if (sensors.temperature > HEAT_THRESHOLD && !status.heatAlert) {
    // 더위 경고 시작
    Serial.println("🔥 더위 경고! 냉각 모드 시작");
    status.heatAlert = true;
    status.operationMode = 2;
    
    deployParasol();
    
    // 충분한 물이 있을 때만 냉각 시스템 가동
    if (sensors.waterLevel > WATER_LOW_THRESHOLD) {
      status.coolingActive = true;
      digitalWrite(MIST_SPRAY_PIN, HIGH);
      Serial.println("  💧 미스트 분사 시작");
    }
    
    // 경고음
    tone(BUZZER_PIN, 1000, 500);
    
  } else if (sensors.temperature <= HEAT_THRESHOLD - 2 && status.heatAlert) {
    // 더위 경고 해제
    Serial.println("❄ 온도 정상화, 냉각 모드 정지");
    status.heatAlert = false;
    status.coolingActive = false;
    
    digitalWrite(MIST_SPRAY_PIN, LOW);
    
    if (!status.rainCollection) {
      retractParasol();
      status.operationMode = 0;
    }
  }
}

void manageWaterTank() {
  // 물탱크 만수위 체크
  if (sensors.waterLevel > WATER_FULL_THRESHOLD) {
    digitalWrite(DRAIN_VALVE_PIN, HIGH); // 배수 밸브 열기
  } else if (sensors.waterLevel < WATER_FULL_THRESHOLD - 50) {
    digitalWrite(DRAIN_VALVE_PIN, LOW);  // 배수 밸브 닫기
  }
  
  // 물 부족 시 냉각 시스템 정지
  if (sensors.waterLevel < WATER_LOW_THRESHOLD && status.coolingActive) {
    Serial.println("⚠ 물탱크 수위 부족! 냉각 시스템 정지");
    status.coolingActive = false;
    digitalWrite(MIST_SPRAY_PIN, LOW);
    tone(BUZZER_PIN, 500, 1000); // 경고음
  }
}

void deployParasol() {
  if (!status.parasolDeployed) {
    Serial.println("☂ 파라솔 전개 중...");
    parasolServo.write(90); // 90도로 전개
    status.parasolDeployed = true;
    delay(1000); // 서보모터 동작 완료 대기
  }
}

void retractParasol() {
  if (status.parasolDeployed) {
    Serial.println("📦 파라솔 수납 중...");
    parasolServo.write(0); // 0도로 수납
    status.parasolDeployed = false;
    delay(1000); // 서보모터 동작 완료 대기
  }
}

void printSystemStatus() {
  Serial.println("========== 시스템 상태 ==========");
  
  // 센서 상태
  Serial.print("온도: ");
  Serial.print(sensors.temperature, 1);
  Serial.print("°C | 습도: ");
  Serial.print(sensors.humidity, 1);
  Serial.println("%");
  
  Serial.print("빗물: ");
  Serial.print(sensors.rainLevel);
  Serial.print(" | UV: ");
  Serial.print(sensors.uvLevel);
  Serial.print(" | 조도: ");
  Serial.println(sensors.lightLevel);
  
  Serial.print("물탱크: ");
  Serial.print(sensors.waterLevel);
  Serial.print(" | 파라솔: ");
  Serial.print(status.parasolDeployed ? "전개" : "수납");
  Serial.print(" | 냉각: ");
  Serial.println(status.coolingActive ? "ON" : "OFF");
  
  // 동작 모드
  Serial.print("모드: ");
  switch(status.operationMode) {
    case 0: Serial.println("대기"); break;
    case 1: Serial.println("빗물수집"); break;
    case 2: Serial.println("더위대응"); break;
    default: Serial.println("알 수 없음"); break;
  }
  
  // 센서 상태
  Serial.print("센서 상태: ");
  Serial.println(sensors.isValid ? "정상" : "오류");
  
  Serial.println("================================");
}