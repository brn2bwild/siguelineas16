#ifndef PuenteH_h
#define PuenteH_h
#include "Arduino.h"
#include <stdint.h>

class PuenteH {
public:
  const uint8_t dirMotorIzq = 18;
  const uint8_t motorIzqPwm = 8;

  const uint8_t dirMotorDer = 9;
  const uint8_t motorDerPwm = 6;

  // const int pwmFreq = 1000;
  // const int pwmRes = 10;

  // void begin();
  PuenteH();
  void motorIzq(int velocidad);
  void motorDer(int velocidad);
  void motores(int velIzq, int velDer);
  void freno(bool sentido, int velocidad);
private:
};
#endif
