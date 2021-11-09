// макс. скорость приводов руки
#define MAX_ARM_SPEED 40

// пределы поворота приводов руки
// здесь и далее:
// CLAW - сам захват
// WRIST - кисть (вверх вниз)
// ARM - рука (вперёд назад)
// YAW - вращение по горизонтали
#define CLAW_MIN 0
#define CLAW_MAX 300
#define WRIST_MIN 0
#define WRIST_MAX 450
#define ARM_MIN 150
#define ARM_MAX 600
#define YAW_MIN 0
#define YAW_MAX 1800

// пины манипулятора
#define SERVO_CLAW 6
#define SERVO_WRIST 7
#define SERVO_ARM 8
#define SERVO_YAW 9

#include <Servo.h>
Servo claw, wrist, arm, yaw;

// пины ресивера ps2
#define PS2_DAT A0
#define PS2_CMD A1
#define PS2_SEL A2
#define PS2_CLK A3

#include <PS2X_lib.h>
PS2X ps2x;

// позиции приводов (градусы * 10)
int clawP = 0;
int wristP = 0;
int armP = 200;
int yawP = 900;

void setup() {
  //Serial.begin(9600);

  // подключаем
  claw.attach(SERVO_CLAW);
  wrist.attach(SERVO_WRIST);
  arm.attach(SERVO_ARM);
  yaw.attach(SERVO_YAW);

  // ставим на начальные позиции
  claw.write(clawP);
  wrist.write(wristP);
  arm.write(armP);
  yaw.write(yawP);

  // подрубаем геймпад
  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
}

void loop() {
  // читаем геймпад
  bool success = ps2x.read_gamepad(false, 0);  // читаем
  ps2x.reconfig_gamepad();      // костыль https://stackoverflow.com/questions/46493222/why-arduino-needs-to-be-restarted-after-ps2-controller-communication-in-arduino

  // таймер на 30 мс (30 раз в секунду)
  static uint32_t tmr;
  if (success && millis() - tmr >= 30) {
    tmr = millis();
    // преобразуем стики от 0..255 к -255, 255

    //if (ps2x.ButtonPressed(PSB_PAD_RIGHT));
    // изменение позиции на величину скорости
    clawP += map(ps2x.Analog(PSS_RX), 0, 255, MAX_ARM_SPEED, -MAX_ARM_SPEED);
    wristP += map(ps2x.Analog(PSS_RY), 0, 255, MAX_ARM_SPEED, -MAX_ARM_SPEED);
    armP += map(ps2x.Analog(PSS_LY), 0, 255, -MAX_ARM_SPEED, MAX_ARM_SPEED);
    yawP += map(ps2x.Analog(PSS_LX), 0, 255, MAX_ARM_SPEED, -MAX_ARM_SPEED);

    // ограничиваем скорость
    clawP = constrain(clawP, CLAW_MIN, CLAW_MAX);
    wristP = constrain(wristP, WRIST_MIN, WRIST_MAX);
    armP = constrain(armP, ARM_MIN, ARM_MAX);
    yawP = constrain(yawP, YAW_MIN, YAW_MAX);

    // переводим в градусы и применяем
    claw.write(clawP / 10);
    wrist.write(wristP / 10);
    arm.write(armP / 10);
    yaw.write(yawP / 10);
  }
}
