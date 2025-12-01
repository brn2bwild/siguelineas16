#include "esp32-hal-ledc.h"
#include "esp32-hal.h"
#include "esp32-hal-gpio.h"
#include "Arduino.h"
#include "Max14870.h"

PuenteH::PuenteH() {
  pinMode(motorIzqPwm, OUTPUT);
  pinMode(motorDerPwm, OUTPUT);

  analogWrite(motorDerPwm, 0);
  analogWrite(motorIzqPwm, 0);

  pinMode(dirMotorDer, OUTPUT);
  pinMode(dirMotorIzq, OUTPUT);

  // ledcAttach(motorIzqPwm, pwmFreq, pwmRes);
  // ledcAttach(motorDerPwm, pwmFreq, pwmRes);

  // ledcWrite(motorIzqPwm, 0);
  // ledcWrite(motorDerPwm, 0);
}

void PuenteH::motorIzq(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(dirMotorIzq, HIGH);
  } else {
    digitalWrite(dirMotorIzq, LOW);
  }
  // ledcWrite(motorIzqPwm, abs(velocidad));
  analogWrite(motorIzqPwm, abs(velocidad));
}

void PuenteH::motorDer(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(dirMotorDer, HIGH);
  } else {
    digitalWrite(dirMotorDer, LOW);
  }
  // ledcWrite(motorDerPwm, abs(velocidad));
  analogWrite(motorDerPwm, abs(velocidad));
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