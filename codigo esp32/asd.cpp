#include <WiFi.h>
#include <esp_smartconfig.h>
#include <HTTPClient.h>

const char* serverName='http://localhost/pagina_web/pagina_web/public/new_esp/insert'

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Iniciando SmartConfig...");
  WiFi.beginSmartConfig();

  // Espera hasta que SmartConfig esté completado
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("SmartConfig completado.");
  Serial.printf("Conectado a %s\n", WiFi.SSID().c_str());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (WiFi.status() == WL_CONNECTED) {
    // Crea un cliente HTTP
    HTTPClient http;
    
    // Establece la URL del servidor
    http.begin(serverName);
    
    // Envía la solicitud HTTP GET
    int httpResponseCode = http.GET();
    
    // Verifica el código de respuesta del servidor
    if (httpResponseCode > 0) {
      Serial.printf("Solicitud enviada. Código de respuesta: %d\n", httpResponseCode);
      String payload = http.getString();
      Serial.println("Respuesta del servidor:");
      Serial.println(payload);
    } else {
      Serial.printf("Error al realizar la solicitud HTTP. Código: %d\n", httpResponseCode);
    }

    // Finaliza la conexión HTTP
    http.end();
  } else {
    Serial.println("Error: No conectado a WiFi");
  }
}

void loop() {
  // Aquí puedes añadir el código que necesites ejecutar continuamente
}


