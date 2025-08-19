# PlatformIO 오류 해결 가이드

## 🚫 발생한 오류들

### 1. `#include errors detected` 오류
```
#include errors detected. Please update your includePath.
```

### 2. `could not open source file "Arduino.h"` 오류  
```
could not open source file "Arduino.h" (no directories in search list)
```

### 3. `option 'build_flags' in section 'env:uno' already exists` 오류
```
While reading from platformio.ini [line 39]: option 'build_flags' already exists
```

## ✅ 해결 방법

### 1단계: platformio.ini 파일 수정 완료
중복된 build_flags 옵션을 제거하고 간소화했습니다.

### 2단계: PlatformIO 프로젝트 리빌드
```bash
1. VS Code에서 Ctrl+Shift+P
2. "PlatformIO: Clean" 입력 후 실행
3. "PlatformIO: Build" 입력 후 실행
```

### 3단계: 또는 터미널에서 실행
```bash
# PlatformIO 터미널에서
pio run --target clean
pio run
```

### 4단계: IntelliSense 재빌드
```bash
1. Ctrl+Shift+P
2. "C/C++: Reset IntelliSense Database" 검색 후 실행
3. 또는 "PlatformIO: Rebuild IntelliSense Index" 실행
```

## 🔧 추가 해결 방법

### VS Code 재시작
```bash
1. VS Code 완전 종료 (Ctrl+Shift+W)
2. 다시 프로젝트 폴더 열기
3. PlatformIO가 자동으로 프로젝트 인덱싱
```

### 라이브러리 재설치 (필요시)
```bash
# PlatformIO 터미널에서
pio lib uninstall --all
pio lib install
```

## 📋 오류 해결 체크리스트

### ✅ 확인할 것들
- [ ] platformio.ini 파일에 중복 옵션 없음
- [ ] src/main.cpp 파일에 #include <Arduino.h> 있음
- [ ] 프로젝트 클린 빌드 완료
- [ ] IntelliSense 재빌드 완료
- [ ] 빨간 줄 오류 표시 사라짐

### 🎯 성공 확인 방법
1. **편집기에서 빨간 오류 줄이 사라짐**
2. **자동완성이 정상 작동**
3. **빌드 시 오류 없음**

## 🚀 다음 단계

오류가 해결되면:
1. **빌드 테스트**: Ctrl+Alt+B
2. **업로드 테스트**: Ctrl+Alt+U (아두이노 연결 시)
3. **센서 연결 시작**: DHT22부터 시작

## 💡 예방 팁

앞으로 이런 오류를 피하려면:
- platformio.ini 수정 시 중복 옵션 주의
- 새 라이브러리 추가 시 PlatformIO Home에서 추가
- 프로젝트 변경 후 Clean & Build 습관화

이제 오류가 해결되었을 겁니다! 🎉