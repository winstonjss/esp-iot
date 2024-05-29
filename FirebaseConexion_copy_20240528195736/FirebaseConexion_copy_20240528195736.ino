#include <ESP32Firebase.h>

#define _SSID "espiot"          // Your WiFi SSID
#define _PASSWORD "12345679"      // Your WiFi Password
#define REFERENCE_URL "https://fir-esp-1e388-default-rtdb.firebaseio.com/"  // Your Firebase project reference url

Firebase firebase(REFERENCE_URL);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
    connectionAttempts++;

    if(connectionAttempts > 20) { // Si no se puede conectar después de 20 intentos, reinicia el ESP32
      Serial.println("No se pudo conectar a la WiFi, reiniciando...");
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  if (Serial2.available()) {
    delay(1000);
    String data = Serial2.readStringUntil('\n');
    if (data.length() > 0) { // Asegúrate de que la cadena de datos no esté vacía
      int firstCommaIndex = data.indexOf(',');
      int secondCommaIndex = data.lastIndexOf(',');
      
      float humedad = data.substring(0, firstCommaIndex).toFloat();
      float temperatura = data.substring(firstCommaIndex + 1, secondCommaIndex).toFloat();
      int humedadSueloPorcentaje = data.substring(secondCommaIndex + 1).toInt();

      Serial.print(humedad);
      Serial.print(",");
      Serial.print(temperatura);
      Serial.print(",");
      Serial.println(humedadSueloPorcentaje);

      // Ahora puedes usar estos valores para hacer lo que necesites, por ejemplo, subirlos a Firebase.
      if(humedad != 0) {
        firebase.setFloat("HumedadAmbiente", humedad);
      }
      if(temperatura != 0) {
        firebase.setFloat("TemperaturaAmbiente", temperatura);
      }
      if(humedadSueloPorcentaje != 0) {
        firebase.setInt("HumedadSuelo", humedadSueloPorcentaje);
      }
    }
  }
}