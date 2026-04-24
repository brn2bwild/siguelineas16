#include "esp32-hal-ledc.h"
#include "esp32-hal.h"
#include "esp32-hal-gpio.h"
#include "Arduino.h"
#include "Max14870.h"

PuenteH::PuenteH() {
  pinMode(dirMotorDer, OUTPUT);
  pinMode(dirMotorIzq, OUTPUT);

  pinMode(motorIzqPwm, OUTPUT);
  pinMode(motorDerPwm, OUTPUT);

  ledcAttach(motorIzqPwm, pwmFreq, pwmRes);
  ledcAttach(motorDerPwm, pwmFreq, pwmRes);

  ledcWrite(motorIzqPwm, 0);
  ledcWrite(motorDerPwm, 0);
}

void PuenteH::motorIzq(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(dirMotorIzq, HIGH);
    ledcWrite(motorIzqPwm, velocidad);
  } else {
    digitalWrite(dirMotorIzq, LOW);
    ledcWrite(motorIzqPwm, velocidad * (-1));
  }
}

void PuenteH::motorDer(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(dirMotorDer, HIGH);
    ledcWrite(motorDerPwm, velocidad);
  } else {
    digitalWrite(dirMotorDer, LOW);
    ledcWrite(motorDerPwm, velocidad * (-1));
  }
}

void PuenteH::motores(int velIzq, int velDer) {
  motorIzq(velIzq);
  motorDer(velDer);
}

void PuenteH::freno(bool sentido, int velocidad) {
  if (sentido) {
    motorIzq((-1) * velocidad);
    motorDer(velocidad);
  } else {
    motorIzq(velocidad);
    motorDer((-1) * velocidad);
  }
}