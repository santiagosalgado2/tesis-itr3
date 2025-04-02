#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <ArduinoJson.h>

// HASTA AHORA ESTE ES EL CODIGO QUE ANDUVO TODO. EL OTRO NO EMITIIA BIEN LAS SEÑALES PORQUE EL FREERTOS LAS INTERRUMPIA
// HAY QUE PROBAR CON VARIOS DISPOSITIVOS PARA CONFIRMAR SI USAMOS ESTE

const char* serverName = "http://18.228.2.33/new_esp/receive";
const char* serverNameSignal = "http://18.228.2.33/handle/updateDbsignal";
const char* serverDeleteData = "http://18.228.2.33/handle/deleteData";
String code = "8lIsgR9J";

const int ledParpadeo = 5;
const int ledConectado = 14;


const uint16_t kSendPin = 13;


// Pin donde se conecta el receptor IR
const uint16_t kRecvPin = 15; // Cambia este pin según tu conexión

// Tamaño del buffer para capturar señales IR
const uint16_t kCaptureBufferSize = 1024;

// Tiempo de espera para capturar una señal IR (en milisegundos)
const uint8_t kTimeout = 50;

// Crear una instancia de IRrecv
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);

// Variable para almacenar los resultados de la captura

// Variables para almacenar los valores capturados
String protocolo;
String codigoHex;
String bits;

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
  irrecv.enableIRIn();
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
    const char* hexadecimal = doc["hexadecimal"];
    const char* protocolo= doc["protocolo"];
    const char* bits= doc["bits"];
    Serial.print("Hexadecimal recibido: ");
    Serial.println(hexadecimal);
    Serial.println(protocolo);
    Serial.println(bits);

    // Define un array para almacenar los datos crudos
  Serial.print("Hexadecimal recibido: ");
  Serial.println(hexadecimal);
  Serial.print("Protocolo recibido: ");
  Serial.println(protocolo);
  Serial.print("Bits recibidos: ");
  Serial.println(bits);

  // Convertir hexadecimal y bits a valores numéricos
  uint32_t hexValue = strtoul(hexadecimal, NULL, 16);
  uint16_t bitValue = atoi(bits);


    unsigned char data[4];
    data[0] = (hexValue >> 24) & 0xFF;
    data[1] = (hexValue >> 16) & 0xFF;
    data[2] = (hexValue >> 8) & 0xFF;
    data[3] = hexValue & 0xFF;

  // Enviar señal según el protocolo
  if (strcmp(protocolo, "NEC") == 0) {
    irsend.sendNEC(hexValue, bitValue);
  } else if (strcmp(protocolo, "SONY") == 0) {
    irsend.sendSony(hexValue, bitValue);
  } else if (strcmp(protocolo, "RC5") == 0) {
    irsend.sendRC5(hexValue, bitValue);
  } else if (strcmp(protocolo, "RC6") == 0) {
    irsend.sendRC6(hexValue, bitValue);
  } else if (strcmp(protocolo, "PANASONIC") == 0) {
    irsend.sendPanasonic(0x4004, hexValue); // Panasonic usa un parámetro adicional
  } else if (strcmp(protocolo, "SAMSUNG") == 0) {
    irsend.sendSAMSUNG(hexValue, bitValue);
  } else if (strcmp(protocolo, "AIRTON") == 0) {
    irsend.sendAirton(hexValue, bitValue);
  } else if (strcmp(protocolo, "AIRWELL") == 0) {
    irsend.sendAirwell(hexValue, bitValue);
  } else if (strcmp(protocolo, "AIWA_RC_T501") == 0) {
    irsend.sendAiwaRCT501(hexValue, bitValue);
  } else if (strcmp(protocolo, "AMCOR") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendAmcor(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "ARGO") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendArgo(data, sizeof(data), bitValue, false);
  } else if (strcmp(protocolo, "BOSCH144") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendBosch144(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "COOLIX") == 0) {
    irsend.sendCOOLIX(hexValue, bitValue);
  } else if (strcmp(protocolo, "CORONA_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendCoronaAc(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "DAIKIN") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendDaikin(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "DELONGHI_AC") == 0) {
    irsend.sendDelonghiAc(hexValue, bitValue);
  } else if (strcmp(protocolo, "DISH") == 0) {
    irsend.sendDISH(hexValue, bitValue);
  } else if (strcmp(protocolo, "DOSHISHA") == 0) {
    irsend.sendDoshisha(hexValue, bitValue);
  } else if (strcmp(protocolo, "ECOCLIM") == 0) {
    irsend.sendEcoclim(hexValue, bitValue);
  } else if (strcmp(protocolo, "ELECTRA_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendElectraAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "ELITESCREENS") == 0) {
    irsend.sendElitescreens(hexValue, bitValue);
  } else if (strcmp(protocolo, "EPSON") == 0) {
    irsend.sendEpson(hexValue, bitValue);
  } else if (strcmp(protocolo, "FUJITSU_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendFujitsuAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "GICABLE") == 0) {
    irsend.sendGICable(hexValue, bitValue);
  } else if (strcmp(protocolo, "GOODWEATHER") == 0) {
    irsend.sendGoodweather(hexValue, bitValue);
  } else if (strcmp(protocolo, "GORENJE") == 0) {
    irsend.sendGorenje(hexValue, bitValue);
  } else if (strcmp(protocolo, "GREE") == 0) {
    irsend.sendGree(hexValue, bitValue);
  } else if (strcmp(protocolo, "HAIER_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendHaierAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "HAIER_AC160") == 0) {
    irsend.sendHaierAC160(data, bitValue);
  } else if (strcmp(protocolo, "HAIER_AC176") == 0) {
    irsend.sendHaierAC176(data, bitValue);
  } else if (strcmp(protocolo, "HAIER_AC_YRW02") == 0) {
    irsend.sendHaierACYRW02(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendHitachiAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC1") == 0) {
    irsend.sendHitachiAC1(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC2") == 0) {
    irsend.sendHitachiAC2(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC264") == 0) {
    irsend.sendHitachiAc264(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC296") == 0) {
    irsend.sendHitachiAc296(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC3") == 0) {
    irsend.sendHitachiAC(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC344") == 0) {
    irsend.sendHitachiAc344(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC424") == 0) {
    irsend.sendHitachiAc424(data, bitValue);
  } else if (strcmp(protocolo, "INAX") == 0) {
    irsend.sendInax(hexValue, bitValue);
  } else if (strcmp(protocolo, "JVC") == 0) {
    irsend.sendJVC(hexValue, bitValue);
  } else if (strcmp(protocolo, "KELON") == 0) {
    irsend.sendKelon(hexValue, bitValue);
  } else if (strcmp(protocolo, "KELON168") == 0) {
    irsend.sendKelon168(data, bitValue);
  } else if (strcmp(protocolo, "KELVINATOR") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendKelvinator(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "LASERTAG") == 0) {
    irsend.sendLasertag(hexValue, bitValue);
  } else if (strcmp(protocolo, "LEGOPF") == 0) {
    irsend.sendLegoPf(hexValue, bitValue);
  } else if (strcmp(protocolo, "LG") == 0) {
    irsend.sendLG(hexValue, bitValue);
  } else if (strcmp(protocolo, "LG2") == 0) {
    irsend.sendLG2(hexValue, bitValue);
  } else if (strcmp(protocolo, "LUTRON") == 0) {
    irsend.sendLutron(hexValue, bitValue);
  } else if (strcmp(protocolo, "MAGIQUEST") == 0) {
    irsend.sendMagiQuest(hexValue, bitValue);
  } else if (strcmp(protocolo, "METZ") == 0) {
    irsend.sendMetz(hexValue, bitValue);
  } else if (strcmp(protocolo, "MIDEA") == 0) {
    irsend.sendMidea(hexValue, bitValue);
  } else if (strcmp(protocolo, "MIDEA24") == 0) {
    irsend.sendMidea24(hexValue, bitValue);
  } else if (strcmp(protocolo, "MILESTAG2") == 0) {
    irsend.sendMilestag2(hexValue, bitValue);
  } else if (strcmp(protocolo, "MIRAGE") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendMirage(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI") == 0) {
    irsend.sendMitsubishi(hexValue, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI112") == 0) {
    irsend.sendMitsubishi112(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI136") == 0) {
    irsend.sendMitsubishi136(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI2") == 0) {
    irsend.sendMitsubishi2(hexValue, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI_AC") == 0) {
    irsend.sendMitsubishiAC(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI_HEAVY_152") == 0) {
    irsend.sendMitsubishiHeavy152(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI_HEAVY_88") == 0) {
    irsend.sendMitsubishiHeavy88(data, bitValue);
  } else if (strcmp(protocolo, "MULTIBRACKETS") == 0) {
    irsend.sendMultibrackets(hexValue, bitValue);
  } else if (strcmp(protocolo, "MWM") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendMWM(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "NEC") == 0) {
    irsend.sendNEC(hexValue, bitValue);
  } else if (strcmp(protocolo, "NEOCLIMA") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendNeoclima(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "NIKAI") == 0) {
    irsend.sendNikai(hexValue, bitValue);
  } else if (strcmp(protocolo, "PANASONIC_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendPanasonicAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "PANASONIC_AC32") == 0) {
    irsend.sendPanasonicAC32(hexValue, bitValue);
  } else if (strcmp(protocolo, "PIONEER") == 0) {
    irsend.sendPioneer(hexValue, bitValue);
  } else if (strcmp(protocolo, "RC5") == 0) {
    irsend.sendRC5(hexValue, bitValue);
  } else if (strcmp(protocolo, "RCMM") == 0) {
    irsend.sendRCMM(hexValue, bitValue);
  } else if (strcmp(protocolo, "RHOSS") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendRhoss(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "SAMSUNG36") == 0) {
    irsend.sendSamsung36(hexValue, bitValue);
  } else if (strcmp(protocolo, "SAMSUNG_AC") == 0) {
    irsend.sendSamsungAC(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_AC") == 0) {
    irsend.sendSanyoAc(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_AC152") == 0) {
    irsend.sendSanyoAc152(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_AC88") == 0) {
    irsend.sendSanyoAc88(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_LC7461") == 0) {
    irsend.sendSanyoLC7461(hexValue, bitValue);
  } else if (strcmp(protocolo, "SHARP") == 0) {
    irsend.sendSharp(hexValue, bitValue);
  } else if (strcmp(protocolo, "SHARP_AC") == 0) {
    irsend.sendSharpAc(data, bitValue);
  } else if (strcmp(protocolo, "SONY") == 0) {
    irsend.sendSony(hexValue, bitValue);
  } else if (strcmp(protocolo, "SONY_38K") == 0) {
    irsend.sendSony38(hexValue, bitValue);
  } else if (strcmp(protocolo, "SYMPHONY") == 0) {
    irsend.sendSymphony(hexValue, bitValue);
  } else if (strcmp(protocolo, "TCL112AC") == 0) {
    irsend.sendTcl112Ac(data, bitValue);
  } else if (strcmp(protocolo, "TCL96AC") == 0) {
    irsend.sendTcl96Ac(data, bitValue);
  } else if (strcmp(protocolo, "TCL96AC") == 0) {
    irsend.sendTcl96Ac(data, bitValue);
  }  else if (strcmp(protocolo, "TECO") == 0) {
    irsend.sendTeco(hexValue, bitValue);
  } else if (strcmp(protocolo, "TEKNOPOINT") == 0) {
    irsend.sendTeknopoint(data, bitValue);
  } else if (strcmp(protocolo, "TOSHIBA_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendToshibaAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "TOTO") == 0) {
    irsend.sendToto(hexValue, bitValue);
  } else if (strcmp(protocolo, "TRANSCOLD") == 0) {
    irsend.sendTranscold(hexValue, bitValue);
  } else if (strcmp(protocolo, "TROTEC") == 0) {
    irsend.sendTrotec(data, bitValue);
  } else if (strcmp(protocolo, "TROTEC_3550") == 0) {
    irsend.sendTrotec3550(data, bitValue);
  } else if (strcmp(protocolo, "TRUMA") == 0) {
    irsend.sendTruma(hexValue, bitValue);
  } else if (strcmp(protocolo, "VESTEL_AC") == 0) {
    irsend.sendVestelAc(hexValue, bitValue);
  } else if (strcmp(protocolo, "VOLTAS") == 0) {
    irsend.sendVoltas(data, bitValue);
  } else if (strcmp(protocolo, "WHIRLPOOL_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsend.sendWhirlpoolAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "WHYNTER") == 0) {
    irsend.sendWhynter(hexValue, bitValue);
  } else if (strcmp(protocolo, "WOWWEE") == 0) {
    irsend.sendWowwee(hexValue, bitValue);
  } else if (strcmp(protocolo, "XMP") == 0) {
    irsend.sendXmp(hexValue, bitValue);
  } else if (strcmp(protocolo, "YORK") == 0) {
    irsend.sendYork(data, bitValue);
  } else if (strcmp(protocolo, "ZEPEAL") == 0) {
    irsend.sendZepeal(hexValue, bitValue);
  }else{
    Serial.println("Protocolo no encontrado");
  }

  HTTPClient http;
      http.begin(serverDeleteData);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "code=" + code;
      int httpResponseCode = http.POST(postData);
      Serial.print("Respuesta del servidor al eliminar datos: ");
      Serial.println(httpResponseCode);

 } else if (strcmp(accion, "grabar_senal") == 0) {
    Serial.println("hola");

    Serial.println("hola");
    while (!irrecv.decode(&results)) {
      delay(100);
    }
    protocolo = typeToString(results.decode_type);
    codigoHex = resultToHexidecimal(&results); // Eliminar el segundo argumento
    bits = String(results.bits);

    // Imprimir los valores almacenados en las variables
    Serial.println("Protocolo: " + protocolo);
    Serial.println("Código Hexadecimal: " + codigoHex);
    Serial.println("Bits: " + bits);

    // Enviar los datos a la función PHP
    sendIRCode(protocolo, codigoHex, bits);

    // Reiniciar el receptor para capturar la siguiente señal
    irrecv.resume();
  }
}

void sendIRCode(const String& protocolo, const String& codigoHex, const String& bits) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverNameSignal);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "protocolo=" + protocolo + "&codigoHex=" + codigoHex + "&bits=" + bits + "&code=" + code;
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