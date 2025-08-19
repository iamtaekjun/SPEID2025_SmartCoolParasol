# SmartCool Parasol 핀 연결 가이드

## 🔌 아두이노 우노 핀 맵핑

### 📍 디지털 핀 (Digital Pins)
```
Pin 2  → DHT22 온습도 센서 (DATA)
Pin 5  → 배수 밸브 릴레이 (IN1)
Pin 6  → 미스트 분사 릴레이 (IN2)  
Pin 7  → 워터펌프 릴레이 (IN3)
Pin 8  → 부저 (Buzzer +)
Pin 9  → 서보모터 (Signal/Orange)
Pin 13 → 내장 LED (Built-in)
```

### 📍 아날로그 핀 (Analog Pins)
```
A0 → 빗물 감지 센서 (Signal)
A1 → UV 센서 (Signal)
A2 → 조도 센서 (Signal)  
A3 → 물탱크 수위 센서 (Signal)
```

### 📍 전원 및 GND 연결
```
5V  → 모든 센서 VCC (DHT22, 서보모터 등)
3.3V → (사용 안함)
GND → 모든 센서 및 부품 GND 공통 연결
```

## 🔧 센서별 상세 연결도

### 1. DHT22 온습도 센서
```
DHT22    →    Arduino
VCC      →    5V
DATA     →    Pin 2
GND      →    GND
NC       →    (연결 안함)
```

### 2. 서보모터 (SG90)
```
Servo    →    Arduino
Red      →    5V
Brown    →    GND  
Orange   →    Pin 9
```

### 3. 아날로그 센서들 (공통 연결법)
```
센서     →    Arduino
VCC      →    5V
Signal   →    A0/A1/A2/A3
GND      →    GND
```

### 4. 릴레이 모듈 (4채널)
```
릴레이   →    Arduino
VCC      →    5V
GND      →    GND
IN1      →    Pin 5 (배수 밸브)
IN2      →    Pin 6 (미스트)
IN3      →    Pin 7 (워터펌프)
IN4      →    (예비)
```

### 5. 부저 (Passive Buzzer)
```
Buzzer   →    Arduino
+        →    Pin 8
-        →    GND
```

## ⚡ 전원 공급 계획

### Phase 1: 개발/테스트 단계
```
아두이노: USB 전원 (5V, 최대 500mA)
센서들: 아두이노 5V 핀에서 공급
릴레이: 아두이노 5V 핀에서 공급 (신호만)
실제 펌프: 연결하지 않음 (릴레이 LED로 확인)
```

### Phase 2: 실제 동작 단계  
```
아두이노: USB 어댑터 (5V, 2A)
센서들: 아두이노에서 공급
펌프/밸브: 별도 12V 어댑터 (12V, 3A)
공통 GND: 반드시 연결!
```

## 🛠️ 단계별 연결 순서

### Step 1: 기본 센서 연결 (첫 번째 테스트)
```
1. DHT22 센서만 연결
2. 내장 LED 사용 (Pin 13)
3. 부저 연결 (Pin 8)
4. 코드 업로드 후 동작 확인
```

### Step 2: 서보모터 추가
```
1. 서보모터 연결 (Pin 9)
2. 파라솔 동작 테스트
3. 온도 변화 → 서보 동작 확인
```

### Step 3: 아날로그 센서 추가
```
1. 빗물 센서 연결 (A0)
2. 물방울 테스트
3. 나머지 센서들 순차 연결 (A1, A2, A3)
```

### Step 4: 릴레이 모듈 추가
```
1. 릴레이 모듈 연결 (Pin 5, 6, 7)
2. LED로 릴레이 동작 확인
3. 실제 펌프는 나중에 연결
```

## 📋 연결 체크리스트

### ✅ 연결 전 확인사항
- [ ] 아두이노 전원 OFF 상태에서 연결
- [ ] 모든 VCC는 5V에, GND는 GND에 연결
- [ ] 극성 확인 (특히 서보모터, 부저)
- [ ] 짧은 연결 (short) 없는지 확인

### ✅ 연결 후 확인사항  
- [ ] 전원 켠 후 LED 정상 점등
- [ ] 시리얼 모니터에서 센서 값 확인
- [ ] 센서에 물리적 자극 시 값 변화 확인
- [ ] 서보모터 초기 동작 확인

## ⚠️ 주의사항

### 🔥 전기 안전
```
- 전원 연결 시 극성 주의
- 합선 방지를 위한 점검
- 과전류 방지 (퓨즈 사용 권장)
```

### 💧 방수 대책
```
- 센서 연결부 절연 처리
- 케이스 사용 권장  
- 실외 사용 시 IP65 등급 필요
```

### 🔧 기계적 고정
```
- 브레드보드 사용 시 연결 불량 주의
- 중요 연결부는 납땜 권장
- 케이블 정리로 단선 방지
```

## 🧪 테스트 코드 예제

### 연결 테스트용 간단 코드
```cpp
// 각 핀별 연결 상태 확인
void testAllConnections() {
  // DHT22 테스트
  float temp = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.println(isnan(temp) ? "ERROR" : String(temp));
  
  // 아날로그 센서 테스트
  Serial.print("Rain: "); Serial.println(analogRead(A0));
  Serial.print("UV: "); Serial.println(analogRead(A1));
  Serial.print("Light: "); Serial.println(analogRead(A2));
  Serial.print("Water: "); Serial.println(analogRead(A3));
  
  // 서보모터 테스트
  servo.write(0); delay(500);
  servo.write(90); delay(500);
  servo.write(0);
  
  // LED 및 부저 테스트
  digitalWrite(13, HIGH);
  tone(8, 1000, 200);
  delay(300);
  digitalWrite(13, LOW);
}
```

이 가이드를 따라 차근차근 연결하시면 안전하고 확실하게 하드웨어를 구성할 수 있습니다! 🔧