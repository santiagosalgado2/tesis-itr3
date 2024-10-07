#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <HTTPClient.h>

const char* serverName = "http://192.168.1.105/pagina_web/pagina_web/public/new_esp/receive";
// REEMPLAZAR POR LA IP DE LA PC HASTA QUE SE HOSTEE LA PAGINA
String code = "8lIsgR9J";

void setup() {
  Serial.begin(115200);
  
  // Intentar conectar automáticamente a la última red conocida
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin();

  int retries = 0;
  Serial.println("Intentando conectar a la última red conocida...");

  // Esperar hasta que se conecte o se agoten los intentos de reconexión
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    retries++;
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado a la red WiFi.");
    Serial.printf("Conectado a %s\n", WiFi.SSID().c_str());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

  } else {
    Serial.println("\nNo se pudo conectar a la última red. Iniciando SmartConfig...");

    // Si no se puede conectar, inicia SmartConfig
    WiFi.beginSmartConfig();

    // Espera hasta que SmartConfig esté completado
    while (!WiFi.smartConfigDone()) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("\nSmartConfig completado.");
    Serial.printf("Conectado a %s\n", WiFi.SSID().c_str());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  // Si la conexión WiFi está establecida, envía la IP al servidor
  if (WiFi.status() == WL_CONNECTED) {
    String espIp = WiFi.localIP().toString();
    
    // Crea un cliente HTTP
    HTTPClient http;
    
    // Envía la solicitud HTTP POST
    String serverPath = "code=" + code + "&ipAddress=" + espIp;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(serverPath);

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