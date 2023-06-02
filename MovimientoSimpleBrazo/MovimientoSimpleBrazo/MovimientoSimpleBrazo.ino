#include <AccelStepper.h>
#include <ArduinoJson.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

Servo Gripper;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

const int enPin = 8;

const int stepXPin = 2; // X.STEP
const int dirXPin =  5;  // X.DIR

const int stepYPin = 3; // Y.STEP
const int dirYPin = 6;  // Y.DIR

const int stepZPin = 4; // Z.STEP
const int dirZPin = 7;  // Z.DIR

const int stepAPin = 12; // Z.STEP
const int dirAPin = 13;  // Z.DIR

const int finalb = 9;
const int finalz = 11;
const int finalc = 10;
const int finalm = A3;

const float tamCaja = 2.5;

int cajasRojas = 0;
int cajasAzules = 0;
int cajasVerdes = 0;

// Definimos los motores con sus respectivos pines de pasos y de direccion
AccelStepper Z(AccelStepper::DRIVER, stepZPin, dirZPin);
AccelStepper Base(AccelStepper::DRIVER, stepXPin, dirXPin);
AccelStepper Codo(AccelStepper::DRIVER, stepYPin, dirYPin);
AccelStepper Muneca(AccelStepper::DRIVER, stepAPin, dirAPin);

boolean Homing = false;
boolean listo = false;

int inventario = 0;
int Rojas = 0;
int Azules = 0;
int Verdes = 0;

boolean PosicionInicial = false;

void crearJSON(int dato) {
  // Crear un documento JSON con una matriz y un objeto anidado
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  
  switch(dato){
    case 0:{
      array.add("Estado");
      JsonObject parametro = array.createNestedObject();
      parametro["Estado"] = "Listo";
      break;
    }
    case 1:{
      array.add("Cajas");
      JsonObject parametro = array.createNestedObject();
      parametro["Rojo"] = 1;
      break;
    }
    case 2:{
      array.add("Cajas");
      JsonObject parametro = array.createNestedObject();
      parametro["Azul"] = 1;
      break;
    }
    case 3:{
      array.add("Cajas");
      JsonObject parametro = array.createNestedObject();
      parametro["Verde"] = 1;
      break;
    }
  }
  String output;
  serializeJson(doc, output);
  Serial.println(output);
  Serial1.println(output);
  listo = true;
}

void procesarmensaje(char* json){
  StaticJsonDocument<1024> doc;
  deserializeJson(doc, json);
  String TipoMsg=doc[0];
  String Valor = doc[1];
  if(strcmp(TipoMsg.c_str(),"Inventario") == 0){
    inventario = Valor.toInt();
    posicioninicial();
  }
}

int IdentificacionColor(int R,int G, int B){

  if(R>G && R>B){
    return 1;
  }
  if(G>R && G>B){
    return 2;
  }
  if(B>R && B>G){
    return 3;
  }
  else{
    return 0;
  }
}

int LecturaDeColor(){
  uint16_t R,G,B,C;
  tcs.getRawData(&R,&G,&B,&C);//Leemos el color en el instante
  return IdentificacionColor(R,G,B);
}

void setup(){ 
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(enPin, OUTPUT);
  pinMode(finalb, INPUT_PULLUP);
  pinMode(finalz, INPUT_PULLUP);
  pinMode(finalc, INPUT_PULLUP);
  pinMode(finalm, INPUT_PULLUP);
  Gripper.attach(52);

  digitalWrite(enPin, LOW);

  if(tcs.begin()){
    Serial.println("Inicializando");
    delay(2000);
  }
  else{
    Serial.println("Error");
    Serial.println("Debe de revisar las conexiones e iniciar nuevamente");
    while(1)delay(500);
  }
}

void Home(){
  boolean finalBase  = digitalRead(finalb);
  boolean finalZ  = digitalRead(finalz);
  boolean finalCodo  = digitalRead(finalc);
  boolean finalMuneca  = digitalRead(finalm);
    // Verificar y ejecutar los movimientos en el orden correspondiente
  Base.setMaxSpeed(1500);
  Base.setAcceleration(700);
  Base.moveTo(-100000);

  Z.setMaxSpeed(2000);
  Z.setAcceleration(1000);
  Z.moveTo(-100000);

  Codo.setMaxSpeed(1500);
  Codo.setAcceleration(1000);
  Codo.moveTo(-100000);

  Muneca.setMaxSpeed(3000);
  Muneca.setAcceleration(500);
  Muneca.moveTo(-100000);
  Gripper.write(90);
  if (Z.distanceToGo() != 0 && finalZ == HIGH) {
    Z.run();
  } else if (Muneca.distanceToGo() != 0 && finalMuneca == LOW) {
    Muneca.run();
  } else if (Codo.distanceToGo() != 0 && finalCodo == LOW) {
    Codo.run();
  } else if (Base.distanceToGo() != 0 && finalBase == HIGH) {
    Base.run();
  }else {
    // Si no hay movimientos pendientes, establecer la posición actual en 0
    Z.setCurrentPosition(0);
    Base.setCurrentPosition(0);
    Codo.setCurrentPosition(0);
    Muneca.setCurrentPosition(0);
    Homing = true;
  }
}

