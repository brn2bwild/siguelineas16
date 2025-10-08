/*Codigo de ejemplo para un seguidor de linea basado en:
 * ESP32 Nano
 * Motores pololu 6V HPCP
 * Max14870
 * Barra de 16 sensores marca Ingeniero Maker
 * Chasis para seguidor de linea en PCB marca Ingeniero Maker
 * Modulo de arranque
 * 
 * Procedimiento de Calibración
 * 1.-la barra de sensores se debe poner primero en el color blanco de la pista procurando 
 * que todos los sensores esten sobre el color blanco.
 * 
 * 2.- Presionar el boton en el PCB del robot, despues de esto ya estara calibrado el color blanco.
 * 
 * 3.-la barra de sensores se debe poner despues en el color negro de la pista procurando 
 * que todos los sensores esten sobre el color negro.
 * 
 * 4.- Presionar el boton en el PCB del robot, despues de esto ya estara calibrado el color negro.
 * 
 * 5.- Presionar de nuevo el boton para hacer el calculo de muestras.
 * el robot se encuentra listo para el arranque.
 */

#include <BarraSensores16.h>
#include <Max14870.h>

PuenteH puenteH;
BarraSensores16 barraSensores;

//Mapeo de pines
#define LEDS 48
#define GO 17
#define BOTON 7

// Constantes para PID
int p;
float kp = 0.01;   //0.01;
float kd = 1.0;    // 1.0;
float ki = 0.035;  //.006s

// Regulación de la velocidad Máxima
int velocidad_maxima = 50;  //40 //Maximo 255 nieveles
int velocidad_freno = 60;

// Data para intrgal
int error1 = 0;
int error2 = 0;
int error3 = 0;
int error4 = 0;
int error5 = 0;
int error6 = 0;
unsigned int position = 0;

//declara variables para utilizar PID
int proporcional = 0;  // Proporcional
int integral = 0;      // Integral
int derivativo = 0;    // Derivativo
int diferencial = 0;   // Diferencia aplicada a los motores
int last_prop;         // Última valor del proporcional (utilizado para calcular la derivada del error)
int target = 750;      // Setpoint (Como utilizamos 16 sensores, la línea debe estar entre 0 y 1500, por lo que el ideal es que esté en 750)

void setup() {
  puenteH.begin();

  Serial.begin(115200);
  delay(10);

  // Declaramos como salida los pines utilizados
  pinMode(LEDS, OUTPUT);
  pinMode(GO, INPUT);
  pinMode(BOTON, INPUT);

  // Calibración
  waitButton();

  digitalWrite(LEDS, HIGH);
  delay(3000);
  barraSensores.leer_blanco();  // Poner sensores en blanco
  Serial.println("Lectura de blanco correctas");
  digitalWrite(LEDS, LOW);

  waitButton();  // Presionar botón

  digitalWrite(LEDS, HIGH);
  delay(3000);
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

// Cálculo de pid y control de motores
void pid() {
  proporcional = p - 750;
  // Serial.println(proporcional);

  if (proporcional <= -target) {
    puenteH.freno(true, velocidad_freno);
  } else if (proporcional >= target) {
    puenteH.freno(false, velocidad_freno);
  }

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

  if (diferencial > velocidad_maxima) diferencial = velocidad_maxima;
  else if (diferencial < -velocidad_maxima) diferencial = -velocidad_maxima;

  (diferencial < 0) ? puenteH.motores(velocidad_maxima + diferencial, velocidad_maxima) : puenteH.motores(velocidad_maxima, velocidad_maxima - diferencial);

  // Serial.print("v max - diferencial: ");
  // Serial.print(velocidad_maxima - diferencial);
  // Serial.print(", v max + diferencial: ");
  // Serial.print(velocidad_maxima + diferencial);
  // Serial.print(", diferencial: ");
  // Serial.println(diferencial);
}
