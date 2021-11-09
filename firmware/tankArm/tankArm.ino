// макс. скорость приводов руки
#define MAX_ARM_SPEED 40

// пределы поворота приводов руки
// здесь и далее:
// CLAW - сам захват
// WRIST - кисть (вверх вниз)
// ARM - рука (вперёд назад)
// YAW - вращение по горизонтали
#define CLAW_MIN 30
#define CLAW_MAX 300
#define WRIST_MIN 0
#define WRIST_MAX 450
#define ARM_MIN 150
#define ARM_MAX 900

// мин. сигнал, при котором мотор начинает вращение
#define MIN_DUTY 100

// пины драйвера
#define MOT_RA 2
#define MOT_RB 3
#define MOT_LA 4
#define MOT_LB 5

// пины манипулятора
#define SERVO_CLAW 6
#define SERVO_WRIST 7
#define SERVO_ARM 8

#include <Servo.h>
Servo claw, wrist, arm, yaw;

#include <GyverMotor.h>
// (тип, пин, ШИМ пин, уровень)
GMotor motorR(DRIVER2WIRE, MOT_RA, MOT_RB, HIGH);
GMotor motorL(DRIVER2WIRE, MOT_LA, MOT_LB, HIGH);

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

void setup() {
  Serial.begin(9600);

  // подключаем
  claw.attach(SERVO_CLAW);
  wrist.attach(SERVO_WRIST);
  arm.attach(SERVO_ARM);

  // ставим на начальные позиции
  claw.write(clawP);
  wrist.write(wristP);
  arm.write(armP);

  motorR.setMode(AUTO);
  motorL.setMode(AUTO);

  // НАПРАВЛЕНИЕ ГУСЕНИЦ (зависит от подключения)
  motorR.setDirection(REVERSE);
  motorL.setDirection(NORMAL);

  // мин. сигнал вращения
  motorR.setMinDuty(MIN_DUTY);
  motorL.setMinDuty(MIN_DUTY);

  // плавность скорости моторов
  motorR.setSmoothSpeed(80);
  motorL.setSmoothSpeed(80);

  // подрубаем геймпад
  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
}

void loop() {
  // читаем геймпад
  bool success = ps2x.read_gamepad(false, 0);  // читаем
  ps2x.reconfig_gamepad();      // костыль https://stackoverflow.com/questions/46493222/why-arduino-needs-to-be-restarted-after-ps2-controller-communication-in-arduino

  if (success) {
    // таймер на 30 мс (30 раз в секунду)
    static uint32_t tmr;
    if (success && millis() - tmr >= 30) {
      tmr = millis();
      if (ps2x.Button(PSB_R2)) wristP += MAX_ARM_SPEED;
      if (ps2x.Button(PSB_L2)) wristP -= MAX_ARM_SPEED;

      clawP += map(ps2x.Analog(PSS_RX), 0, 255, MAX_ARM_SPEED, -MAX_ARM_SPEED);
      armP += map(ps2x.Analog(PSS_RY), 0, 255, -MAX_ARM_SPEED, MAX_ARM_SPEED);
      
      // ограничиваем скорость
      clawP = constrain(clawP, CLAW_MIN, CLAW_MAX);
      wristP = constrain(wristP, WRIST_MIN, WRIST_MAX);
      armP = constrain(armP, ARM_MIN, ARM_MAX);

      Serial.print(clawP);Serial.print(',');
      Serial.print(wristP);Serial.print(',');
      Serial.println(armP);

      // переводим в градусы и применяем
      claw.write(clawP / 10);
      wrist.write(wristP / 10);
      arm.write(armP / 10);
    }

    // преобразуем стики от 0..255 к -255, 255
    int LX = map(ps2x.Analog(PSS_LX), 255, 0, -255, 255);
    int LY = map(ps2x.Analog(PSS_LY), 255, 0, -255, 255);

    // танковая схема
    int dutyR = LY + LX;
    int dutyL = LY - LX;
    dutyR = constrain(dutyR, -255, 255);
    dutyL = constrain(dutyL, -255, 255);

    if (abs(dutyR) < 10) dutyR = 0;
    if (abs(dutyL) < 10) dutyL = 0;

    // задаём целевую скорость    
    motorR.smoothTick(dutyR);
    motorL.smoothTick(dutyL);
  } else {
    // проблема с геймпадом - остановка
    motorR.setSpeed(0);
    motorL.setSpeed(0);
  }
}
