#include <WiFi.h>
#include <WebServer.h>

// --- Configurações da rede Wi-Fi ---
const char* ssid = "####";     
const char* password = "###";

// --- Pinos dos LEDs ---
const int ledVermelho = 23;
const int ledAmarelo  = 22;
const int ledVerde    = 21;

// --- Cria o servidor web ---
WebServer server(80);

// --- Página HTML ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Controle de LEDs</title>
<style>
  body { background: #111; color: #fff; font-family: sans-serif; text-align:center; padding:40px; }
  button { font-size:20px; margin:10px; padding:20px; border:none; border-radius:10px; cursor:pointer; }
  .on { background:#0f0; color:#000; }
  .off { background:#333; color:#aaa; }
</style>
</head>
<body>
  <h1>Controle de LEDs</h1>
  <button id="vermelho" class="off" onclick="toggle('vermelho')">Vermelho</button>
  <button id="amarelo" class="off" onclick="toggle('amarelo')">Amarelo</button>
  <button id="verde" class="off" onclick="toggle('verde')">Verde</button>

<script>
function toggle(cor){
  const b=document.getElementById(cor);
  const aceso=b.classList.contains('on');
  fetch(`/${cor}_${aceso?'off':'on'}`).then(()=> {
    b.classList.toggle('on');
    b.classList.toggle('off');
  });
}
</script>
</body>
</html>
)rawliteral";

// --- Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  // Garante que começam apagados
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVerde, LOW);

  // --- Conecta ao Wi-Fi ---
  Serial.println("\nConectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.print("🌐 IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // --- Rota principal ---
  server.on("/", []() {
    server.send_P(200, "text/html", index_html);
  });

  // --- Rotas dos LEDs ---
  server.on("/vermelho_on", [](){ digitalWrite(ledVermelho, HIGH); server.send(200, "text/plain", "OK"); });
  server.on("/vermelho_off", [](){ digitalWrite(ledVermelho, LOW);  server.send(200, "text/plain", "OK"); });

  server.on("/amarelo_on", [](){ digitalWrite(ledAmarelo, HIGH); server.send(200, "text/plain", "OK"); });
  server.on("/amarelo_off", [](){ digitalWrite(ledAmarelo, LOW);  server.send(200, "text/plain", "OK"); });

  server.on("/verde_on", [](){ digitalWrite(ledVerde, HIGH); server.send(200, "text/plain", "OK"); });
  server.on("/verde_off", [](){ digitalWrite(ledVerde, LOW);  server.send(200, "text/plain", "OK"); });

  // --- Inicia servidor ---
  server.begin();
  Serial.println("🚀 Servidor HTTP iniciado!");
}

// --- Loop ---
void loop() {
  server.handleClient();
}
