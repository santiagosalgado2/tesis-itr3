#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <ArduinoJson.h>

// HASTA AHORA ESTE ES EL CODIGO QUE ANDUVO TODO. EL OTRO NO EMITIIA BIEN LAS SEÑALES PORQUE EL FREERTOS LAS INTERRUMPIA
// HAY QUE PROBAR CON VARIOS DISPOSITIVOS PARA CONFIRMAR SI USAMOS ESTE

const char* serverName = "http://192.168.1.105/pagina_web/pagina_web/public/new_esp/receive";
const char* serverNameSignal = "http://192.168.1.105/pagina_web/pagina_web/public/handle/updateDbsignal";
const char* serverDeleteData = "http://192.168.1.105/pagina_web/pagina_web/public/handle/deleteData";
String code = "8lIsgR9J";

const int ledParpadeo = 5;
const int ledConectado = 14;

const uint16_t kRecvPin = 15;
const uint16_t kSendPin = 13;
IRrecv irrecv(kRecvPin);
IRsend irsend(kSendPin);
decode_results results;

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

  irsend.begin();  // Inicializa el objeto IRsend
}

void loop() {
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

  delay(1000); // Espera 1 segundo antes de verificar nuevamente
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
  Serial.print("Accion recibida: ");
  Serial.println(accion);

  if (strcmp(accion, "emitir_senal") == 0) {
    const char* codigo_raw = doc["codigo_raw"];
    Serial.print("Codigo raw recibido: ");
    Serial.println(codigo_raw);

    // Define un array para almacenar los datos crudos
    const int maxRawDataSize = 500; // Ajusta el tamaño según sea necesario
    uint16_t rawData[maxRawDataSize];
    int rawDataSize = 0;

    String rawDataStr = String(codigo_raw);
    int startIndex = 0;
    int commaIndex;
    while ((commaIndex = rawDataStr.indexOf(',', startIndex)) != -1 && rawDataSize < maxRawDataSize) {
      String numberStr = rawDataStr.substring(startIndex, commaIndex);
      rawData[rawDataSize++] = numberStr.toInt();
      startIndex = commaIndex + 1;
    }
    if (startIndex < rawDataStr.length() && rawDataSize < maxRawDataSize) {
      String numberStr = rawDataStr.substring(startIndex);
      rawData[rawDataSize++] = numberStr.toInt();
    }

    Serial.print("Datos crudos procesados: ");
    for (int i = 0; i < rawDataSize; i++) {
      Serial.print(rawData[i]);
      Serial.print(" ");
    }
    Serial.println();

    if (rawDataSize > 0) {
      irsend.sendRaw(rawData, rawDataSize, 38); // Asegúrate de que la frecuencia sea correcta
      Serial.println("Señal IR enviada correctamente.");
      HTTPClient http;
      http.begin(serverDeleteData);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "code=" + code;
      int httpResponseCode = http.POST(postData);
      Serial.print("Respuesta del servidor al eliminar datos: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Array 'rawData' está vacío.");
    }
  } else if (strcmp(accion, "grabar_senal") == 0) {
    irrecv.enableIRIn();
    while (!irrecv.decode(&results)) {
      delay(10);
    }
    String rawDataStr = decodeIRResultToRaw(results);
    String cleanedData = processRawData(rawDataStr);
    int firstSpaceIndex = cleanedData.indexOf(" ");
    if (firstSpaceIndex != -1) {
      cleanedData = cleanedData.substring(firstSpaceIndex + 1);
    }
    sendIRCodeRaw(cleanedData);
    irrecv.resume();
  }
}

String decodeIRResultToRaw(decode_results results) {
  String rawDataStr = "";
  for (int i = 1; i < results.rawlen; i++) {
    if (i % 2) {  // Mark
      rawDataStr += results.rawbuf[i] * kRawTick;
    } else {  // Space
      rawDataStr += -results.rawbuf[i] * kRawTick;
    }
    if (i < results.rawlen - 1) {
      rawDataStr += ",";
    }
  }
  return rawDataStr;
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