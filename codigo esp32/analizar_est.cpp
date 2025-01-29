#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremote.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Configuración de la red WiFi
const char* ssid = "RioTel_SalgadO";
const char* password = "dcfs5411";

// Configuración del receptor y emisor IR
const int irPin = 13;
const int RECV_PIN = 15;
IRrecv irrecv(RECV_PIN);
IRData results;

// URL del servidor
const char* serverName = "http://192.168.2.151/pagina_web/pagina_web/public/new_esp/receive";
String code = "8lIsgR9J"; // Código identificador almacenado en la placa

// Pines de los LEDs
const int ledParpadeo = 5;
const int ledConectado = 14;

// Variables para el flujo de funcionamiento
unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 1000; // Intervalo de 1 segundo

void setup() {
  Serial.begin(115200);
  pinMode(ledParpadeo, OUTPUT);
  pinMode(ledConectado, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    retries++;
    digitalWrite(ledParpadeo, HIGH);
    delay(1000);
    digitalWrite(ledParpadeo, LOW);
    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledConectado, HIGH);
    String espIp = WiFi.localIP().toString();
    HTTPClient http;
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
  } else {
    Serial.println("No se pudo conectar a la red WiFi.");
  }

  IrReceiver.begin(RECV_PIN);
  IrSender.begin(irPin);

  // Crear tareas FreeRTOS
  xTaskCreatePinnedToCore(checkForTasks, "CheckForTasks", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(handleIRReception, "HandleIRReception", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // El loop se mantiene vacío ya que las tareas se manejan con FreeRTOS
}

void checkForTasks(void * parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      int httpResponseCode = http.GET();
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response: " + response);
        handleTask(response);
      } else {
        Serial.println("Error on sending GET: " + String(httpResponseCode));
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    vTaskDelay(requestInterval / portTICK_PERIOD_MS);
  }
}

void handleTask(const String& task) {
  if (task == "emitir_senal") {
    // Emitir señal IR
    // Aquí se debe implementar la lógica para emitir la señal IR
  } else if (task == "grabar_senal") {
    // Grabar señal IR
    // Aquí se debe implementar la lógica para grabar la señal IR
  }
}

void handleIRReception(void * parameter) {
  for (;;) {
    if (IrReceiver.decode()) {
      StringPrint stringCapture;
      IrReceiver.printIRResultRawFormatted(&stringCapture, true);
      String rawDataStr = stringCapture.buffer;
      String cleanedData = processRawData(rawDataStr);
      int firstSpaceIndex = cleanedData.indexOf(" ");
      if (firstSpaceIndex != -1) {
        cleanedData = cleanedData.substring(firstSpaceIndex + 1);
      }
      sendIRCodeRaw(cleanedData);
      IrReceiver.resume();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS); // Pequeño delay para evitar sobrecarga
  }
}

void sendIRCodeRaw(const String& rawDataStr) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "irCode=" + rawDataStr + "&code=" + code;
    int httpResponseCode = http.POST(postData);
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

String processRawData(String rawData) {
  int startIndex = rawData.indexOf("]:") + 2;
  int sumIndex = rawData.indexOf("Sum:");
  if (sumIndex > 0) {
    rawData = rawData.substring(startIndex, sumIndex);
  } else {
    rawData = rawData.substring(startIndex);
  }
  rawData.replace(" ", "");
  rawData.replace("\n", "");
  int firstDelimiterIndex = rawData.indexOf(",");
  if (firstDelimiterIndex != -1) {
    rawData = rawData.substring(firstDelimiterIndex + 1);
  }
  String processedData = "";
  int valueCount = 0;
  while (rawData.length() > 0) {
    int commaIndex = rawData.indexOf(",");
    String value = (commaIndex != -1) ? rawData.substring(0, commaIndex) : rawData;
    rawData = (commaIndex != -1) ? rawData.substring(commaIndex + 1) : "";
    if (valueCount >= 0 && value.length() > 0) {
      if (value[0] == '-') {
        value = value.substring(1);
      }
      int firstSpaceIndex = value.indexOf(' ');
      if (firstSpaceIndex != -1) {
        value = value.substring(firstSpaceIndex + 1);
      }
      processedData += (processedData == "" ? "" : ",") + value;
    }
    valueCount++;
  }
  return processedData;
}