/*****************************************************
 * PROJETO: Detector de Movimento com ESP32 + Blynk + HC-SR501

 * DESCRIÇÃO:
 *   - Detecta movimento com o sensor PIR (HC-SR501)
 *   - Envia estado para o Blynk
 *   - Acende LED (pino 18) ao detectar movimento
 *   - Mantém o LED aceso por um tempo configurável (V3)
 *   - Permite inverter a lógica com botão (V1)
 *****************************************************/

// ----------------------------------------------------
// CONFIGURAÇÕES DO BLYNK
// ----------------------------------------------------
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2dVP325wU"
#define BLYNK_TEMPLATE_NAME "Movimento"
#define BLYNK_AUTH_TOKEN "8pqf2OEoqgnEiqRypSWAKhDf6l7d0veE"

// ----------------------------------------------------
// BIBLIOTECAS NECESSÁRIAS
// ----------------------------------------------------
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// ----------------------------------------------------
// CREDENCIAIS DE REDE WI-FI
// ----------------------------------------------------
char ssid[] = "###";        // Nome da rede Wi-Fi
char pass[] = "###";

// ----------------------------------------------------
// DEFINIÇÃO DOS PINOS
// ----------------------------------------------------
const int PIR_PIN = 4;        // Sensor de movimento HC-SR501
const int LED_ALERTA = 21;    // LED que acende conforme movimento
const int LED_FIXO = 35;      // LED fixo (sempre aceso)

// ----------------------------------------------------
// DEFINIÇÃO DE PINOS VIRTUAIS NO BLYNK
// ----------------------------------------------------
#define VIRTUAL_MOVIMENTO V0   // Mostra no app se há movimento
#define VIRTUAL_LOGICA    V1   // Botão no app que inverte a lógica
#define VIRTUAL_TEMPO_LED V3   // Define tempo que o LED fica aceso após movimento

// ----------------------------------------------------
// VARIÁVEIS GLOBAIS
// ----------------------------------------------------
bool inverterLogica = false;        // false = LED acende com movimento / true = acende sem movimento
unsigned long tempoLED = 5000;      // tempo que o LED permanece aceso (em milissegundos)
unsigned long ultimoMovimento = 0;  // guarda o último momento em que houve movimento

BlynkTimer timer;

/*****************************************************
 * BOTÃO NO APP (V1)
 * Inverte a lógica de funcionamento do LED
 *****************************************************/
BLYNK_WRITE(VIRTUAL_LOGICA) {
  inverterLogica = param.asInt();
  if (inverterLogica)
    Serial.println("Modo invertido ativado: LED acende sem movimento.");
  else
    Serial.println("Modo normal ativado: LED acende com movimento.");
}

/*****************************************************
 * SLIDER NO APP (V3)
 * Define quanto tempo o LED ficará aceso após detectar movimento
 *****************************************************/
BLYNK_WRITE(VIRTUAL_TEMPO_LED) {
  int segundos = param.asInt();
  if (segundos <= 0) segundos = 1;  // tempo mínimo de 1 segundo
  tempoLED = segundos * 1000UL;
  Serial.print("Tempo de permanência do LED ajustado para ");
  Serial.print(segundos);
  Serial.println(" segundos.");
}

/*****************************************************
 * FUNÇÃO: sendSensor()
 * Lê o sensor e mantém o LED aceso pelo tempo configurado.
 *****************************************************/
void sendSensor() {
  int movimento = digitalRead(PIR_PIN);
  Blynk.virtualWrite(VIRTUAL_MOVIMENTO, movimento);

  unsigned long agora = millis();
  bool acenderLED = false;

  if (!inverterLogica) {
    // --- MODO NORMAL: LED acende com movimento ---
    if (movimento == HIGH) {
      ultimoMovimento = agora;  // registra o momento do movimento
      acenderLED = true;
      Serial.println("Movimento detectado!");
    } else if (agora - ultimoMovimento < tempoLED) {
      // mantém o LED aceso pelo tempo configurado
      acenderLED = true;
    }
  } else {
    // --- MODO INVERTIDO: LED acende sem movimento ---
    if (movimento == LOW) {
      ultimoMovimento = agora;
      acenderLED = true;
      Serial.println("Sem movimento (modo invertido).");
    } else if (agora - ultimoMovimento < tempoLED) {
      acenderLED = true;
    }
  }

  // Atualiza o LED físico
  digitalWrite(LED_ALERTA, acenderLED ? HIGH : LOW);

  // Log no monitor serial
  Serial.println(acenderLED ? "LED 18 ACESO" : "LED 18 APAGADO");
}

/*****************************************************
 * FUNÇÃO: setup()
 *****************************************************/
void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando o sistema...");

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_ALERTA, OUTPUT);
  pinMode(LED_FIXO, OUTPUT);

  digitalWrite(LED_FIXO, HIGH);
  Serial.println("LED 19 ligado permanentemente.");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Conectado ao Blynk com sucesso!");

  // Verifica o sensor rapidamente (a cada 200ms)
  timer.setInterval(200L, sendSensor);
}

/*****************************************************
 * FUNÇÃO: loop()
 *****************************************************/
void loop() {
  Blynk.run();
  timer.run();
}
