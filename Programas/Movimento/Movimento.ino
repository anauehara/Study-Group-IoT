// ----------------------------------------------------
// CONFIGURAÇÕES DO BLYNK
// ----------------------------------------------------
#define BLYNK_PRINT Serial  // Mostra mensagens do Blynk no Serial Monitor

// Informações do seu template no Blynk
#define BLYNK_TEMPLATE_ID "TMPL2dVP325wU"
#define BLYNK_TEMPLATE_NAME "Movimento"
#define BLYNK_AUTH_TOKEN "8pqf2OEoqgnEiqRypSWAKhDf6l7d0veE"

// ----------------------------------------------------
// BIBLIOTECAS NECESSÁRIAS
// ----------------------------------------------------
#include <WiFi.h>              // Biblioteca Wi-Fi do ESP32
#include <WiFiClient.h>        // Cliente TCP/IP
#include <BlynkSimpleEsp32.h>  // Integração ESP32 com o Blynk

// ----------------------------------------------------
// CREDENCIAIS DE REDE WI-FI
// ----------------------------------------------------
char ssid[] = "###";      // Nome da rede Wi-Fi
char pass[] = "###";   // Senha da rede Wi-Fi

// ----------------------------------------------------
// DEFINIÇÃO DOS PINOS
// ----------------------------------------------------
const int PIR_PIN = 4;        // Pino do sensor de movimento HC-SR501
const int LED_ALERTA = 21;    // LED de alerta (acende conforme movimento)
const int LED_FIXO = 19;      // LED que fica sempre aceso

// ----------------------------------------------------
// DEFINIÇÃO DE PINOS VIRTUAIS NO BLYNK
// ----------------------------------------------------
#define VIRTUAL_MOVIMENTO V0  // Mostra no app se há movimento
#define VIRTUAL_LOGICA    V1  // Botão no app que inverte a lógica

// ----------------------------------------------------
// VARIÁVEIS GLOBAIS
// ----------------------------------------------------
bool inverterLogica = false;   // false = LED acende com movimento / true = LED acende sem movimento
BlynkTimer timer;              // Controla a execução periódica das funções

/*****************************************************
 * FUNÇÃO: BLYNK_WRITE(V1)
 * OBJETIVO:
 *   Ler o botão no app Blynk (V1) para inverter a lógica
 *****************************************************/
BLYNK_WRITE(VIRTUAL_LOGICA) {
  inverterLogica = param.asInt();  // 0 ou 1 (false/true)
  
  if (inverterLogica) {
    Serial.println("Modo invertido ativado: LED acende sem movimento.");
  } else {
    Serial.println("Modo normal ativado: LED acende com movimento.");
  }
}

/*****************************************************
 * FUNÇÃO: sendSensor()
 * OBJETIVO:
 *   Ler o sensor PIR, enviar o valor para o Blynk,
 *   e acender/apagar o LED conforme a lógica atual.
 *****************************************************/
void sendSensor() {
  int movimento = digitalRead(PIR_PIN);  // Lê o sensor (1 = movimento detectado, 0 = sem movimento)
  Blynk.virtualWrite(VIRTUAL_MOVIMENTO, movimento); // Envia para o app Blynk

  // Determina se o LED deve acender
  bool acenderLED;

  if (!inverterLogica) {
    // MODO NORMAL → LED acende quando há movimento
    acenderLED = (movimento == HIGH);
  } else {
    // MODO INVERTIDO → LED acende quando NÃO há movimento
    acenderLED = (movimento == LOW);
  }

  // Controla o LED físico
  digitalWrite(LED_ALERTA, acenderLED ? HIGH : LOW);

  // Mensagens no monitor serial
  if (movimento == HIGH) {
    Serial.println("Movimento detectado!");
  } else {
    Serial.println("Sem movimento.");
  }

  if (acenderLED) {
    Serial.println("LED 18 ACESO (condição verdadeira).");
  } else {
    Serial.println("LED 18 APAGADO (condição falsa).");
  }
}

/*****************************************************
 * FUNÇÃO: setup()
 * OBJETIVO:
 *   Inicializar pinos, Wi-Fi, Blynk e timer.
 *****************************************************/
void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando o sistema...");

  // Configuração dos pinos
  pinMode(PIR_PIN, INPUT);        // Sensor PIR como entrada
  pinMode(LED_ALERTA, OUTPUT);    // LED de alerta
  pinMode(LED_FIXO, OUTPUT);      // LED fixo

  // Liga o LED fixo permanentemente
  digitalWrite(LED_FIXO, HIGH);
  Serial.println("LED 19 ligado permanentemente.");

  // Conecta ao Blynk
  Serial.println("Conectando ao Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Conectado ao Blynk com sucesso!");

  // Executa a leitura do sensor a cada 2 segundos
  timer.setInterval(1000L, sendSensor);
}

/*****************************************************
 * FUNÇÃO: loop()
 * OBJETIVO:
 *   Manter o Blynk e o timer ativos continuamente.
 *****************************************************/
void loop() {
  Blynk.run();  // Mantém a comunicação com o Blynk
  timer.run();  // Executa as funções agendadas
}
