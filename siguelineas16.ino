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

#include "index.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ESP32Servo.h>
#include "BarraSensores16.h"
#include "Max14870.h"

/* Mapeo de pines */
#define LEDS 48
#define LED_RED 46
#define LED_GREEN 0
#define LED_BLUE 45
#define GO 17
#define BUTTON 7
#define BUTTON_CALIBRACION 10
#define PIN_ESC 21
#define PIN_RDY 47

// #define DEBUG /* Para debuggear el código descomentamos esta línea */

/* Constants for WiFi config*/
const char* SSID = "9klf";
const char* PASS = "0123456789";

const char* PARAM_INPUT_KP = "kp";
const char* PARAM_INPUT_KI = "ki";
const char* PARAM_INPUT_KD = "kd";
const char* PARAM_INPUT_MAX_SPEED = "max_speed";
const char* PARAM_INPUT_MAX_ESC = "max_esc";

/* Constantes para PID */
// const float KP = 0.095;     //0.095;      // 0.01;
// const float KI = 0.0;       //.006s
// const float KD = 0.0;       // 1.0;
const int SETPOINT = 7500;  // Setpoint (Como utilizamos 16 sensores, la línea debe estar entre 0 y 1500, por lo que el ideal es que esté en 750)

/* Regulación de la velocidad Máxima */
// const int MAX_SPEED = 100;  //Máximo 1023 nieveles
// const int MIN_SPEED = -1 * MAX_SPEED;
const int BRAKE_SPEED = 1020;  //200

/* constante para valor máximo del diff */
// const int MAX_DIFF = 1000;

/* Variable para guardar el valor de la posición */
// int input;

/**/
float kp = 0.0, ki = 0.0, kd = 0.0;

/* Variables para las velocidades de los motores */
// int left_motor_speed, right_motor_speed;
int max_percent_speed, min_speed, max_speed, max_percent_esc, max_esc, left_motor_speed, right_motor_speed;

/* Variables para calcular la integral */
int error1 = 0, error2 = 0, error3 = 0, error4 = 0, error5 = 0, error6 = 0;

/* Variables para utilizar en el PID */
// int error = 0;     // Proporcional
float integral = 0;  // Integral
// int derivative = 0;  // derivative
float last_error;  // Última valor del proporcional (utilizado para calcular la derivada del error)

PuenteH puenteH;
BarraSensores16 barraSensores;
Servo esc;

AsyncWebServer server(80);
Preferences preferences;

String processor(const String& var) {
  if (var == "KP") {
    return String(preferences.getFloat("kp", 0.0),5);
  } else if (var == "KI") {
    return String(preferences.getFloat("ki", 0.0),6);
  } else if (var == "KD") {
    return String(preferences.getFloat("kd", 0.0),5);
  } else if (var == "MAX_SPEED") {
    return String(preferences.getInt("max_speed", 0));
  } else if (var == "MAX_ESC") {
    return String(preferences.getInt("max_esc", 0));
  }

  return String();
}

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "NOT FOUND");
}

