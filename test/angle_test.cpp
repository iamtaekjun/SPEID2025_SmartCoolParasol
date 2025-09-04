#include <Arduino.h>
#include <Servo.h>

// ============= 핀 정의 =============
// 센서 레이어 (Sensor Layer)
#define TEMP_SENSOR_PIN A4      // KY-013 아날로그 온도센서 핀
#define RAIN_SENSOR_PIN A0      // 빗물 감지 센서 (아날로그)
#define WATER_LEVEL_PIN A3      // 물탱크 수위 센서 (아날로그)

// 액추에이터 레이어 (Actuator Layer)
#define SERVO_PIN 9             // 파라솔 구동 서보모터 
#define RELAY_PIN 6             // 릴레이 모듈 제어 핀

Servo parasolServo;

parasolServo.attach(SERVO_PIN);

parasolServo.write(180);