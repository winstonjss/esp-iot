#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define SENSOR_HUMEDAD_SUELO_PIN A0 // Pin del sensor de humedad del suelo
#define MOTOR_PIN 3 

const long TIEMPO_ESPERA_INICIAL = 10000; // Tiempo de espera inicial antes de permitir el riego (en milisegundos)
const long TIEMPO_MAX_RIEGO = 2000; // Tiempo máximo de riego en milisegundos (10 minutos)
const long TIEMPO_ACTIVACION = 1000; // Define la duración de la activación del motor antes de verificar la humedad del suelo nuevamente (en milisegundos).

const long RETARDO_VERIFICACION = 1000; // Retraso entre verificaciones de humedad del suelo en milisegundos (1 minuto)
const int umbralHumedadSuelo = 35; // Umbral de humedad del suelo para activar el riego

DHT dht(DHTPIN, DHTTYPE);

unsigned long tiempoInicioActivacion = 0; // Variable para almacenar el tiempo de inicio de la activación del motor
unsigned long tiempoInicioRiego = 0; // Variable para almacenar el tiempo de inicio del riego
bool motorActivado = false; // Variable para rastrear si el motor ha sido activado recientemente

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

}

void leerSensores(float& temperatura, float& humedad, int& humedadSueloPorcentaje) {
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();

  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println("Error al leer el sensor DHT!");
    return;
  }

  humedadSueloPorcentaje = map(analogRead(SENSOR_HUMEDAD_SUELO_PIN), 1023, 0, 0, 100);
}

void controlarRiego(const float& temperatura, const float& humedad, const int& humedadSueloPorcentaje) {
  if (millis() >= TIEMPO_ESPERA_INICIAL && humedadSueloPorcentaje < umbralHumedadSuelo) {
    if (!motorActivado) {
      tiempoInicioActivacion = millis();
      delay(5000); // Implementar un retardo de 5 segundos antes de activar el motor
      digitalWrite(MOTOR_PIN, LOW);
      motorActivado = true;
      tiempoInicioRiego = millis(); // Guardar el tiempo de inicio del riego
    } else if (millis() - tiempoInicioRiego >= TIEMPO_MAX_RIEGO) {
      digitalWrite(MOTOR_PIN, HIGH);
      motorActivado = false;
    } else if (millis() - tiempoInicioActivacion >= TIEMPO_ACTIVACION) {
      digitalWrite(MOTOR_PIN, HIGH);
      motorActivado = false;
    }
  } else {
    digitalWrite(MOTOR_PIN, LOW);
    motorActivado = false;
  }
}

void verificarHumedadSuelo() {
  if (!motorActivado && millis() - tiempoInicioRiego >= RETARDO_VERIFICACION) {
    tiempoInicioRiego = millis(); // Actualizar el tiempo de inicio del riego
    int humedadSuelo = analogRead(SENSOR_HUMEDAD_SUELO_PIN);
    int humedadSueloPorcentaje = map(humedadSuelo, 1023, 0, 0, 100);
  }
}

void loop() {
  delay(2000); // Espera dos segundos entre mediciones

  float temperatura, humedad;
  int humedadSueloPorcentaje;

  leerSensores(temperatura, humedad, humedadSueloPorcentaje);
  controlarRiego(temperatura, humedad, humedadSueloPorcentaje);
  verificarHumedadSuelo();

  Serial.print("Humedad ambiente: ");
  Serial.print(humedad);
  Serial.print(" %\t");
  Serial.print("Temperatura ambiente: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  Serial.print("Humedad del Suelo: ");
  Serial.print(humedadSueloPorcentaje);
  Serial.println(" %\t");
  
  Serial.print(humedad);
  Serial.print(",");
  Serial.print(temperatura);
  Serial.print(",");
  Serial.println(humedadSueloPorcentaje);
}
