#include "esp32-hal.h"
#include "esp32-hal-gpio.h"
#include "Arduino.h"
#include "ControlMotores.h"

void motores::motordere() {
  pinMode(motorDerPwm, OUTPUT);
  pinMode(dirMotorDer, OUTPUT);
  digitalWrite(dirMotorDer, HIGH);
  analogWrite(motorDerPwm, velocidadMotor);
}
void motores::motorizq() {
  pinMode(motorIzqPwm, OUTPUT);
  pinMode(dirMotorIzq, OUTPUT);
  digitalWrite(dirMotorIzq, LOW);
  analogWrite(motorIzqPwm, velocidadMotor)
}