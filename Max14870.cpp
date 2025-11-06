#include "esp32-hal-ledc.h"
#include "esp32-hal.h"
#include "esp32-hal-gpio.h"
#include "Arduino.h"
#include "Max14870.h"

void PuenteH::begin() {
  // pinMode(motorDerPwm, OUTPUT);
  // pinMode(motorIzqPwm, OUTPUT);

  ledcAttach(motorIzqPwm, pwmFreq, pwmRes);
  ledcAttach(motorDerPwm, pwmFreq, pwmRes);

  ledcWrite(motorIzqPwm, 0);
  ledcWrite(motorDerPwm, 0);

  // analogWrite(motorDerPwm, 0);
  // analogWrite(motorIzqPwm, 0);

  pinMode(dirMotorDer, OUTPUT);
  pinMode(dirMotorIzq, OUTPUT);
}

void PuenteH::motorIzq(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(dirMotorIzq, HIGH);
    // ledcWrite(motorIzqPwm, velocidad);
    // analogWrite(motorIzqPwm, velocidad);
  } else {
    digitalWrite(dirMotorIzq, LOW);
    // ledcWrite(motorIzqPwm, (-1) * velocidad);
    // analogWrite(motorIzqPwm, (-1) * velocidad);
  }
  ledcWrite(dirMotorIzq, abs(velocidad));
}

void PuenteH::motorDer(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(dirMotorDer, LOW);
    // ledcWrite(motorDerPwm, velocidad);
    // analogWrite(motorDerPwm, velocidad);
  } else {
    digitalWrite(dirMotorDer, HIGH);
    // ledcWrite(motorDerPwm, (-1) * velocidad);
    // analogWrite(motorDerPwm, (-1) * velocidad);
  }
  ledcWrite(dirMotorDer, abs(velocidad));
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