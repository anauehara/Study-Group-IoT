#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "Insira o Template ID"
#define BLYNK_TEMPLATE_NAME "Temperatura e Umidade"
#define BLYNK_AUTH_TOKEN "Inseira o Token"

// Bibliotecas necessárias
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Credenciais de rede
char ssid[] = "Nome da Rede Wi-Fi";
char pass[] = "senha da Rede Wi-Fi";

// Sensor DHT11 no GPIO2
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Função que envia dados ao Blynk
void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Falha na leitura do DHT!");
    return;
  }

  Blynk.virtualWrite(V0, h); // Umidade
  Blynk.virtualWrite(V6, t); // Temperatura
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  timer.setInterval(1000L, sendSensor); // A cada 1 segundo
}

void loop() {
  Blynk.run();
  timer.run();
}