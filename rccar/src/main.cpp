#include <PinChangeInterrupt.h>  
#include <math.h>                

// === 입력, 출력력 핀 정의 ===
// [작성자: 황주옥 - PWM 입력 채널 핀 번호 및 출력용 LED핀 설정]
const int ch5_pin = 2;
const int ch6_pin = 3;
const int ch7_pin = 4;
const int ch8_pin = 7;

const int r_pin = 6;
const int g_pin = 10;
const int b_pin = 11;
const int led1_pin = 9;
const int led2_pin = 5;

// === PWM 값 저장 변수 ===
// [작성자: 황주옥 - 인터럽트 기반 측정 결과 저장용 변수]
volatile int pwm_ch5 = 1500;
volatile int pwm_ch6 = 1500;
volatile int pwm_ch7 = 1500;
volatile int pwm_ch8 = 1500;

volatile unsigned long start_ch5 = 0;
volatile unsigned long start_ch6 = 0;
volatile unsigned long start_ch7 = 0;
volatile unsigned long start_ch8 = 0;

// === 상태 변수 ===
// [작성자: 박기주 - 출력 대상과 밝기 값 제어를 위한 상태 변수]
int selectedTarget = 0; // 현재 선택 대상 (0: RGB, 1: LED1, 2: LED2)

// 개별 밝기 저장
// [작성자: 박기주 - 각각의 LED 밝기를 조절하기 위한 밝기기 값 저장용 변수]
int brightness_r = 0, brightness_g = 0, brightness_b = 0;
int led1_brightness = 0;
int led2_brightness = 0;

// === 함수 원형 선언 ===
// [작성자: 황주옥 - 인터럽트 함수 선언]
void ISR_ch5();
void ISR_ch6();
void ISR_ch7();
void ISR_ch8();

// [작성자: 박기주 - 보정 및 색상 변환 관련 함수 선언]
int getSmoothBrightness(int raw);
void hsvToRgb(float h, float s, float v, int& r, int& g, int& b);

// === 설정 함수 ===
// [작성자: 황주옥 - 초기 핀모드 설정 및 인터럽트 등록]
void setup() {
  Serial.begin(9600);

  pinMode(ch5_pin, INPUT_PULLUP);
  pinMode(ch6_pin, INPUT_PULLUP);
  pinMode(ch7_pin, INPUT_PULLUP);
  pinMode(ch8_pin, INPUT_PULLUP);

  pinMode(r_pin, OUTPUT);
  pinMode(g_pin, OUTPUT);
  pinMode(b_pin, OUTPUT);
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);

  attachPCINT(digitalPinToPCINT(ch5_pin), ISR_ch5, CHANGE);
  attachPCINT(digitalPinToPCINT(ch6_pin), ISR_ch6, CHANGE);
  attachPCINT(digitalPinToPCINT(ch7_pin), ISR_ch7, CHANGE);
  attachPCINT(digitalPinToPCINT(ch8_pin), ISR_ch8, CHANGE);
}

