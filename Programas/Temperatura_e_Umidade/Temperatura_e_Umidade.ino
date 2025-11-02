#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL2yBO12Vfy"
#define BLYNK_TEMPLATE_NAME "Temperatura e umidade"
#define BLYNK_AUTH_TOKEN "-HHNSM_AGykkVSHdZk4aCL5dti4-XTqn"

// Bibliotecas necessárias
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Credenciais de rede
char ssid[] = "###";        // Nome da rede Wi-Fi
char pass[] = "###";

// Sensor DHT11 no GPIO4
#define DHTPIN 5
#define DHTTYPE DHT11

// LEDs
const int LED_ALERTA = 19; // LED de alerta
const int LED_FIXO = 35;   // LED fixo sempre ligado

// Temperatura limite inicial (padrão)
float temperaturaLimite = 5.0;
float umidadeLimite = 5.0;
bool alertaAtivo = false;

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Função que envia dados ao Blynk
void sendSensor() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // 1. Inicia a verificação resetando o estado de alerta a cada ciclo
    alertaAtivo = false; 

    if (isnan(h) || isnan(t)) {
        Serial.println("Falha na leitura do DHT!");
        return;
    }

    Blynk.virtualWrite(V0, h); // Umidade
    Blynk.virtualWrite(V6, t); // Temperatura

    // ----------------------------------------------------
    // Lógica do LED de alerta
    // ----------------------------------------------------
    
    // 2. Verifica a Temperatura (SÓ ATIVA O FLAG)
    if (t >= temperaturaLimite) {
        alertaAtivo = true; 
        Serial.print("ALERTA: Temperatura (");
        Serial.print(t);
        Serial.print(" °C) acima do limite...");
        Serial.println(" °C!");
    } 

    // 3. Verifica a Umidade (SÓ ATIVA O FLAG)
    if (h <= umidadeLimite) {
        alertaAtivo = true; 
        Serial.print("ALERTA: umidade (");
        Serial.print(h);
        Serial.print(" %) acima do limite...");
        Serial.println(" %!");
    }
    
    // 4. Controle Final Único
    if (alertaAtivo) {
        digitalWrite(LED_ALERTA, HIGH);
    } else {
        // O LED SÓ É DESLIGADO se NENHUM dos IFs acima for TRUE
        digitalWrite(LED_ALERTA, LOW);
    }
}

// Recebe o valor do slider/número no Blynk
BLYNK_WRITE(V1) {
  temperaturaLimite = param.asFloat(); // atualiza o limite dinamicamente
  Serial.print("Novo limite de temperatura recebido: ");
  Serial.println(temperaturaLimite);
}

BLYNK_WRITE(V2) {
  umidadeLimite = param.asFloat(); // atualiza o limite dinamicamente
  Serial.print("Novo limite de umidade recebido: ");
  Serial.println(umidadeLimite);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_ALERTA, OUTPUT);
  pinMode(LED_FIXO, OUTPUT);

  digitalWrite(LED_FIXO, HIGH); // sempre ligado

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  timer.setInterval(2000L, sendSensor); // atualiza a cada 2s
}

void loop() {
  Blynk.run();
  timer.run();
}
