/*****************************************************
 * PROJETO UNIFICADO
 * ESP32 + Blynk + TCRT5000 + LDR + DHT11
 *
 * OBS:
 * - A LÓGICA ORIGINAL FOI MANTIDA EXATAMENTE IGUAL
 * - Apenas os PINOS foram reorganizados para evitar conflito
 * - HASHES DO BLYNK IGNORADOS CONFORME PEDIDO
 *****************************************************/

// ====================================================
// CONFIGURAÇÕES DO BLYNK
// ====================================================
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL22v6VhBqy"
#define BLYNK_TEMPLATE_NAME "Casa Automática"
#define BLYNK_AUTH_TOKEN "Ua7q1vzgTQnLYS3CItK4zM8NrtuVPAYz"

// ====================================================
// BIBLIOTECAS NECESSÁRIAS
// ====================================================
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// ====================================================
// CREDENCIAIS WI-FI
// ====================================================
char ssid[] = "...";
char pass[] = "...";

// ====================================================
// DEFINIÇÃO DOS PINOS
// ====================================================

// ---------- TCRT5000 ----------
const int SENSOR_PIN = 4;       // TCRT5000

// ---------- LDR ----------
const int LDR_PIN = 15;         // LDR

// ---------- DHT11 ----------
#define DHTPIN 5
#define DHTTYPE DHT11

// ---------- LEDs ----------
const int LED_TCRT = 21;        // LED do TCRT5000
const int LED_LDR = 18;         // LED do LDR
const int LED_DHT = 19;         // LED do DHT11
const int LED_FIXO = 23;        // LED sempre ligado

// ====================================================
// PINOS VIRTUAIS DO BLYNK
// ====================================================

// ---------- TCRT5000 ----------
#define VIRTUAL_MOVIMENTO V0
#define VIRTUAL_LOGICA_TCRT V1
#define VIRTUAL_TEMPO_LED V3

// ---------- LDR ----------
#define VIRTUAL_LUMINOSIDADE V4
#define VIRTUAL_LOGICA_LDR V5

// ---------- DHT11 ----------
#define VIRTUAL_UMIDADE V6
#define VIRTUAL_TEMPERATURA V7
#define VIRTUAL_TEMP_LIMITE V8
#define VIRTUAL_UMIDADE_LIMITE V9

// ====================================================
// VARIÁVEIS GLOBAIS - TCRT5000
// ====================================================
bool inverterLogicaTCRT = false;

unsigned long tempoLED = 5000;
unsigned long ultimoMovimento = 0;

// ====================================================
// VARIÁVEIS GLOBAIS - LDR
// ====================================================
bool inverterLogicaLDR = false;

// ====================================================
// VARIÁVEIS GLOBAIS - DHT11
// ====================================================
float temperaturaLimite = 5.0;
float umidadeLimite = 5.0;
bool alertaAtivo = false;

// ====================================================
// OBJETOS
// ====================================================
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// ====================================================
// TCRT5000 - BOTÃO APP
// ====================================================
BLYNK_WRITE(VIRTUAL_LOGICA_TCRT) {

  inverterLogicaTCRT = param.asInt();

  if (inverterLogicaTCRT)
    Serial.println("Modo invertido ativado.");
  else
    Serial.println("Modo normal ativado.");
}

// ====================================================
// TCRT5000 - SLIDER TEMPO
// ====================================================
BLYNK_WRITE(VIRTUAL_TEMPO_LED) {

  int segundos = param.asInt();

  if (segundos <= 0)
    segundos = 1;

  tempoLED = segundos * 1000UL;

  Serial.print("Tempo do LED: ");
  Serial.print(segundos);
  Serial.println(" segundos.");
}

// ====================================================
// LDR - BOTÃO APP
// ====================================================
BLYNK_WRITE(VIRTUAL_LOGICA_LDR) {

    inverterLogicaLDR = param.asInt();

    if (inverterLogicaLDR) {
        Serial.println("Modo invertido ativado: LED acende no ESCURO.");
    } else {
        Serial.println("Modo normal ativado: LED acende com LUZ.");
    }
}

// ====================================================
// DHT11 - LIMITE TEMPERATURA
// ====================================================
BLYNK_WRITE(VIRTUAL_TEMP_LIMITE) {

  temperaturaLimite = param.asFloat();

  Serial.print("Novo limite de temperatura recebido: ");
  Serial.println(temperaturaLimite);
}

