#include <IRremote.hpp>
#include <WiFi.h>
#include <HTTPClient.h>

// Configuración del receptor IR
const int RECV_PIN = 15;
IRrecv irrecv(RECV_PIN);
IRData results;

// Configuración de la red WiFi
const char* ssid = "RioTel_SalgadO";
const char* password = "dcfs5411";

// URL de destino
const char* serverName = "http://192.168.1.119/pagina_web/pagina_web/public/recibir_codigo";
String code = "ABCD1234";

// Clase para capturar salida en un String
class StringPrint : public Print {
public:
  String buffer; // Variable para almacenar el contenido
  size_t write(uint8_t c) override {
    buffer += (char)c; // Agrega el carácter al String
    return 1;
  }
  size_t write(const uint8_t* data, size_t size) override {
    buffer += String((const char*)data).substring(0, size); // Agrega un bloque de datos
    return size;
  }
};

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(RECV_PIN); // Inicia el receptor IR

  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// Función para enviar la señal IR capturada al servidor
void sendIRCodeRaw(const String& rawDataStr) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName); // URL del servidor
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "irCode=" + rawDataStr + "&code=" + code;
    int httpResponseCode = http.POST(postData); // Envía la solicitud POST
    if (httpResponseCode > 0) {
      Serial.println("Response Code: " + String(httpResponseCode));
      Serial.println("Response: " + http.getString());
    } else {
      Serial.println("Error on sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  if (IrReceiver.decode()) {
    // Capturar la señal formateada RAW en un String
    StringPrint stringCapture;
    IrReceiver.printIRResultRawFormatted(&stringCapture, true); // Captura en StringPrint

    String rawDataStr = stringCapture.buffer; // Obtener el contenido del String
    Serial.println("Captured IR RAW Signal:");
    Serial.println(rawDataStr);

    // Enviar la señal capturada
    sendIRCodeRaw(rawDataStr);

    IrReceiver.resume(); // Reanudar el receptor
  }
}