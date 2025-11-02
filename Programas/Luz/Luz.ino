#define BLYNK_PRINT Serial  // Habilita mensagens do Blynk no monitor serial

// Informações do projeto criadas no painel do Blynk
#define BLYNK_TEMPLATE_ID "TMPL2SYV3HrX0"
#define BLYNK_TEMPLATE_NAME "Luz"
#define BLYNK_AUTH_TOKEN "KWT1OThAjFuLnq1Pwb0M8SVI2AQkMjcI"

// ----------------------------------------------------
// BIBLIOTECAS NECESSÁRIAS
// ----------------------------------------------------
#include <WiFi.h>              // Biblioteca de conexão Wi-Fi do ESP32
#include <WiFiClient.h>        // Cliente TCP/IP
#include <BlynkSimpleEsp32.h>  // Integração do ESP32 com o Blynk

// ----------------------------------------------------
// CREDENCIAIS DE REDE WI-FI
// ----------------------------------------------------
char ssid[] = "###";        // Nome da rede Wi-Fi
char pass[] = "###";   // Senha da rede Wi-Fi

// ----------------------------------------------------
// DEFINIÇÃO DOS PINOS FÍSICOS
// ----------------------------------------------------
const int LDR_PIN = 15;          // Pino onde o LDR está conectado (D4)
const int LED_ALERTA = 18;      // LED de alerta (D18)
const int LED_FIXO = 35;        // LED que fica sempre aceso (D19)

// ----------------------------------------------------
// DEFINIÇÃO DE PINOS VIRTUAIS DO BLYNK
// ----------------------------------------------------
#define VIRTUAL_LUMINOSIDADE V0   // Envia leitura da luz (display)
#define VIRTUAL_LOGICA       V1   // Botão para inverter lógica no app

// ----------------------------------------------------
// VARIÁVEIS GLOBAIS
// ----------------------------------------------------
bool inverterLogica = false;   // false = LED acende com LUZ, true = LED acende no ESCURO
BlynkTimer timer;              // Cria o timer do Blynk (executa funções periodicamente)

/*****************************************************
 * FUNÇÃO: BLYNK_WRITE(V1)
 * OBJETIVO:
 *   Lida com o botão no aplicativo Blynk (pino virtual V1)
 *   e altera o modo de funcionamento da lógica do LED.
 *****************************************************/
BLYNK_WRITE(VIRTUAL_LOGICA) {
    inverterLogica = param.asInt(); // Lê o valor do botão (0 ou 1)
    if (inverterLogica) {
        Serial.println("Modo invertido ativado: LED acende no ESCURO.");
    } else {
        Serial.println("Modo normal ativado: LED acende com LUZ.");
    }
}

/*****************************************************
 * FUNÇÃO: sendSensor()
 * OBJETIVO:
 *   Ler o sensor LDR, enviar o valor para o Blynk
 *   e controlar o LED de alerta conforme a lógica atual.
 *****************************************************/
void sendSensor() {
    int luz_status = digitalRead(LDR_PIN);  // Lê o valor digital do LDR (0 = escuro, 1 = luz)
    Blynk.virtualWrite(VIRTUAL_LUMINOSIDADE, luz_status); // Envia leitura para o app

    // --------------------------------------------------------
    // DECISÃO DE ACENDIMENTO DO LED
    // --------------------------------------------------------
    bool acenderLED;

    if (!inverterLogica) {
        // MODO NORMAL → LED acende quando há luz
        acenderLED = (luz_status == HIGH);
    } else {
        // MODO INVERTIDO → LED acende quando está escuro
        acenderLED = (luz_status == LOW);
    }

    // Aplica o estado ao LED físico
    digitalWrite(LED_ALERTA, acenderLED ? HIGH : LOW);

    // Exibe informações no monitor serial
    if (acenderLED) {
        Serial.println("LED 18 ACESO (condição verdadeira).");
    } else {
        Serial.println("LED 18 APAGADO (condição falsa).");
    }
}

/*****************************************************
 * FUNÇÃO: setup()
 * OBJETIVO:
 *   Configurar os pinos, iniciar o Serial e o Blynk,
 *   e agendar as leituras periódicas do sensor.
 *****************************************************/
void setup() {
    Serial.begin(9600); // Inicia comunicação serial
    Serial.println("Iniciando o sistema...");

    // Configuração dos pinos
    pinMode(LDR_PIN, INPUT);        // LDR como entrada
    pinMode(LED_ALERTA, OUTPUT);    // LED de alerta como saída
    pinMode(LED_FIXO, OUTPUT);      // LED fixo como saída

    // Liga o LED fixo permanentemente
    digitalWrite(LED_FIXO, HIGH);
    Serial.println("LED 19 ligado permanentemente.");

    // Conecta ao Blynk e à rede Wi-Fi
    Serial.println("Conectando ao Blynk...");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Serial.println("Conectado ao Blynk com sucesso!");

    // Executa a função sendSensor() a cada 2 segundos (2000 milissegundos)
    timer.setInterval(2000L, sendSensor);
}

/*****************************************************
 * FUNÇÃO: loop()
 * OBJETIVO:
 *   Mantém o Blynk e o timer funcionando continuamente.
 *****************************************************/
void loop() {
    Blynk.run();  // Mantém conexão com o servidor Blynk
    timer.run();  // Executa as funções temporizadas (como sendSensor)
}