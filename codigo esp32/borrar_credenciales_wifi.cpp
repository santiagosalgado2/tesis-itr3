#include <WiFi.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

const int ledParpadeo = 25;
const int ledConectado = 12;

void setup() {
  Serial.begin(115200);
  Serial.println("========================================");
  Serial.println("BORRADOR DE CREDENCIALES WIFI - ESP32");
  Serial.println("========================================");
  
  // Configurar LEDs
  pinMode(ledParpadeo, OUTPUT);
  pinMode(ledConectado, OUTPUT);
  
  // Apagar LED conectado y parpadear para indicar proceso
  digitalWrite(ledConectado, LOW);
  
  Serial.println("Iniciando proceso de borrado de credenciales...");
  
  // Parpadear LED para indicar que está trabajando
  for(int i = 0; i < 3; i++) {
    digitalWrite(ledParpadeo, HIGH);
    delay(200);
    digitalWrite(ledParpadeo, LOW);
    delay(200);
  }
  
  // Paso 1: Desconectar WiFi actual
  Serial.println("1. Desconectando de la red actual...");
  WiFi.disconnect(true, true); // true, true = borrar credenciales y resetear
  delay(1000);
  
  // Paso 2: Borrar configuración almacenada en NVS
  Serial.println("2. Borrando configuración almacenada en NVS...");
  esp_err_t err = nvs_flash_erase();
  if (err == ESP_OK) {
    Serial.println("   ✓ NVS Flash borrado exitosamente");
  } else {
    Serial.printf("   ✗ Error borrando NVS Flash: %s\n", esp_err_to_name(err));
  }
  
  // Reinicializar NVS
  err = nvs_flash_init();
  if (err == ESP_OK) {
    Serial.println("   ✓ NVS Flash reinicializado");
  } else {
    Serial.printf("   ✗ Error reinicializando NVS Flash: %s\n", esp_err_to_name(err));
  }
  
  // Paso 3: Borrar configuración WiFi específica
  Serial.println("3. Borrando configuración WiFi almacenada...");
  esp_wifi_restore();
  Serial.println("   ✓ Configuración WiFi restaurada a valores por defecto");
  
  // Paso 4: Reiniciar modo WiFi
  Serial.println("4. Reiniciando subsistema WiFi...");
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  
  Serial.println("========================================");
  Serial.println("✓ PROCESO COMPLETADO");
  Serial.println("✓ Todas las credenciales WiFi han sido borradas");
  Serial.println("✓ La ESP32 ya no recordará ninguna red");
  Serial.println("✓ Para conectar a una red, necesitarás configurarla nuevamente");
  Serial.println("========================================");
  
  // Indicar proceso completado con LEDs
  for(int i = 0; i < 5; i++) {
    digitalWrite(ledParpadeo, HIGH);
    digitalWrite(ledConectado, HIGH);
    delay(300);
    digitalWrite(ledParpadeo, LOW);
    digitalWrite(ledConectado, LOW);
    delay(300);
  }
  
  Serial.println("Reiniciando en 5 segundos...");
  delay(5000);
  
  // Reiniciar el ESP32
  ESP.restart();
}

void loop() {
  // Este código solo se ejecuta una vez en setup()
  // El loop está vacío porque reiniciamos el dispositivo
}