// === 메인 루프 ===
// [작성자: 박기주 - 전원 상태, 대상 선택, 밝기 및 색상 처리]
void loop() {
  bool powerOn = pwm_ch8 > 1500;  // ON/OFF 판단

  // 대상 선택
  int mappedTarget = map(pwm_ch5, 1000, 2000, 0, 3);
  selectedTarget = constrain(mappedTarget, 0, 2);

  // 밝기 계산
  int brightness = getSmoothBrightness(pwm_ch6);

  // 선택 대상의 밝기/색상 처리 
  // **3색 RED 밝기값 RED값이 불안정하게 나오는 문제 박기주와 함께 해결**
  if (selectedTarget == 0) {
    int r_raw, g_raw, b_raw;
    float hue = map(pwm_ch7, 1200, 2000, 0, 360);
    hue = constrain(hue, 0.0, 360.0);
    hsvToRgb(hue, 1.0, 1.0, r_raw, g_raw, b_raw);

    brightness_r = r_raw * brightness / 255;
    brightness_g = g_raw * brightness / 255;
    brightness_b = b_raw * brightness / 255;
  } else if (selectedTarget == 1) {
    led1_brightness = brightness;
  } else if (selectedTarget == 2) {
    led2_brightness = brightness;
  }

  // 출력
  if (powerOn) {
    analogWrite(r_pin, brightness_r);
    analogWrite(g_pin, brightness_g);
    analogWrite(b_pin, brightness_b);
    analogWrite(led1_pin, led1_brightness);
    analogWrite(led2_pin, led2_brightness);
  } else {
    analogWrite(r_pin, 0);
    analogWrite(g_pin, 0);
    analogWrite(b_pin, 0);
    analogWrite(led1_pin, 0);
    analogWrite(led2_pin, 0);
  }

  // 디버깅 정보 출력
  Serial.print("[Selected] ");
  Serial.print(selectedTarget == 0 ? "RGB" : selectedTarget == 1 ? "LED1" : "LED2");
  Serial.print(" | Brightness R,G,B: ");
  Serial.print(brightness_r); Serial.print(", ");
  Serial.print(brightness_g); Serial.print(", ");
  Serial.print(brightness_b);
  Serial.print(" | LED1: "); Serial.print(led1_brightness);
  Serial.print(" | LED2: "); Serial.print(led2_brightness);
  Serial.print(" | ON/OFF: "); Serial.println(powerOn);

  delay(50);
}

// === 인터럽트 핸들러 ===
// [작성자: 황주옥 - PWM 펄스 폭 측정을 위한 핀 인터럽트 처리]
void ISR_ch5() {
  if (digitalRead(ch5_pin) == HIGH) start_ch5 = micros();
  else pwm_ch5 = micros() - start_ch5;
}
void ISR_ch6() {
  if (digitalRead(ch6_pin) == HIGH) start_ch6 = micros();
  else pwm_ch6 = micros() - start_ch6;
}
void ISR_ch7() {
  if (digitalRead(ch7_pin) == HIGH) start_ch7 = micros();
  else pwm_ch7 = micros() - start_ch7;
}
void ISR_ch8() {
  if (digitalRead(ch8_pin) == HIGH) start_ch8 = micros();
  else pwm_ch8 = micros() - start_ch8;
}

// === 밝기 보정 함수 ===
// [작성자: 박기주 - 감마 보정을 통한 부드러운 밝기 계산]
int getSmoothBrightness(int raw) {
  float norm = map(raw, 1000, 2000, 0, 1000) / 1000.0;
  norm = constrain(norm, 0.0, 1.0);
  float gamma = 1.2;
  int brightness = pow(norm, gamma) * 215 + 40;
  return constrain(brightness, 40, 255);
}

// === HSV → RGB 변환 함수 ===
// [작성자: 박기주 - 색상(Hue)을 R,G,B값으로 변환]
void hsvToRgb(float h, float s, float v, int& r, int& g, int& b) {
  h = fmod(h, 360.0);
  if (h < 0) h += 360;

  s = constrain(s, 0.0, 1.0);
  v = constrain(v, 0.0, 1.0);

  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - c;

  float r1, g1, b1;

  if (h < 60)      { r1 = c; g1 = x; b1 = 0; }
  else if (h < 120){ r1 = x; g1 = c; b1 = 0; }
  else if (h < 180){ r1 = 0; g1 = c; b1 = x; }
  else if (h < 240){ r1 = 0; g1 = x; b1 = c; }
  else if (h < 300){ r1 = x; g1 = 0; b1 = c; }
  else             { r1 = c; g1 = 0; b1 = x; }

  r = constrain(round((r1 + m) * 255), 0, 255);
  g = constrain(round((g1 + m) * 255), 0, 255);
  b = constrain(round((b1 + m) * 255), 0, 255);
}
