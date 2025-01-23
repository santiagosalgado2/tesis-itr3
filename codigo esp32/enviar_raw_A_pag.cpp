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
const char* serverName = "http://192.168.1.105/pagina_web/pagina_web/public/recibir_codigo";
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

    // Procesar para eliminar valores iniciales, signos, el sum y los primeros tres valores
    String cleanedData = processRawData(rawDataStr);

    // Eliminar desde el primer dígito hasta el primer espacio en blanco
    int firstSpaceIndex = cleanedData.indexOf(" ");
    if (firstSpaceIndex != -1) {
      cleanedData = cleanedData.substring(firstSpaceIndex + 1);
    }

    // Imprimir la señal procesada
    Serial.println("Processed IR Signal:");
    Serial.println(cleanedData);

    // Enviar la señal capturada procesada
    sendIRCodeRaw(cleanedData);

    IrReceiver.resume(); // Reanudar el receptor
  }
}

// Función para procesar y limpiar la señal RAW
String processRawData(String rawData) {
  // Quitar las líneas iniciales y el "Sum"
  int startIndex = rawData.indexOf("]:") + 2; // Encuentra el final de "rawData[XX]:"
  int sumIndex = rawData.indexOf("Sum:");
  if (sumIndex > 0) {
    rawData = rawData.substring(startIndex, sumIndex); // Extraer solo la parte de datos
  } else {
    rawData = rawData.substring(startIndex); // Si no hay "Sum", tomar hasta el final
  }

  // Eliminar espacios y saltos de línea
  rawData.replace(" ", ""); // Quitar todos los espacios en la cadena
  rawData.replace("\n", ""); // Quitar todos los saltos de línea en la cadena

  // Eliminar el primer valor hasta el primer espacio o salto de línea
  int firstDelimiterIndex = rawData.indexOf(",");
  if (firstDelimiterIndex != -1) {
    rawData = rawData.substring(firstDelimiterIndex + 1);
  }

  // Convertir a un array de números y omitir los primeros tres valores
  String processedData = "";
  int valueCount = 0;
  while (rawData.length() > 0) {
    int commaIndex = rawData.indexOf(","); // Buscar la coma como separador
    String value = (commaIndex != -1) ? rawData.substring(0, commaIndex) : rawData;
    rawData = (commaIndex != -1) ? rawData.substring(commaIndex + 1) : "";

    // Validar y procesar el valor
    if (valueCount >= 0 && value.length() > 0) {
      // Eliminar el signo negativo si existe
      if (value[0] == '-') {
        value = value.substring(1); // Eliminar el primer carácter si es un signo negativo
      }
      // Eliminar todo hasta el primer espacio en blanco
      int firstSpaceIndex = value.indexOf(' ');
      if (firstSpaceIndex != -1) {
        value = value.substring(firstSpaceIndex + 1);
      }
      processedData += (processedData == "" ? "" : ",") + value; // Agregar el valor al resultado
    }
    valueCount++;
  }
  return processedData;
}