// ====================================================
// DHT11 - LIMITE UMIDADE
// ====================================================
BLYNK_WRITE(VIRTUAL_UMIDADE_LIMITE) {

  umidadeLimite = param.asFloat();

  Serial.print("Novo limite de umidade recebido: ");
  Serial.println(umidadeLimite);
}

// ====================================================
// FUNÇÃO TCRT5000
// ====================================================
void sendTCRT() {

  // LOW  = objeto detectado
  // HIGH = sem objeto

  int leitura = digitalRead(SENSOR_PIN);

  bool movimento = (leitura == LOW);

  Blynk.virtualWrite(VIRTUAL_MOVIMENTO, movimento);

  unsigned long agora = millis();

  bool acenderLED = false;

  if (!inverterLogicaTCRT) {

    // MODO NORMAL
    if (movimento) {

      ultimoMovimento = agora;
      acenderLED = true;

      Serial.println("Objeto detectado!");

    } else if (agora - ultimoMovimento < tempoLED) {

      acenderLED = true;
    }

  } else {

    // MODO INVERTIDO
    if (!movimento) {

      ultimoMovimento = agora;
      acenderLED = true;

      Serial.println("Sem objeto (modo invertido).");

    } else if (agora - ultimoMovimento < tempoLED) {

      acenderLED = true;
    }
  }

  digitalWrite(LED_TCRT, acenderLED ? HIGH : LOW);

  Serial.println(acenderLED ? "LED TCRT ACESO" : "LED TCRT APAGADO");
}

// ====================================================
// FUNÇÃO LDR
// ====================================================
void sendLDR() {

    int luz_status = digitalRead(LDR_PIN);

    Blynk.virtualWrite(VIRTUAL_LUMINOSIDADE, luz_status);

    bool acenderLED;

    if (!inverterLogicaLDR) {

        // MODO NORMAL → LED acende quando há luz
        acenderLED = (luz_status == HIGH);

    } else {

        // MODO INVERTIDO → LED acende quando está escuro
        acenderLED = (luz_status == LOW);
    }

    digitalWrite(LED_LDR, acenderLED ? HIGH : LOW);

    if (acenderLED) {
        Serial.println("LED LDR ACESO (condição verdadeira).");
    } else {
        Serial.println("LED LDR APAGADO (condição falsa).");
    }
}

// ====================================================
// FUNÇÃO DHT11
// ====================================================
void sendDHT() {

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    alertaAtivo = false;

    if (isnan(h) || isnan(t)) {
        Serial.println("Falha na leitura do DHT!");
        return;
    }

    Blynk.virtualWrite(VIRTUAL_UMIDADE, h);
    Blynk.virtualWrite(VIRTUAL_TEMPERATURA, t);

    // Verifica temperatura
    if (t >= temperaturaLimite) {

        alertaAtivo = true;

        Serial.print("ALERTA: Temperatura (");
        Serial.print(t);
        Serial.print(" °C) acima do limite...");
        Serial.println(" °C!");
    }

    // Verifica umidade
    if (h <= umidadeLimite) {

        alertaAtivo = true;

        Serial.print("ALERTA: umidade (");
        Serial.print(h);
        Serial.print(" %) acima do limite...");
        Serial.println(" %!");
    }

    // Controle final
    if (alertaAtivo) {

        digitalWrite(LED_DHT, HIGH);

    } else {

        digitalWrite(LED_DHT, LOW);
    }
}

// ====================================================
// SETUP
// ====================================================
void setup() {

    Serial.begin(9600);

    Serial.println("Iniciando sistema unificado...");

    // ------------------------------------------------
    // CONFIGURAÇÃO DOS PINOS
    // ------------------------------------------------

    pinMode(SENSOR_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);

    pinMode(LED_TCRT, OUTPUT);
    pinMode(LED_LDR, OUTPUT);
    pinMode(LED_DHT, OUTPUT);
    pinMode(LED_FIXO, OUTPUT);

    // ------------------------------------------------
    // LED FIXO
    // ------------------------------------------------

    digitalWrite(LED_FIXO, HIGH);

    Serial.println("LED fixo ligado.");

    // ------------------------------------------------
    // BLYNK
    // ------------------------------------------------

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    Serial.println("Conectado ao Blynk!");

    // ------------------------------------------------
    // DHT
    // ------------------------------------------------

    dht.begin();

    // ------------------------------------------------
    // TIMERS
    // ------------------------------------------------

    timer.setInterval(200L, sendTCRT);
    timer.setInterval(2000L, sendLDR);
    timer.setInterval(2000L, sendDHT);
}

// ====================================================
// LOOP
// ====================================================
void loop() {

    Blynk.run();
    timer.run();
}