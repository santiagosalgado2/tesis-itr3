#include <WiFi.h>
#include <WebServer.h>
#include <IRremote.hpp>  
//LIBRERIAS USADAS
// Definir el pin del emisor IR
const int irPin = 13;

// Crear un objeto WebServer en el puerto 80
WebServer server(80);

// Datos de la red Wi-Fi
const char* ssid = "RioTel_SalgadO";
const char* password = "dcfs5411";

// Función para manejar la recepción de señales y enviarlas por IR
void handleSendIR() {
  // Verificar si hay datos en el cuerpo de la solicitud
  if (server.hasArg("plain")) {
    // Obtener el cuerpo de la solicitud (se espera que sea un string de números separados por comas)
    // Se almacena en una variable llamada body
    String body = server.arg("plain");

    // Crear un vector para almacenar los datos procesados
    //El vector en este caso funciona como un array de enteros, que va a contener los valores de la señal IR
    std::vector<uint16_t> rawData;
    
    // Dividir el string en partes separadas por comas
    //Como se recibe un string con numeros separados con , se itera este string y por cada coma se separa el numero y se agrega al vector 
    int startIndex = 0;
    int commaIndex;
    // commaIndex va a almacenar los indices de las comas, y start index va a almacenar el indice del inicio del numero
    // Si es desigual a -1 quiere decir que hay una coma en el string con un numero adelante por lo que entra en el bucle
    while ((commaIndex = body.indexOf(',', startIndex)) != -1) {
      //Se extrae un substring entre el indice inicial y el de la coma, extrayendo el numero
      //Por ejemplo, suponiendo que el string pasado es "1,2,3,4", en la primera iteracion se extrae el substring entre 0 y 1, que es 1
      //En la segunda iteracion se extrae el substring entre 2 y 3, que es 2, y asi sucesivamente
      String numberStr = body.substring(startIndex, commaIndex);
      rawData.push_back(numberStr.toInt()); // Convertir a entero y agregar al vector
      startIndex = commaIndex + 1; // Avanzar al siguiente índice
    }
    // Agregar el último número (si existe)
    // Esto se hace ya que el ultimo valor no tiene una coma adelante
    // La funcion lenght devuelve la cantidad de caracteres, por lo que si el indice de la coma es menor a la cantidad de caracteres, significa que hay un numero al final
    if (startIndex < body.length()) {
      String numberStr = body.substring(startIndex);
      rawData.push_back(numberStr.toInt());
    }

    // Verificar que el array no esté vacío
    if (rawData.size() > 0) {
      // Emitir la señal IR utilizando la señal cruda (RAW) utilizando los valores del vector
      IrSender.sendRaw(rawData.data(), rawData.size(), 38);  

      // Responder al cliente
      server.send(200, "application/json", "{\"success\":\"Señal IR enviada correctamente.\"}");
    } else {
      // Enviar error si el array está vacío
      server.send(400, "application/json", "{\"error\":\"Array 'rawData' está vacío.\"}");
    }
  }
  else {
    // Enviar error si no hay datos en el cuerpo de la solicitud o parámetros necesarios
    server.send(400, "application/json", "{\"error\":\"Faltan datos en la solicitud.\"}");
  }
}

// Configurar Wi-Fi
void setupWiFi() {
  delay(100);
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Iniciar comunicación serial
  Serial.begin(115200);

  // Configurar el pin del emisor IR
  IrSender.begin(irPin); 
  // Configurar el Wi-Fi
  setupWiFi();

  // Definir la ruta para manejar las solicitudes IR
  server.on("/sendIR", handleSendIR);

  // Iniciar el servidor web
  server.begin();
  Serial.println("Servidor iniciado.");
}

void loop() {
  // Manejar las solicitudes del servidor
  server.handleClient();
}