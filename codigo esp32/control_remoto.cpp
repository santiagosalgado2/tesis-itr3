#include <WiFi.h>
#include <WebServer.h>
#include <IRremote.hpp>  // Incluimos la librería IRremote.hpp

// Definir el pin del emisor IR
const int irPin = 13;

// Crear un objeto WebServer en el puerto 80
WebServer server(80);

// Credenciales de la red Wi-Fi
const char* ssid = "RioTel_SalgadO";
const char* password = "dcfs5411";

// Función para manejar la recepción de señales y enviarlas por IR
void handleSendIR() {
  if (server.hasArg("signal1") && server.hasArg("signal2")) {
    // Lee los parámetros recibidos desde el cliente (signal1 y signal2)
    String signal1Str = server.arg("signal1");
    String signal2Str = server.arg("signal2");

    // Convertir las señales a valores numéricos (hexadecimales)
    uint32_t signal1 = strtoul(signal1Str.c_str(), NULL, 16);
    uint32_t signal2 = strtoul(signal2Str.c_str(), NULL, 16);

    // Enviar la primera señal IR con protocolo RC6
    IrSender.sendRC6(signal1, 32);  // Enviar 32 bits con RC6
    delay(1000);  // Pausa entre las señales

    // Enviar la segunda señal IR con protocolo RC6
    IrSender.sendRC6(signal2, 32);  // Enviar 32 bits con RC6

    // Responder al cliente
    server.send(200, "text/plain", "Señales IR enviadas correctamente.");
  } else {
    // Enviar error si los parámetros no están presentes
    server.send(400, "text/plain", "Faltan los parámetros signal1 o signal2.");
  }
}

// Configurar Wi-Fi
void setupWiFi() {
  delay(100);
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Configurar el pin del emisor IR
  IrSender.begin(irPin);  // Solo el pin del emisor IR es necesario

  // Configurar el Wi-Fi
  setupWiFi();

  // Definir la ruta para manejar las solicitudes IR
  server.on("/sendIR", handleSendIR);

  // Iniciar el servidor web
  server.begin();
  Serial.println("Servidor iniciado.");
}

void loop() {
  // Manejar las solicitudes del servidor
  server.handleClient();
}