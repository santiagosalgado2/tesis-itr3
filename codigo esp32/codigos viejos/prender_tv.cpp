#include <IRremote.hpp>

const int irPin = 26; // Pin al que está conectado el LED IR

void setup() {
  Serial.begin(115200); // Iniciar la comunicación serial
  IrSender.begin(irPin); // Inicializar el emisor IR con el pin especificado
}

void loop() {
  Serial.println("Enviando señales RC6...");
  
  IrSender.sendRC6((uint32_t)0xC, 4); 
  delay(500); // Esperar medio segundo
  
   // Enviar la primera señal RC6
  IrSender.sendRC6((uint32_t)0x1000C, 20); 
  
  
  // Enviar la segunda señal RC6
  
  
  delay(2000); // Esperar 2 segundos antes de enviar nuevamente
}