#ifndef motores_h
#define motores_h
#include "Arduino.h"

class motores {
public:
  const int motorDerPwm = 6;
  const int motorIzqPwm = 8;
  const int dirMotorIzq = 18;
  const int dirMotorDer = 9;

  void motordere();
  void motorizq();
private:
};
#endif
