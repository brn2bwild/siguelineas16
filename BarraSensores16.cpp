#include "Arduino.h"
#include "BarraSensores16.h"

void BarraSensores16::leer_blanco() {
  pinMode(LedsIr, OUTPUT);  //
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(LedsIr, HIGH);  // Enciendo los leds ir
  for (int i = 0; i <= 15; i++) {
    digitalWrite(S0, i & 0x01);
    digitalWrite(S1, i & 0x02);
    digitalWrite(S2, i & 0x04);
    digitalWrite(S3, i & 0x08);
    sensorValores_B[i] = analogRead(ANALOG_IN);
    //Serial.print(i);
    //Serial.print(" =");  //
    //Serial.print(sensorValores_B[i]);
    //Serial.print(" ");
  }
  //Serial.println("Valores en blanco");
}

void BarraSensores16::leer_negro() {
  pinMode(LedsIr, OUTPUT);  //
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(LedsIr, HIGH);  // Enciendo los leds ir
  for (int i = 0; i <= 15; i++) {
    digitalWrite(S0, i & 0x01);
    digitalWrite(S1, i & 0x02);
    digitalWrite(S2, i & 0x04);
    digitalWrite(S3, i & 0x08);
    sensorValores_N[i] = analogRead(ANALOG_IN);
    // Serial.print(i);  //
    //Serial.print(" =");  //
    //Serial.print(sensorValores_N[i]);  //
    //Serial.print(" ");
  }
  // Serial.println(" valores en negro");
}

void BarraSensores16::Calcula_muestras() {
  for (int i = 0; i <= 15; i++) {
    Muestras[i] = (sensorValores_B[i] + sensorValores_N[i]) / 2;
    //   Serial.print(i);  //
    //  Serial.print(" =");  //
    //   Serial.print(Muestras[i]);  //
    //   Serial.print("  ");
  }
  //   Serial.println("promedio");
}

void BarraSensores16::Leer_Sensores_Linea(boolean L) {
  pinMode(LedsIr, OUTPUT);  //
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(LedsIr, HIGH);      // Enciendo los leds ir
  for (int i = 0; i <= 15; i++) {  //Leo los 16 canales
    digitalWrite(S0, i & 0x01);
    digitalWrite(S1, i & 0x02);
    digitalWrite(S2, i & 0x04);
    digitalWrite(S3, i & 0x08);
    sensorValores[i] = analogRead(ANALOG_IN);
    if (L == 1) {
      if (sensorValores[i] < Muestras[i]) { Valor_Digital[i] = 1; }  //Convierto los valores de analogicos a digitales para linea blanca
      else {
        Valor_Digital[i] = 0;
      }
    }
    if (L == 0) {
      if (sensorValores[i] < Muestras[i]) { Valor_Digital[i] = 0; }  //Convierto los valores de analogicos a digitales para linea negra
      else {
        Valor_Digital[i] = 1;
      }
    }
  }
  x = (1500 * Valor_Digital[0] + 1400 * Valor_Digital[1] + 1300 * Valor_Digital[2] + 1200 * Valor_Digital[3] + 1100 * Valor_Digital[4] + 1000 * Valor_Digital[5] + 900 * Valor_Digital[6] + 800 * Valor_Digital[7] + 700 * Valor_Digital[8] + 600 * Valor_Digital[9] + 500 * Valor_Digital[10] + 400 * Valor_Digital[11] + 300 * Valor_Digital[12] + 200 * Valor_Digital[13] + 100 * Valor_Digital[14] + 0 * Valor_Digital[15]);

  y = (Valor_Digital[0] + Valor_Digital[1] + Valor_Digital[2] + Valor_Digital[3] + Valor_Digital[4] + Valor_Digital[5] + Valor_Digital[6] + Valor_Digital[7] + Valor_Digital[8] + Valor_Digital[9] + Valor_Digital[10] + Valor_Digital[11] + Valor_Digital[12] + Valor_Digital[13] + Valor_Digital[14] + Valor_Digital[15]);

  if (x > 0) { x1 = x; }
}

int BarraSensores16::proporcional() {
  if (y > 0) {
    p = x / y;
  } else if (y == 0 && x1 > 750) {
    p = 1500;
  } else if (y == 0 && x1 < 750) {
    p = 0;
  }

  return p;
}

int BarraSensores16::C_sencero() {
  s0 = sensorValores[0];
  return s0;
}

int BarraSensores16::C_senuno() {
  s1 = sensorValores[1];
  return s1;
}

int BarraSensores16::C_sencato() {
  s14 = sensorValores[14];
  return s14;
}

int BarraSensores16::C_senquin() {
  s15 = sensorValores[15];
  return s15;
}