void SerialEvent(){
  if(Serial1.available()>0){
    String Mensaje = Serial1.readStringUntil('\n');
    char* MensajeChar = new char[Mensaje.length() + 1];
    Mensaje.toCharArray(MensajeChar, Mensaje.length() + 1);
    procesarmensaje(MensajeChar);
    delete[] MensajeChar;
  }
}

void posicioninicial(){
  Base.move(8700);
  Codo.move(8700);
  while(Base.distanceToGo() != 0 || Codo.distanceToGo() != 0) {
    if(Base.distanceToGo() != 0){
      Base.run();
    }
    if(Codo.distanceToGo() != 0){
      Codo.run();
    }
  }
  PosicionInicial = true;
}

void Bajar(int Distancia){
  Z.move(1057.1*Distancia);
  while(Z.distanceToGo() != 0 ) {
    Z.run();
  }
}

void Subir(int Distancia){
  Z.move(-(1057.1*Distancia));
  while(Z.distanceToGo() != 0 ) {
    Z.run();
  }
}

void LLevarVerde(){
  Base.move(-1000);
  Codo.move(2000);
  while(Base.distanceToGo() != 0 || Codo.distanceToGo() != 0) {
    if(Base.distanceToGo() != 0){
      Base.run();
    }
    if(Codo.distanceToGo() != 0){
      Codo.run();
    }
  }
}

void DevolverVerde(){
  Base.move(1000);
  Codo.move(-2000);
  while(Base.distanceToGo() != 0 || Codo.distanceToGo() != 0) {
    if(Base.distanceToGo() != 0){
      Base.run();
    }
    if(Codo.distanceToGo() != 0){
      Codo.run();
    }
  }
}

void LLevarRojo(){
  Base.move(-700);
  Codo.move(1500);
  while(Base.distanceToGo() != 0 || Codo.distanceToGo() != 0) {
    if(Base.distanceToGo() != 0){
      Base.run();
    }
    if(Codo.distanceToGo() != 0){
      Codo.run();
    }
  }
}

void DevolverRojo(){
  Base.move(700);
  Codo.move(-1500);
  while(Base.distanceToGo() != 0 || Codo.distanceToGo() != 0) {
    if(Base.distanceToGo() != 0){
      Base.run();
    }
    if(Codo.distanceToGo() != 0){
      Codo.run();
    }
  }
}


void LLevarAzul(){
  Base.move(-400);
  Codo.move(1000);
  while(Base.distanceToGo() != 0 || Codo.distanceToGo() != 0) {
    if(Base.distanceToGo() != 0){
      Base.run();
    }
    if(Codo.distanceToGo() != 0){
      Codo.run();
    }
  }
}

void DevolverAzul(){
  Base.move(400);
  Codo.move(-1000);
  while(Base.distanceToGo() != 0 || Codo.distanceToGo() != 0) {
    if(Base.distanceToGo() != 0){
      Base.run();
    }
    if(Codo.distanceToGo() != 0){
      Codo.run();
    }
  }
}

void loop(){
  SerialEvent();
  if(Homing != true){
    Home();
  }else{
    if(listo !=true){
      crearJSON(0);
    }
    if(inventario > 0){
      Serial.println("hay inventario");
      if(PosicionInicial == true){
        switch(LecturaDeColor()){
          case 1:{//Si el color es rojo
            Serial.println("El color es Rojo");
            Bajar(15);
            Gripper.write(0);
            delay(1000);
            Subir(15);
            int ultimapos=0;
            LLevarRojo();
            Bajar(15-(tamCaja*cajasRojas));
            ultimapos= 15-(tamCaja*cajasRojas);
            Serial.print("Bajo: ");
            Serial.println(ultimapos);
            Gripper.write(90);
            delay(1000);
            inventario-=1;
            crearJSON(1);
            Subir(ultimapos);
            Serial.print("Subio: ");
            Serial.println(ultimapos);
            cajasRojas++;
            DevolverRojo();
            break;
          }
          case 2:{//Si el color es Verde
            Serial.println("El color es Verde");
            Bajar(15);
            Gripper.write(0);
            delay(1000);
            Subir(15);
            int ultimapos=0;
            LLevarVerde();
            Bajar(15-(tamCaja*cajasVerdes));
            ultimapos= 15-(tamCaja*cajasVerdes);
            Serial.print("Bajo: ");
            Serial.println(ultimapos);
            Gripper.write(90);
            delay(1000);
            inventario-=1;
            crearJSON(3);
            Subir(ultimapos);
            Serial.print("Subio: ");
            Serial.println(ultimapos);
            cajasVerdes++;
            DevolverVerde();
            break;
          }
          case 3:{//Si el color es Azul
            Serial.println("El color es Azul");
            Bajar(15);
            Gripper.write(0);
            delay(1000);
            Subir(15);
            int ultimapos=0;
            LLevarAzul();
            Bajar(15-(tamCaja*cajasAzules));
            ultimapos= 15-(tamCaja*cajasAzules);
            Serial.print("Bajo: ");
            Serial.println(ultimapos);
            Gripper.write(90);
            delay(1000);
            inventario-=1;
            crearJSON(2);
            Subir(ultimapos);
            Serial.print("Subio: ");
            Serial.println(ultimapos);
            cajasAzules++;
            DevolverAzul();
            break;
          }
        }
      }
    }else{
      Home();
    }

  }
}