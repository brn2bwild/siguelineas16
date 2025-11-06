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

#include <BarraSensores16.h>
#include <Max14870.h>

/* Mapeo de pines */
#define LEDS 48
#define GO 17
#define BUTTON 7

// #define DEBUG /* Para debuggear el código descomentamos esta línea */

/* Constantes para PID */
#define KP 2.9        // 0.01;
#define KI 0.0        //.006s
#define KD 24.1       // 1.0;
#define SETPOINT 750  // Setpoint (Como utilizamos 16 sensores, la línea debe estar entre 0 y 1500, por lo que el ideal es que esté en 750)

/* Regulación de la velocidad Máxima */
#define MAX_SPEED 600  //Máximo 1023 nieveles
#define MIN_SPEED (-1) * MAX_SPEED
#define BRAKE_SPEED 800

/* constante para valor máximo del PWM */
#define MAX_PWM_VALUE 1023

/* Variable para guardar el valor de la posición */
// int p;

/* Variables para las velocidades de los motores */
int left_motor_speed, right_motor_speed;

/* Variables para calcular la integral */
// int error1, error2, error3, error4, error5, error6;

/* Variables para utilizar en el PID */
int error = 0;       // Proporcional
int integral = 0;    // Integral
int derivative = 0;  // derivative
int last_error;      // Última valor del proporcional (utilizado para calcular la derivada del error)

PuenteH puenteH;
BarraSensores16 barraSensores;

void setup() {
  puenteH.begin();

#ifdef DEBUG
  Serial.begin(115200);
  delay(10);
#endif

  /* Declaramos como salida los pines utilizados */
  pinMode(LEDS, OUTPUT);
  pinMode(GO, INPUT);
  pinMode(BUTTON, INPUT);

  /* Iniciamos el proceso de calibración */
  waitButton();

  digitalWrite(LEDS, HIGH);
  delay(2000);
  barraSensores.leer_blanco();  // Poner sensores en blanco
  Serial.println("Lectura de blanco correctas");
  digitalWrite(LEDS, LOW);

  waitButton();  // Presionar botón

  digitalWrite(LEDS, HIGH);
  delay(2000);
  barraSensores.leer_negro();  // Poner sensores en negro
  Serial.println("Lectura de negro correctas");
  digitalWrite(LEDS, LOW);

  waitButton();  // Presionar botón

  digitalWrite(LEDS, HIGH);
  barraSensores.Calcula_muestras();  // Calcular las muestras
  for (int i = 0; i < 5; i++) {
    digitalWrite(LEDS, HIGH);
    delay(100);
    digitalWrite(LEDS, LOW);
    delay(100);
  }

  waitGo();  // Control de arranque

  digitalWrite(LEDS, LOW);
}

void loop() {
  while (digitalRead(GO)) {
    barraSensores.Leer_Sensores_Linea(0);

    pid(barraSensores.proporcional());
  }

  puenteH.motores(0, 0);
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
void pid(int p) {
  error = SETPOINT - p;

#ifndef DEBUG
  if (error <= -SETPOINT) {
    puenteH.motores(-BRAKE_SPEED, BRAKE_SPEED);
  } else if (error >= SETPOINT) {
    puenteH.motores(BRAKE_SPEED, -BRAKE_SPEED);
  }
#endif
  // integral = error1 + error2 + error3 + error4 + error5 + error6;

  // error6 = error5;
  // error5 = error4;
  // error4 = error3;
  // error3 = error2;
  // error2 = error1;
  // error1 = error;

  // integral += (KI * error);

  // integral = constrain(integral, 0, MAX_PWM_VALUE);

  derivative = error - last_error;

  int diff = (KP * error) + (KD * derivative);

  last_error = error;

  left_motor_speed = constrain(MAX_SPEED - diff, MIN_SPEED, MAX_SPEED);
  right_motor_speed = constrain(MAX_SPEED + diff, MIN_SPEED, MAX_SPEED);

#ifndef DEBUG
  puenteH.motores(left_motor_speed, right_motor_speed);
#endif

#ifdef DEBUG
  Serial.print("error: ");
  Serial.print(error);
  Serial.print(", diff: ");
  Serial.print(diff);
  Serial.print(", left speed: ");
  Serial.print(left_motor_speed);
  Serial.print(", right speed: ");
  Serial.println(right_motor_speed);
#endif
}
