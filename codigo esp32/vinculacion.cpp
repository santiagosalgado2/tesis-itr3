#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <HTTPClient.h>
//LIBRERIAS UTILIZADAS
const char* serverName = "http://192.168.2.151/pagina_web/pagina_web/public/new_esp/receive";
// RUTA QUE EJECUTA UN CONTROLADOR DE LA PAGINA WEB
String code = "8lIsgR9J"; //CODIGO IDENTIFICADOR ALMACENADO EN LA PLACA, ES EL QUE PONE EL USUARIO PARA HACER LA VINCULACION

const int ledParpadeo = 5;  // LED que parpadea mientras se conecta
const int ledConectado = 14;  // LED que queda encendido una vez echa la conexión


void setup() {
  Serial.begin(115200);
  pinMode(ledParpadeo, OUTPUT);
  pinMode(ledConectado, OUTPUT);

  // Intentar conectar automáticamente a la última red conocida
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin();

  int retries = 0;
  Serial.println("Intentando conectar a la última red conocida...");

  // Esperar hasta que se conecte o se agoten los intentos de reconexión
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    retries++;
    digitalWrite(ledParpadeo, HIGH);  // Enciende LED de parpadeo
    delay(1000);
    digitalWrite(ledParpadeo, LOW);   // Apaga LED de parpadeo
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledConectado, HIGH);  // Enciende LED de parpadeo
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
      digitalWrite(ledParpadeo, HIGH);  // Enciende LED de parpadeo
      delay(1000);
      digitalWrite(ledParpadeo, LOW);   // Apaga LED de parpadeo
      delay(1000);
      Serial.print(".");
    }

    Serial.println("\nSmartConfig completado.");
    Serial.printf("Conectado a %s\n", WiFi.SSID().c_str());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  // Si la conexión WiFi está establecida, envía la IP al servidor
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledConectado, HIGH);  // Enciende LED de conexión exitosa
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