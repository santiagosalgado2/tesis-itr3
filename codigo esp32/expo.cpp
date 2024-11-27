#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremote.hpp>

#define LED_ROJO  4
#define LED_AMARILLO 13
#define LED_VERDE 32
#define IR_RECEIVER_PIN 15

const char* ssid = "placa";
const char* password = "12345678";
const char* serverUrl = "http://192.168.2.119/pagina_web/pagina_web/public/expo/actualizar"; // Cambia esto

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");

    IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);
    pinMode(LED_ROJO, OUTPUT);
    pinMode(LED_AMARILLO, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    apagarLeds();
}

void loop() {
    if (IrReceiver.decode()) {
        uint32_t irCode = IrReceiver.decodedIRData.decodedRawData;

        String estado = "";
        if (irCode == 0xF30CFF00) {
            apagarLeds();
            toggleLed(LED_ROJO);
            estado = "rojo";
        } else if (irCode == 0xE718FF00) {
            apagarLeds();
            toggleLed(LED_AMARILLO);
            estado = "amarillo";
        } else if (irCode == 0xA15EFF00) {
            apagarLeds();
            toggleLed(LED_VERDE);
            estado = "verde";
        } else if (irCode == 0xBA45FF00) {
            modoSemaforo();
            estado = "semaforo";
        }

        if (estado != "") {
            enviarEstadoAlServidor(estado);
        }
        IrReceiver.resume();
    }
}

void toggleLed(int pin) {
    digitalWrite(pin, !digitalRead(pin));
}

void modoSemaforo() {
    apagarLeds();
    digitalWrite(LED_ROJO, HIGH); delay(5000);
    digitalWrite(LED_ROJO, LOW); digitalWrite(LED_AMARILLO, HIGH); delay(2000);
    digitalWrite(LED_AMARILLO, LOW); digitalWrite(LED_VERDE, HIGH); delay(5000);
    digitalWrite(LED_VERDE, LOW);
}

void apagarLeds() {
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_VERDE, LOW);
}

void enviarEstadoAlServidor(String estado) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String urlCompleta = String(serverUrl) + "?estado=" + String(estado);
        http.begin(urlCompleta.c_str());
        int httpResponseCode = http.GET();  
        http.end();
    }
}
