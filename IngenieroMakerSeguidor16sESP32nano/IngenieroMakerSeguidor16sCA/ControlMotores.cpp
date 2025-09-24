#include "esp32-hal.h"
#include "esp32-hal-gpio.h"
#include "Arduino.h"
#include "ControlMotores.h"

void Motores::begin() {
  pinMode(motorDerPwm, OUTPUT);
  pinMode(dirMotorDer, OUTPUT);

  pinMode(motorIzqPwm, OUTPUT);
  pinMode(dirMotorIzq, OUTPUT);

  analogWrite(motorDerPwm, 0);
  analogWrite(motorIzqPwm, 0);

  digitalWrite(dirMotorDer, LOW);
  digitalWrite(dirMotorIzq, HIGH);
}

void Motores::motorDer(int velocidadMotores) {
  analogWrite(motorDerPwm, velocidadMotores);
}

void Motores::motorIzq(int velocidadMotores) {
  analogWrite(motorIzqPwm, velocidadMotores);
}

void Motores::motores(int velDer, int velIzq) {
  analogWrite(motorIzqPwm, velIzq);
  analogWrite(motorDerPwm, velDer);
}