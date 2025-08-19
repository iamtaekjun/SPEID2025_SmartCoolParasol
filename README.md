# SmartCool Parasol - PlatformIO 프로젝트

## 📁 프로젝트 구조
```
SmartCool_Parasol/
├── platformio.ini          # PlatformIO 설정 파일
├── src/
│   └── main.cpp            # 메인 소스 코드
├── lib/                    # 사용자 정의 라이브러리
├── include/                # 헤더 파일
├── README.md               # 이 파일
├── wiring_guide.md         # 배선 가이드
└── docs/                   # 문서 폴더
```

## 🛠️ PlatformIO 설정 및 사용법

### 1. PlatformIO 설치
```bash
# VS Code에서 Extensions 설치
1. VS Code 실행
2. Extensions (Ctrl+Shift+X)
3. "PlatformIO IDE" 검색 후 설치
4. VS Code 재시작
```

### 2. 프로젝트 열기
```bash
# 방법 1: PlatformIO Home에서
1. PlatformIO 아이콘 클릭
2. "Open Project" 선택
3. SmartCool_Parasol 폴더 선택

# 방법 2: VS Code에서 직접
1. File → Open Folder
2. SmartCool_Parasol 폴더 선택
```

### 3. 필요한 라이브러리 자동 설치
```ini
# platformio.ini에 이미 설정됨
lib_deps = 
    adafruit/DHT sensor library@^1.4.4
    arduino-libraries/Servo@^1.2.1
```

## 🚀 빌드 및 업로드

### PlatformIO 단축키
```bash
# 빌드 (컴파일)
Ctrl+Alt+B

# 업로드 (아두이노에 전송)
Ctrl+Alt+U

# 시리얼 모니터 열기
Ctrl+Alt+S

# 클린 빌드
Ctrl+Alt+C
```

### PlatformIO 터미널 명령어
```bash
# 프로젝트 빌드
pio run

# 업로드
pio run --target upload

# 시리얼 모니터
pio device monitor

# 라이브러리 업데이트
pio lib update
```

## 📋 하드웨어 연결 (Arduino Uno)

### 센서 연결
```
DHT22 온습도    → Pin 2 (Digital)
빗물 센서       → A0 (Analog)
UV 센서        → A1 (Analog)
조도 센서       → A2 (Analog)
수위 센서       → A3 (Analog)
```

### 액추에이터 연결
```
서보모터       → Pin 9 (PWM)
LED           → Pin 13 (내장 LED)
부저          → Pin 8 (Digital)
워터펌프 릴레이  → Pin 7 (Digital)
미스트 릴레이   → Pin 6 (Digital)
배수밸브 릴레이  → Pin 5 (Digital)
```

## 🔧 개발 환경 설정

### platformio.ini 주요 설정
```ini
[env:uno]
platform = atmelavr        # AVR 플랫폼
board = uno                # Arduino Uno
framework = arduino        # Arduino 프레임워크
monitor_speed = 9600       # 시리얼 통신 속도
```

### 컴파일러 최적화
```ini
build_flags = 
    -O2                    # 최적화 레벨 2
    -DPLATFORMIO=1         # PlatformIO 매크로 정의
```

## 📊 시리얼 모니터 출력 예제

### 정상 동작 시
```
=== SmartCool Parasol 시스템 초기화 ===
하드웨어 테스트 모드

시스템 상태 초기화...
✓ 시스템 상태 초기화 완료
핀 모드 설정...
✓ 핀 설정 완료
센서 초기화...
  - DHT22 온습도 센서 초기화
  ✓ DHT22 센서 정상 작동
✓ 센서 초기화 완료

========== 시스템 상태 ==========
온도: 23.5°C | 습도: 55.2%
빗물: 123 | UV: 456 | 조도: 789
물탱크: 234 | 파라솔: 수납 | 냉각: OFF
모드: 대기
센서 상태: 정상
================================
```

## 🐛 문제 해결

### 1. 컴파일 오류
```bash
# 라이브러리 재설치
pio lib uninstall "DHT sensor library"
pio lib install "adafruit/DHT sensor library"

# 클린 빌드
pio run --target clean
pio run
```

### 2. 업로드 실패
```bash
# 포트 확인
pio device list

# 수동 포트 지정 (platformio.ini에 추가)
upload_port = COM3  # Windows
upload_port = /dev/ttyUSB0  # Linux
```

### 3. 시리얼 모니터 문제
```bash
# 필터 설정 (한글 깨짐 방지)
monitor_filters = 
    default
    time
    
# 인코딩 설정
monitor_encoding = utf-8
```

## 📈 성능 모니터링

### 메모리 사용량 확인
```bash
# 빌드 후 메모리 사용량 표시
pio run --verbose
```

### 시리얼 플로터 사용
```cpp
// 센서 데이터를 그래프로 확인
void printForPlotter() {
  Serial.print("Temperature:");
  Serial.print(sensors.temperature);
  Serial.print(",Humidity:");
  Serial.print(sensors.humidity);
  Serial.print(",Rain:");
  Serial.println(sensors.rainLevel);
}
```

## 🔄 업그레이드 계획

### Phase 2: ESP32로 업그레이드
```ini
# platformio.ini 변경
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
```

### Phase 3: 고급 기능 추가
- WiFi 연결
- 웹 인터페이스
- OTA(무선) 업데이트
- MQTT 통신

## 💡 PlatformIO 장점

### Arduino IDE 대비 장점
- **자동 라이브러리 관리**: 종속성 자동 해결
- **강력한 디버깅**: 실시간 디버거 지원
- **멀티 플랫폼**: ESP32, STM32 등 다양한 보드 지원
- **버전 관리**: Git 통합
- **코드 자동완성**: IntelliSense 지원

### 추천 확장 프로그램
```
- PlatformIO IDE (필수)
- C/C++ (Microsoft)
- Serial Monitor
- Git Graph
```

이제 PlatformIO 환경에서 완벽하게 동작합니다! 🚀