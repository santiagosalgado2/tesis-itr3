#include <WiFi.h>
#include <WebServer.h>
#include <IRremote.hpp>  
//LIBRERIAS USADAS
// Definir el pin del emisor IR
const int irPin = 13;

// Crear un objeto WebServer en el puerto 80
WebServer server(80);

// Datos de la red Wi-Fi
const char* ssid = "RioTel_SalgadO";
const char* password = "dcfs5411";

// Función para manejar la recepción de señales y enviarlas por IR
void handleSendIR() {
  // Verificar si hay datos en el cuerpo de la solicitud
  if (server.hasArg("plain")) {
    // Obtener el cuerpo de la solicitud (se espera que sea un string de números separados por comas)
    String body = server.arg("plain");

    // Crear un buffer para almacenar los datos procesados
    std::vector<uint16_t> rawData;
    
    // Dividir el string en partes separadas por comas
    int startIndex = 0;
    int commaIndex;
    while ((commaIndex = body.indexOf(',', startIndex)) != -1) {
      String numberStr = body.substring(startIndex, commaIndex);
      rawData.push_back(numberStr.toInt()); // Convertir a entero y agregar al vector
      startIndex = commaIndex + 1; // Avanzar al siguiente índice
    }
    // Agregar el último número (si existe)
    if (startIndex < body.length()) {
      String numberStr = body.substring(startIndex);
      rawData.push_back(numberStr.toInt());
    }

    // Verificar que el array no esté vacío
    if (rawData.size() > 0) {
      // Emitir la señal IR utilizando la señal cruda (RAW)
      IrSender.sendRaw(rawData.data(), rawData.size(), 38);  // 38 kHz de frecuencia

      // Responder al cliente
      server.send(200, "application/json", "{\"success\":\"Señal IR enviada correctamente.\"}");
    } else {
      // Enviar error si el array está vacío
      server.send(400, "application/json", "{\"error\":\"Array 'rawData' está vacío.\"}");
    }
  }
  else {
    // Enviar error si no hay datos en el cuerpo de la solicitud o parámetros necesarios
    server.send(400, "application/json", "{\"error\":\"Faltan datos en la solicitud.\"}");
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