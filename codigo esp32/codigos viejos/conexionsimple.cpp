#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ITR3-6";           // Nombre de tu red WiFi
const char* password = "archivado";   // Contraseña de tu red WiFi

const char* serverUrl = "http://192.168.2.151/pagina_web/pagina_web/public/connect_esp32";  // URL de tu servidor web

void setup() {
  Serial.begin(115200);

  // Conectarse al Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConectado a la red WiFi");
  
  // Enviar solicitud POST al servidor
  if (WiFi.status() == WL_CONNECTED) { // Verificar si estamos conectados
    HTTPClient http;

    http.begin(serverUrl);  // Especificar la URL del servidor

       // Definir los datos a enviar (en formato URL-encoded)
    String postData = "param1=valor1&param2=valor2";  // Enviar dos parámetros

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Establecer el tipo de contenido

    int httpResponseCode = http.POST(postData);  // Enviar solicitud POST

    // Verificar la respuesta del servidor
    if (httpResponseCode > 0) {
      String response = http.getString();  // Obtener respuesta del servidor
      Serial.println("Respuesta del servidor: " + response);
    } else {
      Serial.println("Error en la solicitud POST: " + String(httpResponseCode));
    }

    http.end();  // Cerrar la conexión
  }
}

void loop() {
  // No es necesario hacer nada en el loop para este ejemplo
}