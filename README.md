# 🎮 Arduino + R9D 무선 LED 제어 시스템

본 프로젝트는 **R9D 수신기**를 통해 아두이노가 **PWM 신호 4개를 수신**하고,  
이를 기반으로 **RGB LED 및 일반 LED 두 개의 색상, 밝기, 전원 상태**를 무선으로 제어하는 시스템입니다.

---

## 📺 시연 영상

👉 [시연 영상 보러가기](https://youtu.be/d78b-IXxTWc?si=KHArjQPACeX8IalK)

영상에서는 다음 3가지 기능이 정상 동작하는 것을 확인할 수 있습니다:
1. **RGB LED 색상 및 밝기 조절**
2. **LED1, LED2의 독립적인 밝기 조절**
3. **전체 ON/OFF 스위치 기능**

또한 영상에서는 회로 구성을 카메라로 직접 비추며 **R9D와 아두이노, LED 간의 배선 상태를 설명**합니다.

---

## 🧩 입력/출력 핀 연결도

### 📥 입력 (R9D → Arduino)

| R9D 채널 | Arduino 핀 | 설명                           |
|----------|-------------|--------------------------------|
| CH5      | D2          | 제어 대상 선택 (RGB/LED1/LED2) |
| CH6      | D3          | 밝기 조절                      |
| CH7      | D4          | 색상 조절 (RGB 전용)           |
| CH8      | D7          | 전원 ON/OFF 스위치             |

### 💡 출력 (Arduino → LED)

| Arduino 핀 | 연결 대상 | 설명                 |
|------------|------------|----------------------|
| D6         | RGB LED R  | 빨간색 출력 (PWM)    |
| D10        | RGB LED G  | 초록색 출력 (PWM)    |
| D11        | RGB LED B  | 파란색 출력 (PWM)    |
| D9         | LED1       | 일반 LED 1 출력 (PWM) |
| D5         | LED2       | 일반 LED 2 출력 (PWM) |

---

## 🖼 실물 회로 구성 사진

> 아래 사진은 R9D 수신기, Arduino Uno, 브레드보드, RGB 및 일반 LED의 연결 상태를 보여줍니다.

(![image](https://github.com/user-attachments/assets/75ee19a9-21fa-47df-bd9e-9311ed93bc81))

- 오른쪽 상단: **R9D 수신기**
- 중앙: **Arduino Uno**
- 브레드보드: **RGB LED**, **LED1, LED2**, **저항 연결**
- 각 `CH5~CH8` PWM 신호는 Arduino의 `D2~D7` 핀으로 연결됩니다.


---

## 🧠 주요 기능 요약

| 기능 항목       | 설명                                                                 |
|----------------|----------------------------------------------------------------------|
| PWM 입력 처리   | `PinChangeInterrupt`로 CH5~CH8 PWM 신호를 인터럽트 기반으로 측정         |
| 제어 대상 선택 | CH5를 통해 RGB, LED1, LED2 중 하나 선택                                   |
| 밝기 보정       | `getSmoothBrightness()`에서 감마 보정 적용 (1.2)                        |
| 색상 조절       | HSV → RGB 변환 (`hsvToRgb`)을 통해 부드러운 색상 전환 구현               |
| 전원 스위치     | CH8을 기준으로 전체 시스템 ON/OFF 제어                                   |

---

## ✍️ 코드 구성 및 협업 내역

| 이름     | 담당 역할 |
|----------|-----------|
| **황주옥** | 하드웨어 핀 정의, PWM 입력 측정, 인터럽트 처리, `setup()` 구성 |
| **박기주** | 출력 대상 제어, 밝기 및 색상 처리, 메인 제어 흐름, 보정 함수 및 변환 함수 작성 |

> 🔧 코드의 각 함수 및 주요 변수에는 담당자 이름이 주석으로 명시되어 있어 기능 구분이 명확합니다.

---

