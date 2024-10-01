#include <WiFi.h>
#include <esp_smartconfig.h>

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
}

void loop() {
  // Aquí puedes añadir el código que necesites ejecutar continuamente
}