void setup() {
  /* Declaramos como salida los pines utilizados */
  pinMode(LEDS, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  pinMode(GO, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(PIN_RDY, INPUT);
  pinMode(BUTTON_CALIBRACION, INPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  esc.attach(PIN_ESC, 1000, 2000);
  delay(100);
  esc.writeMicroseconds(2000);
  delay(1000);
  esc.writeMicroseconds(1000);
  delay(1000);
  esc.writeMicroseconds(1000);

#ifdef DEBUG
  Serial.begin(115200);
#endif

  Serial.println("Initing data storage");
  preferences.begin("data", false);
  Serial.println("Initing data storage done");

  Serial.println("Reading data from storage");
  kp = preferences.getFloat("kp", 0.0);
  ki = preferences.getFloat("ki", 0.0);
  kd = preferences.getFloat("kd", 0.0);

  max_percent_speed = preferences.getInt("max_speed", 0);
  max_speed = map(max_percent_speed, 0, 100, 0, 1022);
  min_speed = (-1) * max_speed;

  max_percent_esc = preferences.getInt("max_esc", 0);
  max_esc = map(max_percent_esc, 0, 100, 1000, 2000);
  Serial.println("Reading data from storage done");

  Serial.print("Initing AP: ");
  Serial.println(SSID);
  WiFi.softAP(SSID, PASS);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  Serial.println("Initing server");
  server.onNotFound(notFound);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", index_html, processor);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest* request) {
    String inputMessage;

    if (request->hasParam(PARAM_INPUT_KP)) {
      inputMessage = request->getParam(PARAM_INPUT_KP)->value();
      kp = inputMessage.toFloat();
      preferences.putFloat("kp", kp);
    }

    if (request->hasParam(PARAM_INPUT_KI)) {
      inputMessage = request->getParam(PARAM_INPUT_KI)->value();
      ki = inputMessage.toFloat();
      preferences.putFloat("ki", ki);
    }

    if (request->hasParam(PARAM_INPUT_KD)) {
      inputMessage = request->getParam(PARAM_INPUT_KD)->value();
      kd = inputMessage.toFloat();
      preferences.putFloat("kd", kd);
    }

    if (request->hasParam(PARAM_INPUT_MAX_SPEED)) {
      inputMessage = request->getParam(PARAM_INPUT_MAX_SPEED)->value();
      max_percent_speed = inputMessage.toInt();
      max_speed = map(max_percent_speed, 0, 100, 0, 1022);
      min_speed = (-1) * max_speed;
      preferences.putInt("max_speed", max_percent_speed);
    }

    if (request->hasParam(PARAM_INPUT_MAX_ESC)) {
      inputMessage = request->getParam(PARAM_INPUT_MAX_ESC)->value();
      max_percent_esc = inputMessage.toInt();
      max_esc = map(max_percent_esc, 0, 100, 1000, 2000);
      preferences.putInt("max_esc", max_percent_esc);
    }

    request->redirect("/");
  });
  server.begin();

  Serial.println("esperando a seleccionar modo");
  bool modoCalibracion=false;
  bool modoSeleccionado=false;
  while(!modoSeleccionado){
    digitalWrite(LED_BLUE, LOW);
    delay(100);
    digitalWrite(LED_BLUE, HIGH);
    delay(100);
    if (!digitalRead(BUTTON)){
      modoCalibracion=false;
      modoSeleccionado=true;
    }
    if (!digitalRead(BUTTON_CALIBRACION)) {
      modoCalibracion = true;
      modoSeleccionado = true;
    }
  }
  delay(300);
  if (modoCalibracion){
    Serial.println("Modo calibracion");
    waitButton();
    digitalWrite(LED_RED, LOW);
    delay(2000);
    barraSensores.leer_blanco();
    Serial.println("Blanco calibrado");
    digitalWrite(LED_RED, HIGH);

    //Calibrar NEGRO
    waitButton();
    digitalWrite(LED_RED, LOW);
    delay(2000);
    barraSensores.leer_negro();
    Serial.println("Negro calibrado");
    digitalWrite(LED_RED, HIGH);

    //Guardar en Preferences
    preferences.putBytes("cal_blanco",
                         barraSensores.sensorValores_B,
                         sizeof(barraSensores.sensorValores_B));
    preferences.putBytes("cal_negro",
                         barraSensores.sensorValores_N,
                         sizeof(barraSensores.sensorValores_N));
    Serial.println("Calibración guardada en memoria");
  }
  else{
    Serial.println("MODO NORMAL");

    preferences.getBytes("cal_blanco",
                         barraSensores.sensorValores_B,
                         sizeof(barraSensores.sensorValores_B));
    preferences.getBytes("cal_negro",
                         barraSensores.sensorValores_N,
                         sizeof(barraSensores.sensorValores_N));
    Serial.println("Calibración cargada");
  }
  waitButton();  // Presionar botón

  barraSensores.Calcula_muestras();  

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

  delay(1000);

  esc.writeMicroseconds(1000);
  digitalWrite(LED_GREEN, HIGH);
}

// Esperar por módulo de arranque
void waitGo() {
  for (int i = 1000; i <= max_esc; i++) {
    esc.writeMicroseconds(i);
    delay(1);
  }
  while (!digitalRead(GO)) {
    digitalWrite(LED_GREEN, LOW);
    delay(100);
    digitalWrite(LED_GREEN, HIGH);
    delay(100);
  }
}


// Esperar por el botón
void waitButton() {
  while (digitalRead(BUTTON)) {
    digitalWrite(LED_BLUE, LOW);
    delay(100);
    digitalWrite(LED_BLUE, HIGH);
    delay(100);
  }
}

// /* Cálculo de pid y control de motores */
// void pid() {
//   uint16_t input = barraSensores.Leer_Sensores_Linea(0);

//   float error = input - SETPOINT;

// #ifndef DEBUG
//   if (error >= SETPOINT) {
//     puenteH.motores(BRAKE_SPEED, -BRAKE_SPEED);
//   } else if (error <= -SETPOINT) {
//     puenteH.motores(-BRAKE_SPEED, BRAKE_SPEED);
//   }
// #endif

//   error6 = error5;
//   error5 = error4;
//   error4 = error3;
//   error3 = error2;
//   error2 = error1;
//   error1 = error;
//   integral = ki * (error6 + error5 + error4 + error3 + error2 + error1 + error);

//   integral = constrain(integral, min_speed, max_speed);

//   float derivative = kd * (error - last_error);
//   last_error = error;

//   float diff = (kp * error) + integral + derivative;

//   left_motor_speed = constrain(max_speed + diff, min_speed, max_speed);
//   right_motor_speed = constrain(max_speed - diff, min_speed, max_speed);

// #ifndef DEBUG
//   (diff >= 0) ? puenteH.motores(max_speed, right_motor_speed) : puenteH.motores(left_motor_speed, max_speed);
// #endif

/* Cálculo de pid y control de motores */
void pid() {
  uint16_t input = barraSensores.Leer_Sensores_Linea(0);

  float error = input - SETPOINT;

#ifndef DEBUG
  if (error >= SETPOINT) {
    puenteH.motores(BRAKE_SPEED, -BRAKE_SPEED);
  } else if (error <= -SETPOINT) {
    puenteH.motores(-BRAKE_SPEED, BRAKE_SPEED);
  }
#endif

  error6 = error5;
  error5 = error4;
  error4 = error3;
  error3 = error2;
  error2 = error1;
  error1 = error;
  integral = ki * (error6 + error5 + error4 + error3 + error2 + error1 + error);

  integral = constrain(integral, min_speed, max_speed);

  float derivative = kd * (error - last_error);
  last_error = error;

  float diff = (kp * error) + integral + derivative;

  //Codigo de velocidad Adaptativa
  float desviacion=constrain(abs(error)/SETPOINT, 0.0, 1.0);
  float speed_factor=1.0-(0.5*desviacion);
  int vel_adaptativa=(int)(max_speed*speed_factor);

  // vel_adaptativa es la base, diff corrige la dirección
  left_motor_speed = constrain(vel_adaptativa + diff, min_speed, max_speed);
  right_motor_speed = constrain(vel_adaptativa - diff, min_speed, max_speed);

#ifndef DEBUG
  (diff >= 0) ? puenteH.motores(vel_adaptativa, right_motor_speed) 
              : puenteH.motores(left_motor_speed, vel_adaptativa);
#endif

#ifdef DEBUG
  Serial.print("err: ");
  Serial.print(error);
  Serial.print(", diff: ");
  Serial.print(diff);
  Serial.print(", ls: ");
  Serial.print(left_motor_speed);
  Serial.print(", V_Adap: ");
  Serial.print(vel_adaptativa);
  Serial.print(", rs: ");
  Serial.print(right_motor_speed);
  Serial.print(", v_Adap: ");
  Serial.print(vel_adaptativa);
  Serial.print(", kp: ");
  Serial.print(kp);
  Serial.print(", ki: ");
  Serial.print(ki);
  Serial.print(", kd: ");
  Serial.print(kd);
  Serial.print(", ms: ");
  Serial.print(max_speed);
  Serial.print(", mesc: ");
  Serial.print(max_esc);
  Serial.println("");
  delay(100);
#endif
}