// мин. сигнал, при котором мотор начинает вращение
#define MIN_DUTY 120

// пины драйвера
#define MOT_RA 2
#define MOT_RB 3
#define MOT_LA 4
#define MOT_LB 5

// пины ресивера ps2
#define PS2_DAT A0
#define PS2_CMD A1
#define PS2_SEL A2
#define PS2_CLK A3

// ===========================
#include <GyverMotor.h>
// (тип, пин, ШИМ пин, уровень)
GMotor motorR(DRIVER2WIRE, MOT_RA, MOT_RB, HIGH);
GMotor motorL(DRIVER2WIRE, MOT_LA, MOT_LB, HIGH);

#include <PS2X_lib.h>
PS2X ps2x;

void setup() {
  //Serial.begin(9600);

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
    // преобразуем стики от 0..255 к -255, 255
    int LX = map(ps2x.Analog(PSS_LX), 255, 0, -255, 255);
    int LY = map(ps2x.Analog(PSS_LY), 255, 0, -255, 255);

    // танковая схема
    int dutyR = LY + LX;
    int dutyL = LY - LX;
    dutyR = constrain(dutyR, -255, 255);
    dutyL = constrain(dutyL, -255, 255);

    // задаём целевую скорость
    motorR.smoothTick(dutyR);
    motorL.smoothTick(dutyL);
  } else {
    // проблема с геймпадом - остановка
    motorR.setSpeed(0);
    motorL.setSpeed(0);
  }
}
