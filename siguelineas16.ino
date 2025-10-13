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

2.- Presionar el boton en el PCB del robot, después de esto ya estará calibrado el color blanco.

3.- La barra de sensores se debe poner después en el color negro de la pista procurando que todos los sensores esten sobre el color negro.

4.- Presionar el boton en el PCB del robot, despues de esto ya estará calibrado el color negro.

5.- Presionar de nuevo el boton para hacer el cálculo de muestras, el robot se encuentra listo para el arranque.

*/

#include <BarraSensores16.h>
#include <Max14870.h>

PuenteH puenteH;
BarraSensores16 barraSensores;

// #define DEBUG /* Para debuggear el código descomentamos esta línea */

/* constante para el diferencial */
const int diferencial_maximo = 1023;

/* Constantes para PID */
const float kp = 0.2;    //0.01;
const float kd = 1.3;    // 1.0;
const float ki = 0.016;  //.006s

/* Variable para guardar el valor de la posición*/
int p;

/* Regulación de la velocidad Máxima */
const int velocidad_maxima = 400;  //Máximo 1023 nieveles
const int velocidad_freno = 400;

int velocidad_motor_izquierdo = 0;
int velocidad_motor_derecho = 0;

/* Variables para calcular la integral */
int error1 = 0;
int error2 = 0;
int error3 = 0;
int error4 = 0;
int error5 = 0;
int error6 = 0;

/* Variables para utilizar en el PID */
const int target = 750;  // Setpoint (Como utilizamos 16 sensores, la línea debe estar entre 0 y 1500, por lo que el ideal es que esté en 750)
int proporcional = 0;    // Proporcional
int integral = 0;        // Integral
int derivativo = 0;      // Derivativo
int diferencial = 0;     // Diferencia aplicada a los motores
int last_prop;           // Última valor del proporcional (utilizado para calcular la derivada del error)

/* Mapeo de pines */
const uint8_t LEDS = 48;
const uint8_t GO = 17;
const uint8_t BOTON = 7;

void setup() {
  puenteH.begin();

#ifdef DEBUG
  Serial.begin(115200);
  delay(10);
#endif

  /* Declaramos como salida los pines utilizados */
  pinMode(LEDS, OUTPUT);
  pinMode(GO, INPUT);
  pinMode(BOTON, INPUT);

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

    p = barraSensores.proporcional();

    pid();
  }

  puenteH.motores(0, 0);
}

// Esperar por módulo de arranque
void waitGo() {
  while (!digitalRead(GO)) {}
}

// Esperar por el botón
void waitButton() {
  while (digitalRead(BOTON)) {}
}

/* Cálculo de pid y control de motores */
void pid() {
  proporcional = p - 750;

  // if (proporcional <= -target) {
  //   puenteH.motorDer(velocidad_maxima);
  //   puenteH.freno(true, velocidad_freno);
  // } else if (proporcional >= target) {
  //   puenteH.motorIzq(velocidad_maxima);
  //   puenteH.freno(false, velocidad_freno);
  // }

  derivativo = proporcional - last_prop;

  integral = error1 + error2 + error3 + error4 + error5 + error6;

  last_prop = proporcional;

  error6 = error5;
  error5 = error4;
  error4 = error3;
  error3 = error2;
  error2 = error1;
  error1 = proporcional;

  diferencial = (proporcional * kp) + (derivativo * kd) + (integral * ki);

  velocidad_motor_izquierdo = constrain(velocidad_maxima + diferencial, -velocidad_maxima, velocidad_maxima);
  velocidad_motor_derecho = constrain(velocidad_maxima - diferencial, -velocidad_maxima, velocidad_maxima);

  /* Descomentar esta línea para activar los motores*/
#ifndef DEBUG
  puenteH.motores(velocidad_motor_izquierdo, velocidad_motor_derecho);
#endif

#ifdef DEBUG
  Serial.print("proporcional: ");
  Serial.print(proporcional);
  Serial.print(", diferencial: ");
  Serial.print(diferencial);
  Serial.print(", vel izq: ");
  Serial.print(velocidad_motor_izquierdo);
  Serial.print(", vel der: ");
  Serial.println(velocidad_motor_derecho);
#endif
}
