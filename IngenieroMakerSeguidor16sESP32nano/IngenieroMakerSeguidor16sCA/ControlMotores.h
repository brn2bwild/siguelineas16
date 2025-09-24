#ifndef Motores_h
#define Motores_h
#include "Arduino.h"

class Motores {
public:
  const int motorDerPwm = 8;
  const int motorIzqPwm = 6;
  const int dirMotorIzq = 18;
  const int dirMotorDer = 9;

  void begin();
  void motorDer(int velocidadMotores);
  void motorIzq(int velocidadMotores);
  void motores(int velDer, int velIzqr);
private:
};
#endif
