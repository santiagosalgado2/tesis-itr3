#include <WiFi.h>
#include <WebServer.h>
#include <IRremote.hpp>

const char* ssid = "ITR3-6"; // Cambia por tu SSID
const char* password = "archivado"; // Cambia por tu contraseña
const int irPin = 15; // Pin al que está conectado el LED IR

WebServer server(80); // Crea un servidor en el puerto 80

void setup() {
  Serial.begin(115200); // Iniciar la comunicación serial
  IrSender.begin(irPin); // Inicializar el emisor IR con el pin especificado
  WiFi.begin(ssid, password); // Conectar a Wi-Fi
  
  // Esperar hasta que se conecte
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  
  // Imprimir la dirección IP
  Serial.print("Conectado a WiFi. Dirección IP: ");
  Serial.println(WiFi.localIP()); // Mostrar la dirección IP en el monitor serie

  // Configurar la ruta del servidor
  server.on("/sendIR", HTTP_GET, sendIR); // Al acceder a esta ruta se ejecutará sendIR
  server.begin(); // Iniciar el servidor
}

void loop() {
  server.handleClient(); // Manejar las peticiones del cliente
}

void sendIR() {
  Serial.println("Enviando señales RC6...");
  
  for (int i = 0; i < 3; i++) {
    IrSender.sendRC6((uint32_t)0xC, 4); 
    delay(40); // Espera 40 ms antes de la siguiente señal
    IrSender.sendRC6((uint32_t)0x1000C, 20); 
    delay(1000); // Espera 40 ms antes de la siguiente señal
    IrSender.sendRC6((uint32_t)0x1000C, 20);  
    delay(40); // Espera 40 ms antes de la siguiente señal
    IrSender.sendRC6((uint32_t)0xC, 4); 
    delay(1000); // Espera 40 ms antes de la siguiente señal
  }


  
  server.send(200, "text/plain", "Señales IR enviadas");
}
