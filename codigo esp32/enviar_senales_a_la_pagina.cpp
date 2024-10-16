#include <IRremote.hpp>
#include <WiFi.h>
#include <HTTPClient.h>

// Configuración del receptor IR
const int RECV_PIN = 26; // Cambia esto al pin que estás usando
IRrecv irrecv(RECV_PIN);
IRData results;

// Configuración de la red WiFi
const char* ssid = "ITR3-6";
const char* password = "archivado";

// URL de destino
const char* serverName = "http://192.168.2.151/pagina_web/pagina_web/public/recibir_codigo";

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn(); // Inicia el receptor IR

  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void sendIRCode(uint32_t irCode) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "irCode=" + String(irCode, HEX);
    int httpResponseCode = http.POST(postData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  if (irrecv.decode()) {
    Serial.print("IR Code Received: ");
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    sendIRCode(irrecv.decodedIRData.decodedRawData);
    irrecv.resume(); // Recibe el siguiente valor
  }
}