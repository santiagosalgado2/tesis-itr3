#include <WiFi.h>
#include <IRremote.hpp>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ArduinoJson.h>

const char* serverName = "http://192.168.1.119/pagina_web/pagina_web/public/new_esp/receive";
const char* serverNameSignal = "http://192.168.1.119/pagina_web/pagina_web/public/new_esp/receive_signal";
String code = "8lIsgR9J";

const int ledParpadeo = 5;
const int ledConectado = 14;

IRrecv irrecv(15);
IRData results;

class StringPrint : public Print {
public:
  String buffer;
  size_t write(uint8_t c) override {
    buffer += (char)c;
    return 1;
  }
  size_t write(const uint8_t* data, size_t size) override {
    buffer += String((const char*)data).substring(0, size);
    return size;
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledParpadeo, OUTPUT);
  pinMode(ledConectado, OUTPUT);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin();

  int retries = 0;
  Serial.println("Intentando conectar a la última red conocida...");

  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    retries++;
    digitalWrite(ledParpadeo, HIGH);
    delay(1000);
    digitalWrite(ledParpadeo, LOW);
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledConectado, HIGH);
    Serial.println("\nConectado a la red WiFi.");
    Serial.printf("Conectado a %s\n", WiFi.SSID().c_str());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar a la última red. Iniciando SmartConfig...");
    WiFi.beginSmartConfig();

    while (!WiFi.smartConfigDone()) {
      digitalWrite(ledParpadeo, HIGH);
      delay(1000);
      digitalWrite(ledParpadeo, LOW);
      delay(1000);
      Serial.print(".");
    }

    Serial.println("\nSmartConfig completado.");
    Serial.printf("Conectado a %s\n", WiFi.SSID().c_str());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  IrSender.begin(26);  // Inicializa el objeto IRsend con el pin 13

  xTaskCreatePinnedToCore(checkForTasks, "CheckForTasks", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // El loop se mantiene vacío ya que las tareas se manejan con FreeRTOS
}

void checkForTasks(void * parameter) {
  for (;;) {
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
        handleTask(response);
      } else {
        Serial.print("Error en la petición: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void handleTask(const String& task) {
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, task);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  const char* accion = doc["accion"];
  if (strcmp(accion, "emitir_senal") == 0) {
    const char* codigo_raw = doc["codigo_raw"];
    std::vector<uint16_t> rawData;
    String rawDataStr = String(codigo_raw);
    int startIndex = 0;
    int commaIndex;
    while ((commaIndex = rawDataStr.indexOf(',', startIndex)) != -1) {
      String numberStr = rawDataStr.substring(startIndex, commaIndex);
      rawData.push_back(numberStr.toInt());
      startIndex = commaIndex + 1;
    }
    if (startIndex < rawDataStr.length()) {
      String numberStr = rawDataStr.substring(startIndex);
      rawData.push_back(numberStr.toInt());
    }

    if (rawData.size() > 0) {
      IrSender.sendRaw(rawData.data(), rawData.size(), 38); // Asegúrate de que la frecuencia sea correcta
      Serial.println("Señal IR enviada correctamente.");
    } else {
      Serial.println("Array 'rawData' está vacío.");
    }
  } else if (strcmp(accion, "grabar_senal") == 0) {
    irrecv.enableIRIn();
    while (!irrecv.decode()) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    StringPrint stringCapture;
    irrecv.printIRResultRawFormatted(&stringCapture, true);
    String rawDataStr = stringCapture.buffer;
    String cleanedData = processRawData(rawDataStr);
    int firstSpaceIndex = cleanedData.indexOf(" ");
    if (firstSpaceIndex != -1) {
      cleanedData = cleanedData.substring(firstSpaceIndex + 1);
    }
    sendIRCodeRaw(cleanedData);
    irrecv.resume();
  }
}

void sendIRCodeRaw(const String& rawDataStr) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverNameSignal);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "code=" + code + "&irCode=" + rawDataStr;
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