/*
Codigo de ejemplo para un seguidor de línea basado en:
 - ESP32 Nano
 - Motores pololu 6V HPCP
 - Max14870
 - Barra de 16 sensores marca Ingeniero Maker
 - Chasis para seguidor de linea en PCB marca Ingeniero Maker
 - Modulo de arranque

Procedimiento de calibración
1.- La barra de sensores se debe poner primero en el color blanco de la pista procurando que todos los sensores esten sobre el color blanco.

2.- Presionar el botón en el PCB del robot, después de esto ya estará calibrado el color blanco.

3.- La barra de sensores se debe poner después en el color negro de la pista procurando que todos los sensores esten sobre el color negro.

4.- Presionar el botón en el PCB del robot, despues de esto ya estará calibrado el color negro.

5.- Presionar de nuevo el botón para hacer el cálculo de muestras, el robot se encuentra listo para el arranque.
*/

#include "BarraSensores16.h"
#include "Max14870.h"

/* Mapeo de pines */
#define LEDS 48
#define LED_RED 46
#define LED_GREEN 0
#define LED_BLUE 45
#define GO 17
#define BUTTON 7

// #define DEBUG /* Para debuggear el código descomentamos esta línea */

/* Constantes para PID */
const float KP = 0.065;      // 0.01;
const float KI = 0.008;     //.006s
const float KD = 0.9;       // 1.0;
const int SETPOINT = 3750;  // Setpoint (Como utilizamos 16 sensores, la línea debe estar entre 0 y 1500, por lo que el ideal es que esté en 750)

/* Regulación de la velocidad Máxima */
const int MAX_SPEED = 130;  //Máximo 1023 nieveles
const int MIN_SPEED = -1 * MAX_SPEED;
const int BRAKE_SPEED = 200;

/* constante para valor máximo del diff */
const int MAX_DIFF = 1000;

/* Variable para guardar el valor de la posición */
// int input;

/* Variables para las velocidades de los motores */
int left_motor_speed, right_motor_speed;

/* Variables para calcular la integral */
// int error1, error2, error3, error4, error5, error6;

/* Variables para utilizar en el PID */
int error = 0;       // Proporcional
int integral = 0;    // Integral
int derivative = 0;  // derivative
int last_input;      // Última valor del proporcional (utilizado para calcular la derivada del error)

PuenteH puenteH;
BarraSensores16 barraSensores;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  /* Declaramos como salida los pines utilizados */
  pinMode(LEDS, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(GO, INPUT);
  pinMode(BUTTON, INPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  /* Iniciamos el proceso de calibración */
  waitButton();

  digitalWrite(LED_RED, LOW);
  delay(2000);
  barraSensores.leer_blanco();  // Poner sensores en blanco
  Serial.println("Lectura de blanco correctas");
  digitalWrite(LED_RED, HIGH);

  waitButton();  // Presionar botón

  digitalWrite(LED_RED, LOW);
  delay(2000);
  barraSensores.leer_negro();  // Poner sensores en negro
  Serial.println("Lectura de negro correctas");
  digitalWrite(LED_RED, HIGH);

  waitButton();  // Presionar botón

  digitalWrite(LED_BLUE, LOW);
  barraSensores.Calcula_muestras();  // Calcular las muestras
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BLUE, LOW);
    delay(100);
    digitalWrite(LED_BLUE, HIGH);
    delay(100);
  }

#ifndef DEBUG
  waitGo();  // Control de arranque
#endif

  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, LOW);
}

void loop() {
  while (digitalRead(GO)) {
    pid();
  }
  puenteH.motores(0, 0);

  digitalWrite(LED_GREEN, LOW);
}

// Esperar por módulo de arranque
void waitGo() {
  while (!digitalRead(GO)) {}
}

// Esperar por el botón
void waitButton() {
  while (digitalRead(BUTTON)) {}
}

/* Cálculo de pid y control de motores */
void pid() {
  uint16_t input = barraSensores.Leer_Sensores_Linea(0);

  error = input - SETPOINT;

#ifndef DEBUG
  if (error >= SETPOINT) {
    puenteH.motores(BRAKE_SPEED, -BRAKE_SPEED);
  } else if (error <= -SETPOINT) {
    puenteH.motores(-BRAKE_SPEED, BRAKE_SPEED);
  }
#endif

  // integral = error1 + error2 + error3 + error4 + error5 + error6;
  // error6 = error5;
  // error5 = error4;
  // error4 = error3;
  // error3 = error2;
  // error2 = error1;
  // error1 = error;
  integral += error;
  integral = constrain(integral, -1000, 1000);

  derivative = input - last_input;

  int diff = (KP * error) + (KI * integral) + (KD * derivative);

  last_input = input;

  diff = constrain(diff, -255, 255);

  left_motor_speed = constrain(MAX_SPEED + diff, MIN_SPEED, MAX_SPEED);
  right_motor_speed = constrain(MAX_SPEED - diff, MIN_SPEED, MAX_SPEED);

#ifndef DEBUG
  (diff >= 0) ? puenteH.motores(MAX_SPEED, right_motor_speed) : puenteH.motores(left_motor_speed, MAX_SPEED);
#endif

#ifdef DEBUG
  if (error >= SETPOINT) {
    Serial.printf("error: %d, diff: %d, l-speed: %d, r-speed: %d\n", error, diff, BRAKE_SPEED, -BRAKE_SPEED);
  } else if (error <= -SETPOINT) {
    Serial.printf("error: %d, diff: %d, l-speed: %d, r-speed: %d\n", error, diff, -BRAKE_SPEED, BRAKE_SPEED);
  } else if (diff >= 0) {
    Serial.printf("error: %d, diff: %d, l-speed: %d, r-speed: %d\n", error, diff, MAX_SPEED, right_motor_speed);
  } else if (diff < 0) {
    Serial.printf("error: %d, diff: %d, l-speed: %d, r-speed: %d\n", error, diff, left_motor_speed, MAX_SPEED);
  }
  delay(100);
#endif
}
