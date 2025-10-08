#include "esp32-hal.h"
#include "esp32-hal-gpio.h"
#include "Arduino.h"
#include "Max14870.h"

void PuenteH::begin() {
  pinMode(motorDerPwm, OUTPUT);
  pinMode(motorIzqPwm, OUTPUT);

  analogWrite(motorDerPwm, 0);
  analogWrite(motorIzqPwm, 0);

  pinMode(dirMotorDer, OUTPUT);
  pinMode(dirMotorIzq, OUTPUT);
}

void PuenteH::motorIzq(int velocidad) {
  if (velocidad > 0) {
    digitalWrite(dirMotorIzq, HIGH);
    analogWrite(motorIzqPwm, velocidad);
  } else {
    digitalWrite(dirMotorIzq, LOW);
    analogWrite(motorIzqPwm, (-1) * velocidad);
  }
}

void PuenteH::motorDer(int velocidad) {
  if (velocidad > 0) {
    digitalWrite(dirMotorDer, LOW);
    analogWrite(motorDerPwm, velocidad);
  } else {
    digitalWrite(dirMotorDer, HIGH);
    analogWrite(motorDerPwm, (-1) * velocidad);
  }
}

void PuenteH::motores(int velIzq, int velDer) {
  motorIzq(velIzq);
  motorDer(velDer);
}

void PuenteH::freno(bool sentido, int velocidad) {
  if (sentido) {
    motorIzq(-velocidad);
    motorDer(velocidad);
  } else {
    motorIzq(velocidad);
    motorDer(-velocidad);
  }
}