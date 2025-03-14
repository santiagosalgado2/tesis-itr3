#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// Pin donde se conecta el receptor IR
const uint16_t kRecvPin = 15; // Cambia este pin según tu conexión

// Tamaño del buffer para capturar señales IR
const uint16_t kCaptureBufferSize = 1024;

// Tiempo de espera para capturar una señal IR (en milisegundos)
const uint8_t kTimeout = 50;

// Crear una instancia de IRrecv
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);

// Variable para almacenar los resultados de la captura
decode_results results;

void setup() {
  // Inicializar el puerto serie
  Serial.begin(115200);
  // Iniciar el receptor IR
  irrecv.enableIRIn();
  Serial.println("Sistema listo para grabar señales IR");
}

void loop() {
  // Verificar si se ha capturado una señal IR
  if (irrecv.decode(&results)) {
    // Imprimir los resultados en formato legible
    Serial.println(resultToHumanReadableBasic(&results));
    // Imprimir la señal en formato RAW
    Serial.println(resultToSourceCode(&results));
    // Reiniciar el receptor para capturar la siguiente señal
    irrecv.resume();
  }
}