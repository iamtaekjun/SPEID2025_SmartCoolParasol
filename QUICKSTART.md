# SmartCool Parasol - PlatformIO 빠른 시작 가이드

## 🚀 1분 만에 시작하기

### 1단계: PlatformIO 설치
```bash
1. VS Code 실행
2. Extensions (Ctrl+Shift+X) 클릭
3. "PlatformIO IDE" 검색 후 설치
4. VS Code 재시작 (중요!)
```

### 2단계: 프로젝트 열기
```bash
1. VS Code에서 File → Open Folder
2. SmartCool_Parasol 폴더 선택
3. PlatformIO가 자동으로 프로젝트 인식
4. 하단에 PlatformIO 툴바 확인
```

### 3단계: 아두이노 연결 및 테스트
```bash
1. 아두이노 우노를 USB로 연결
2. 하단 툴바에서 "Upload" 버튼 클릭 (→)
3. 컴파일 및 업로드 완료 대기
4. "Serial Monitor" 버튼 클릭 (🔌)
```

## 🎯 PlatformIO 주요 버튼 설명

### 하단 툴바 버튼들
```
🏠 Home        - PlatformIO 홈 화면
✓ Build       - 컴파일만 실행
→ Upload      - 컴파일 + 아두이노 업로드  
🔌 Monitor     - 시리얼 모니터 열기
🗑 Clean       - 빌드 캐시 삭제
🔧 Test        - 단위 테스트 실행
```

### 자주 사용하는 단축키
```
Ctrl+Alt+B    - 빌드 (컴파일)
Ctrl+Alt+U    - 업로드
Ctrl+Alt+S    - 시리얼 모니터
Ctrl+Alt+T    - 터미널 열기
```

## 📱 첫 번째 테스트

### 예상 시리얼 출력
```
=== SmartCool Parasol 시스템 초기화 ===
하드웨어 테스트 모드

✓ 시스템 상태 초기화 완료
✓ 핀 설정 완료
✓ 센서 초기화 완료
✓ 액추에이터 초기화 완료

========== 시스템 상태 ==========
온도: 23.5°C | 습도: 55.2%
빗물: 123 | UV: 456 | 조도: 789
물탱크: 234 | 파라솔: 수납 | 냉각: OFF
모드: 대기
================================
```

## ⚠️ 자주 발생하는 문제

### 1. "Project does not exist" 오류
```
해결책:
1. PlatformIO 완전히 설치되었는지 확인
2. VS Code 재시작
3. 폴더에 platformio.ini 파일 있는지 확인
```

### 2. 업로드 실패
```
해결책:
1. 아두이노 USB 연결 확인
2. 다른 시리얼 프로그램 종료 (Arduino IDE 등)
3. 아두이노 리셋 버튼 누른 후 재시도
```

### 3. 라이브러리 오류
```
해결책:
1. PlatformIO Terminal 열기 (Ctrl+Alt+T)
2. "pio lib install" 입력
3. 자동으로 필요한 라이브러리 설치
```

## 🎉 성공 확인

### ✅ 다음이 모두 동작하면 성공!
- [ ] 컴파일 오류 없이 업로드 완료
- [ ] 시리얼 모니터에서 센서 값 출력
- [ ] DHT22 온습도 센서 정상 동작
- [ ] 내장 LED 깜빡임 확인
- [ ] 부저 테스트 소리 확인

### 🔧 다음 단계
1. **하드웨어 연결**: wiring_guide.md 참고
2. **센서 테스트**: 각 센서별 동작 확인
3. **로직 테스트**: 빗물/온도 임계값 테스트

---

**💡 Tip**: PlatformIO는 Arduino IDE보다 훨씬 강력합니다! 자동완성, 디버깅, 라이브러리 관리 등 모든 기능이 통합되어 있어요. 🚀