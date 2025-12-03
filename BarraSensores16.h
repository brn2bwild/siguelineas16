#ifndef BarraSensores16_h
#define BarraSensores16_h
#include "Arduino.h"

class BarraSensores16 {
public:
  // Constantes para los pines de conexión
  const int LedsIr = 38;     // Pin para encender los leds ir
  const int S0 = 1;          // Pin A0 de modo digital
  const int S1 = 2;          // Pin A1 de modo digital
  const int S2 = 3;          // Pin A2 de modo digital
  const int S3 = 4;          // Pin A3 de modo digital
  const int ANALOG_IN = 11;  // Pin A4 en modo analógico
  // const int Led = 48U;

  // Variables de los sensores.
  int sensorValores[16];
  int Valor_Digital[16];
  int sensorValores_B[16];
  int sensorValores_N[16];

  // Variables de contorno
  int s0, s1, s14, s15;

  // Variables de posición ponderada
  int x;
  int y;
  int p;
  int x1;

  // Variables de calibracion
  int Muestras[16] = { 590, 686, 600, 600, 617, 585, 631, 566, 592, 563, 617, 627, 634, 697, 616, 650 };

  void leer_blanco();
  void leer_negro();
  void Calcula_muestras();
  int Leer_Sensores_Linea(boolean L);
  int proporcional();
  int C_sencero();
  int C_senuno();
  int C_sencato();
  int C_senquin();
private:
};
#endif