#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <irsend.h>
#include <IRutils.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <esp_smartconfig.h>
#include <ArduinoJson.h>

// HASTA AHORA ESTE ES EL CODIGO QUE ANDUVO TODO. EL OTRO NO EMITIIA BIEN LAS SEÑALES PORQUE EL FREERTOS LAS INTERRUMPIA
// HAY QUE PROBAR CON VARIOS DISPOSITIVOS PARA CONFIRMAR SI USAMOS ESTE

const char* serverName = "https://irconnect.site/new_esp/receive";
const char* serverNameSignal = "https://irconnect.site/handle/updateDbsignal";
const char* serverDeleteData = "https://irconnect.site/handle/deleteData";
String code = "8lIsgR9J";

const int ledParpadeo = 5;
const int ledConectado = 14;

 
const uint16_t kSendPin1 = 16;
const uint16_t kSendPin2 = 19;
const uint16_t kSendPin3 = 22;

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

IRsend irsend1(kSendPin1);
IRsend irsend2(kSendPin2);
IRsend irsend3(kSendPin3);

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
      delay(500);
      digitalWrite(ledParpadeo, LOW);
      delay(500);
      Serial.print(".");
    }

    Serial.println("\nSmartConfig completado.");
    Serial.printf("Conectado a %s\n", WiFi.SSID().c_str());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  
  irsend1.begin();
  irsend2.begin();
  irsend3.begin();  // Inicializa el objeto IRsend
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
    return;
  }

  const char* accion = doc["accion"];
  Serial.print("Accion recibida: ");
  Serial.println(accion);

  if (strcmp(accion, "emitir_senal") == 0) {
    const char* hexadecimal = doc["hexadecimal"];
    const char* protocolo= doc["protocolo"];
    const char* bits= doc["bits"];
int led = doc["led"] ? int(doc["led"]) : 1;    Serial.print("Hexadecimal recibido: ");
    Serial.println(hexadecimal);
    Serial.print("Protocolo recibido: ");
    Serial.println(protocolo);
    Serial.print("Bits recibidos: ");
    Serial.println(bits);
    Serial.print("Led recibido: ");
    Serial.println(led);

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

    IRsend* irsendPtr = &irsend1;
    if (led == 2) irsendPtr = &irsend2;
    else if (led == 3) irsendPtr = &irsend3;

  // Enviar señal según el protocolo
  if (strcmp(protocolo, "NEC") == 0) {
    irsendPtr->sendNEC(hexValue, bitValue);
  } else if (strcmp(protocolo, "SONY") == 0) {
    irsendPtr->sendSony(hexValue, bitValue);
  } else if (strcmp(protocolo, "RC5") == 0) {
    irsendPtr->sendRC5(hexValue, bitValue);
  } else if (strcmp(protocolo, "RC6") == 0) {
    irsendPtr->sendRC6(hexValue, bitValue);
  } else if (strcmp(protocolo, "PANASONIC") == 0) {
    irsendPtr->sendPanasonic(0x4004, hexValue); // Panasonic usa un parámetro adicional
  } else if (strcmp(protocolo, "SAMSUNG") == 0) {
    irsendPtr->sendSAMSUNG(hexValue, bitValue);
  } else if (strcmp(protocolo, "AIRTON") == 0) {
    irsendPtr->sendAirton(hexValue, bitValue);
  } else if (strcmp(protocolo, "AIRWELL") == 0) {
    irsendPtr->sendAirwell(hexValue, bitValue);
  } else if (strcmp(protocolo, "AIWA_RC_T501") == 0) {
    irsendPtr->sendAiwaRCT501(hexValue, bitValue);
  } else if (strcmp(protocolo, "AMCOR") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendAmcor(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "ARGO") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendArgo(data, sizeof(data), bitValue, false);
  } else if (strcmp(protocolo, "BOSCH144") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendBosch144(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "COOLIX") == 0) {
    irsendPtr->sendCOOLIX(hexValue, bitValue);
  } else if (strcmp(protocolo, "CORONA_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendCoronaAc(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "DAIKIN") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendDaikin(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "DELONGHI_AC") == 0) {
    irsendPtr->sendDelonghiAc(hexValue, bitValue);
  } else if (strcmp(protocolo, "DISH") == 0) {
    irsendPtr->sendDISH(hexValue, bitValue);
  } else if (strcmp(protocolo, "DOSHISHA") == 0) {
    irsendPtr->sendDoshisha(hexValue, bitValue);
  } else if (strcmp(protocolo, "ECOCLIM") == 0) {
    irsendPtr->sendEcoclim(hexValue, bitValue);
  } else if (strcmp(protocolo, "ELECTRA_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendElectraAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "ELITESCREENS") == 0) {
    irsendPtr->sendElitescreens(hexValue, bitValue);
  } else if (strcmp(protocolo, "EPSON") == 0) {
    irsendPtr->sendEpson(hexValue, bitValue);
  } else if (strcmp(protocolo, "FUJITSU_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendFujitsuAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "GICABLE") == 0) {
    irsendPtr->sendGICable(hexValue, bitValue);
  } else if (strcmp(protocolo, "GOODWEATHER") == 0) {
   irsendPtr->sendGoodweather(hexValue, bitValue);
  } else if (strcmp(protocolo, "GORENJE") == 0) {
    irsendPtr->sendGorenje(hexValue, bitValue);
  } else if (strcmp(protocolo, "GREE") == 0) {
    irsendPtr->sendGree(hexValue, bitValue);
  } else if (strcmp(protocolo, "HAIER_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendHaierAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "HAIER_AC160") == 0) {
    irsendPtr->sendHaierAC160(data, bitValue);
  } else if (strcmp(protocolo, "HAIER_AC176") == 0) {
    irsendPtr->sendHaierAC176(data, bitValue);
  } else if (strcmp(protocolo, "HAIER_AC_YRW02") == 0) {
    irsendPtr->sendHaierACYRW02(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendHitachiAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC1") == 0) {
    irsendPtr->sendHitachiAC1(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC2") == 0) {
    irsendPtr->sendHitachiAC2(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC264") == 0) {
    irsendPtr->sendHitachiAc264(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC296") == 0) {
    irsendPtr->sendHitachiAc296(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC3") == 0) {
    irsendPtr->sendHitachiAC(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC344") == 0) {
    irsendPtr->sendHitachiAc344(data, bitValue);
  } else if (strcmp(protocolo, "HITACHI_AC424") == 0) {
    irsendPtr->sendHitachiAc424(data, bitValue);
  } else if (strcmp(protocolo, "INAX") == 0) {
    irsendPtr->sendInax(hexValue, bitValue);
  } else if (strcmp(protocolo, "JVC") == 0) {
    irsendPtr->sendJVC(hexValue, bitValue);
  } else if (strcmp(protocolo, "KELON") == 0) {
    irsendPtr->sendKelon(hexValue, bitValue);
  } else if (strcmp(protocolo, "KELON168") == 0) {
    irsendPtr->sendKelon168(data, bitValue);
  } else if (strcmp(protocolo, "KELVINATOR") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendKelvinator(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "LASERTAG") == 0) {
    irsendPtr->sendLasertag(hexValue, bitValue);
  } else if (strcmp(protocolo, "LEGOPF") == 0) {
    irsendPtr->sendLegoPf(hexValue, bitValue);
  } else if (strcmp(protocolo, "LG") == 0) {
    irsendPtr->sendLG(hexValue, bitValue);
  } else if (strcmp(protocolo, "LG2") == 0) {
    irsendPtr->sendLG2(hexValue, bitValue);
  } else if (strcmp(protocolo, "LUTRON") == 0) {
    irsendPtr->sendLutron(hexValue, bitValue);
  } else if (strcmp(protocolo, "MAGIQUEST") == 0) {
    irsendPtr->sendMagiQuest(hexValue, bitValue);
  } else if (strcmp(protocolo, "METZ") == 0) {
    irsendPtr->sendMetz(hexValue, bitValue);
  } else if (strcmp(protocolo, "MIDEA") == 0) {
    irsendPtr->sendMidea(hexValue, bitValue);
  } else if (strcmp(protocolo, "MIDEA24") == 0) {
    irsendPtr->sendMidea24(hexValue, bitValue);
  } else if (strcmp(protocolo, "MILESTAG2") == 0) {
    irsendPtr->sendMilestag2(hexValue, bitValue);
  } else if (strcmp(protocolo, "MIRAGE") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendMirage(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI") == 0) {
    irsendPtr->sendMitsubishi(hexValue, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI112") == 0) {
    irsendPtr->sendMitsubishi112(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI136") == 0) {
    irsendPtr->sendMitsubishi136(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI2") == 0) {
    irsendPtr->sendMitsubishi2(hexValue, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI_AC") == 0) {
    irsendPtr->sendMitsubishiAC(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI_HEAVY_152") == 0) {
    irsendPtr->sendMitsubishiHeavy152(data, bitValue);
  } else if (strcmp(protocolo, "MITSUBISHI_HEAVY_88") == 0) {
    irsendPtr->sendMitsubishiHeavy88(data, bitValue);
  } else if (strcmp(protocolo, "MULTIBRACKETS") == 0) {
    irsendPtr->sendMultibrackets(hexValue, bitValue);
  } else if (strcmp(protocolo, "MWM") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendMWM(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "NEC") == 0) {
    irsendPtr->sendNEC(hexValue, bitValue);
  } else if (strcmp(protocolo, "NEOCLIMA") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendNeoclima(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "NIKAI") == 0) {
    irsendPtr->sendNikai(hexValue, bitValue);
  } else if (strcmp(protocolo, "PANASONIC_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendPanasonicAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "PANASONIC_AC32") == 0) {
    irsendPtr->sendPanasonicAC32(hexValue, bitValue);
  } else if (strcmp(protocolo, "PIONEER") == 0) {
    irsendPtr->sendPioneer(hexValue, bitValue);
  } else if (strcmp(protocolo, "RC5") == 0) {
    irsendPtr->sendRC5(hexValue, bitValue);
  } else if (strcmp(protocolo, "RCMM") == 0) {
    irsendPtr->sendRCMM(hexValue, bitValue);
  } else if (strcmp(protocolo, "RHOSS") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendRhoss(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "SAMSUNG36") == 0) {
    irsendPtr->sendSamsung36(hexValue, bitValue);
  } else if (strcmp(protocolo, "SAMSUNG_AC") == 0) {
    irsendPtr->sendSamsungAC(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_AC") == 0) {
    irsendPtr->sendSanyoAc(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_AC152") == 0) {
    irsendPtr->sendSanyoAc152(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_AC88") == 0) {
    irsendPtr->sendSanyoAc88(data, bitValue);
  } else if (strcmp(protocolo, "SANYO_LC7461") == 0) {
    irsendPtr->sendSanyoLC7461(hexValue, bitValue);
  } else if (strcmp(protocolo, "SHARP") == 0) {
    irsendPtr->sendSharp(hexValue, bitValue);
  } else if (strcmp(protocolo, "SHARP_AC") == 0) {
    irsendPtr->sendSharpAc(data, bitValue);
  } else if (strcmp(protocolo, "SONY") == 0) {
    irsendPtr->sendSony(hexValue, bitValue);
  } else if (strcmp(protocolo, "SONY_38K") == 0) {
    irsendPtr->sendSony38(hexValue, bitValue);
  } else if (strcmp(protocolo, "SYMPHONY") == 0) {
    irsendPtr->sendSymphony(hexValue, bitValue);
  } else if (strcmp(protocolo, "TCL112AC") == 0) {
    irsendPtr->sendTcl112Ac(data, bitValue);
  } else if (strcmp(protocolo, "TCL96AC") == 0) {
    irsendPtr->sendTcl96Ac(data, bitValue);
  } else if (strcmp(protocolo, "TCL96AC") == 0) {
    irsendPtr->sendTcl96Ac(data, bitValue);
  }  else if (strcmp(protocolo, "TECO") == 0) {
    irsendPtr->sendTeco(hexValue, bitValue);
  } else if (strcmp(protocolo, "TEKNOPOINT") == 0) {
    irsendPtr->sendTeknopoint(data, bitValue);
  } else if (strcmp(protocolo, "TOSHIBA_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendToshibaAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "TOTO") == 0) {
    irsendPtr->sendToto(hexValue, bitValue);
  } else if (strcmp(protocolo, "TRANSCOLD") == 0) {
    irsendPtr->sendTranscold(hexValue, bitValue);
  } else if (strcmp(protocolo, "TROTEC") == 0) {
    irsendPtr->sendTrotec(data, bitValue);
  } else if (strcmp(protocolo, "TROTEC_3550") == 0) {
    irsendPtr->sendTrotec3550(data, bitValue);
  } else if (strcmp(protocolo, "TRUMA") == 0) {
    irsendPtr->sendTruma(hexValue, bitValue);
  } else if (strcmp(protocolo, "VESTEL_AC") == 0) {
    irsendPtr->sendVestelAc(hexValue, bitValue);
  } else if (strcmp(protocolo, "VOLTAS") == 0) {
    irsendPtr->sendVoltas(data, bitValue);
  } else if (strcmp(protocolo, "WHIRLPOOL_AC") == 0) {
    uint8_t data[] = { (uint8_t)(hexValue >> 24), (uint8_t)(hexValue >> 16), (uint8_t)(hexValue >> 8), (uint8_t)hexValue };
    irsendPtr->sendWhirlpoolAC(data, sizeof(data), bitValue);
  } else if (strcmp(protocolo, "WHYNTER") == 0) {
    irsendPtr->sendWhynter(hexValue, bitValue);
  } else if (strcmp(protocolo, "WOWWEE") == 0) {
    irsendPtr->sendWowwee(hexValue, bitValue);
  } else if (strcmp(protocolo, "XMP") == 0) {
    irsendPtr->sendXmp(hexValue, bitValue);
  } else if (strcmp(protocolo, "YORK") == 0) {
    irsendPtr->sendYork(data, bitValue);
  } else if (strcmp(protocolo, "ZEPEAL") == 0) {
    irsendPtr->sendZepeal(hexValue, bitValue);
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
    Serial.println("Iniciando grabación de señal IR...");
    
    unsigned long startTime = millis();
    unsigned long timeout = 180000; // 3 minutos en milisegundos
    bool signalReceived = false;
    
    Serial.println("Esperando señal IR (máximo 3 minutos)...");
    
    while ((millis() - startTime) < timeout && !signalReceived) {
      if (irrecv.decode(&results)) {
        signalReceived = true;
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
      } else {
        delay(100); // Pequeña pausa antes de intentar de nuevo
      }
    }
    
    // Si no se recibió señal en 3 minutos, eliminar los datos de la solicitud
    if (!signalReceived) {
      Serial.println("Timeout: No se detectó señal IR en 3 minutos. Eliminando solicitud...");
      
      HTTPClient http;
      http.begin(serverDeleteData);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "code=" + code;
      int httpResponseCode = http.POST(postData);
      Serial.print("Respuesta del servidor al eliminar datos: ");
      Serial.println(httpResponseCode);
      http.end();
    }
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
