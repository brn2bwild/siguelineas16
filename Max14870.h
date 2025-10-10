#ifndef PuenteH_h
#define PuenteH_h
#include "Arduino.h"

class PuenteH {
public:
  const int dirMotorIzq = 18;
  const int motorIzqPwm = 6;

  const int dirMotorDer = 9;
  const int motorDerPwm = 8;

  const int pwmFreq = 5000;
  const int pwmRes = 10;

  void begin();
  void motorDer(int velocidad);
  void motorIzq(int velocidad);
  void motores(int velDer, int velIzqr);
  void freno(bool sentido, int velocidad);
private:
};
#endif
