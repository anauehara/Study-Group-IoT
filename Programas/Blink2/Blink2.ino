#define BLYNK_TEMPLATE_ID "TMPL2q0qL0SbQ"
#define BLYNK_TEMPLATE_NAME "Controle Leds"
#define BLYNK_AUTH_TOKEN "2P6aaGaVw1TD8Ij4J7njaFhW02719AXn"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Autenticação Blynk
char auth[] = BLYNK_AUTH_TOKEN;

// Rede wi-fi
char ssid[] = "MANU";
char pass[] = "MANU2402";

// Pinos dos LEDs
const int LED_VERMELHO = 32;
const int LED_AMARELO = 26;
const int LED_VERDE = 13;

// Controle do LED vermelho (V0)
BLYNK_WRITE(V0) {
  int valor = param.asInt();
  digitalWrite(LED_VERMELHO, valor);
}

// Controle do LED amarelo (V1)
BLYNK_WRITE(V1) {
  int valor = param.asInt();
  digitalWrite(LED_AMARELO, valor);
}

// Controle do LED verde (V2)
BLYNK_WRITE(V2) {
  int valor = param.asInt();
  digitalWrite(LED_VERDE, valor);
}

void setup() {
  // Inicialização da serial
  Serial.begin(115200);

  // Inicialização do Blynk
  Blynk.begin(auth, ssid, pass);

  // configura os pinos como saída
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
}

void loop() {
  Blynk.run();
}
