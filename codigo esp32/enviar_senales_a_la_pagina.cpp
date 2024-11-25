#include <IRremote.hpp>
#include <WiFi.h>
#include <HTTPClient.h>

// Configuración del receptor IR
const int RECV_PIN = 15; 
IRrecv irrecv(RECV_PIN);
IRData results;

// Configuración de la red WiFi
const char* ssid = "placa";
const char* password = "12345678";

// URL de destino
const char* serverName = "http://192.168.2.119/pagina_web/pagina_web/public/recibir_codigo";
String code="ABCD1234";

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
    String postData = "irCode=" + String(irCode, HEX) + "&code=" + code;
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