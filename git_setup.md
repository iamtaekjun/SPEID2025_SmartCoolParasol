# Git & GitHub 연결 가이드

## 🔧 **1단계: Git 초기화**

### VS Code 터미널에서 (Ctrl + `)
```bash
# 프로젝트 폴더에서 Git 초기화
git init

# 현재 폴더가 맞는지 확인
pwd
# 결과: C:\Users\jtj72\Desktop\workSpace\SmartCool_Parasol
```

## 📁 **2단계: .gitignore 파일 생성**

### PlatformIO용 .gitignore 필요 (불필요한 파일 제외)

## 📤 **3단계: GitHub 원격 저장소 연결**

### GitHub에서 레포지토리 URL 복사 후:
```bash
# 원격 저장소 추가 (HTTPS)
git remote add origin https://github.com/사용자명/레포지토리명.git

# 또는 SSH (설정되어 있다면)
git remote add origin git@github.com:사용자명/레포지토리명.git

# 연결 확인
git remote -v
```

## 📦 **4단계: 첫 번째 커밋 & 푸시**

```bash
# 파일 추가
git add .

# 첫 번째 커밋
git commit -m "Initial commit: SmartCool Parasol 프로젝트 초기 설정"

# GitHub에 푸시
git push -u origin main
```

## ⚠️ **주의사항**

### GitHub 브랜치명 확인
```bash
# 기본 브랜치명 확인
git branch

# main이 아니라 master인 경우
git push -u origin master
```

### GitHub 인증
```bash
# 토큰 인증이 필요할 수 있음
Username: GitHub사용자명
Password: Personal Access Token (GitHub에서 생성)
```

## 🚀 **VS Code Git 기능 사용**

### 좌측 Source Control 패널:
```
1. Ctrl + Shift + G (Git 패널)
2. "Initialize Repository" 버튼 클릭
3. 파일들이 자동으로 감지됨
4. 커밋 메시지 작성 후 Commit
5. "Publish to GitHub" 클릭
```