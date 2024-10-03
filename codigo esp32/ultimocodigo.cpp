#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <HTTPClient.h>

const char* serverName = "http://192.168.1.105/pagina_web/pagina_web/public/new_esp/receive";
//REEMPLAZAR POR LA IP DE LA PC HASTA QUE SE HOSTEE LA PAGINA
String readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
         baseMac[0], baseMac[1], baseMac[2],
         baseMac[3], baseMac[4], baseMac[5]);

    return macStr;
  } else {
    Serial.println("Failed to read MAC address");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();
  String macAddress=readMacAddress();
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

 String espIp = WiFi.localIP().toString();

  if (WiFi.status() == WL_CONNECTED) {
    // Crea un cliente HTTP
    HTTPClient http;
    
    // Establece la URL del servidor

    
    
    // Envía la solicitud HTTP GET
    String serverPath = String(serverName) + "?macAddress=" + macAddress + "&ipAddress=" + espIp;
    http.begin(serverPath);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.GET();

    

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        } else {
            Serial.print("Error en la petición: ");
            Serial.println(httpResponseCode);
        }
        http.end();
  }
}
void loop() {
  // Aquí puedes añadir el código que necesites ejecutar continuamente
}











