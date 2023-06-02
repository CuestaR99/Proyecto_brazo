#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>

SocketIOclient socketIO;

SoftwareSerial SerialDos(D1,D2);

void EnviarDatos(char* json){
  // Crear un documento JSON con una matriz y un objeto anidado
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("dato");
  JsonObject parametro = array.createNestedObject();
  parametro["Maquina"] = "H900A";
  String output;
  serializeJson(doc, output);
  socketIO.sendEVENT(output);
}

void procesarmensaje(char* Mensaje){
  StaticJsonDocument<1024> doc;
  String Msg = Mensaje;
  deserializeJson(doc, Mensaje);
  String propiedad = doc[0];
  String valor=doc[1];
  if(strcmp(propiedad.c_str(),"Estado")== 0){
    socketIO .sendEVENT(Msg);
  }

  if(strcmp(propiedad.c_str(),"Cajas")== 0){
    socketIO .sendEVENT(Msg);
  }

  if(strcmp(propiedad.c_str(),"Inventario")== 0){
    SerialDos.println(Msg);
  }
}

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length){
  switch(type) {
    // Si se desconecta del servidor, detener el refresco de estados y volver a intentar conectarse.
    case sIOtype_DISCONNECT:
      Serial.println("======== ESP DESCONECTADO DEL SERVIDOR ========");   
      break;

    // Si se conecta al servidor, iniciar el refresco de estados y solicitar datos iniciales.
    case sIOtype_CONNECT:
      Serial.println("\n======== ESP CONECTADO AL SERVIDOR ========");

      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      // Iniciar el proceso de refresco de estados llamando directamente a la función de refresco en lugar de usar una variable de función.
      break;

    // Si se recibe un evento del servidor, procesar el mensaje.
    case sIOtype_EVENT:{
      Serial.printf("%s\n",payload);
      procesarmensaje((char*)payload);
    }
  }
}

void setup(){
  Serial.begin(9600);
  SerialDos.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin("Richard", "12345678");
  Serial.println("Conectando al wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  

  socketIO.begin("192.168.43.91",5000, "/socket.io/?EIO=4");
  // event handler
  socketIO.onEvent(socketIOEvent);
}

void SerialEvent (){

  if(SerialDos.available() > 0 ){
    String Mensaje = SerialDos.readStringUntil('\n');
    char* MensajeChar = new char[Mensaje.length() + 1];
    Mensaje.toCharArray(MensajeChar, Mensaje.length() + 1);
    procesarmensaje(MensajeChar);
    delete[] MensajeChar;
  }
}

void loop(){
  SerialEvent ();
  if(WiFi.status()==WL_CONNECTED){
    if(!socketIO.isConnected()){
      Serial.println("Sin Conexion Al Servidor");
    }
    socketIO.loop(); 
  }else{
    Serial.println("Sin Conexion a Internet");
  }
}
