#include <Arduino.h>
#include <IRremote.hpp>

// Pin donde está conectado el LED IR
const int irLedPin = 13;

// Arreglo con la señal IR en formato raw (duraciones en microsegundos)
uint16_t rawData[] = {
  4600, 4450, 650, 500, 650, 500, 650, 1650, 600, 1700,
  600, 550, 600, 1650, 650, 550, 600, 550,
  600, 550, 600, 550, 650, 1650, 600, 1700,
  600, 550, 600, 1650, 650, 550, 550, 600,
  600, 550, 600, 1700, 600, 1650, 600, 1700,
  600, 1650, 600, 600, 600, 550, 600, 550,
  600, 1700, 600, 550, 550, 600, 550, 600,
  600, 550, 600, 1700, 550, 1750, 550, 1700,
  600
};

void setup() {
  // Iniciar la librería IRremote en el pin irLedPin
  IrSender.begin(irLedPin);  // Inicializamos el transmisor IR
}

void loop() {
  // Emitir la señal IR en formato raw
  IrSender.sendRaw(rawData, sizeof(rawData) / sizeof(rawData[0]), 38);  // Frecuencia de 38kHz
  
  // Esperar 2 segundos antes de enviar la señal nuevamente
  delay(2000);
}