#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "esp";                    // SSID da sua rede Wi-Fi
const char* password = "teste32esp";               // Senha da sua rede Wi-Fi

//////////////////////////////////////////////////////////////////////////////////
const char* broker = "broker.app.wnology.io";          // Endereço do broker MQTT
const int port = 1883;                                 // Porta do broker MQTT
const char* accessKey = "d69d796a-1251-484f-9d6f-3622d0fbad0e";     // Chave de acesso ao broker MQTT
const char* accessSecret = "99d839dea808f2cc4dc6a6bfd61e54c23cad9480f8de473a56a60cc8964f8b9d";  // Segredo de acesso ao broker MQTT
const char* topic = "wnology/64947c24ce6b4c71f6c8369a/state";         // Tópico MQTT a ser assinado
const char* clientID = "64947c24ce6b4c71f6c8369a";     // MQTT Client ID

WiFiClient wifiClient;              // Cliente WiFi
PubSubClient mqttClient(wifiClient); // Cliente MQTT

int temperatura = 0;  // Variável para armazenar a temperatura
String ligaAR = "";  // Variável para armazenar o estado do ar condicionado

int led = 2;

void setup() {
  Serial.begin(115200); // Inicializa a comunicação serial
  setupWiFi();        // Configura a conexão Wi-Fi
  mqttClient.setServer(broker, port); // Configura o servidor MQTT
  mqttClient.setCallback(callback);   // Configura a função de retorno de chamada MQTT
  pinMode(led, OUTPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect(); // Reconecta-se ao broker MQTT, se necessário
  }
  mqttClient.loop(); // Mantém a conexão MQTT ativa
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se à rede Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Conecta-se à rede Wi-Fi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede Wi-Fi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP()); // Imprime o endereço IP atribuído
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT...");

    if (mqttClient.connect(clientID, accessKey, accessSecret)) {
      Serial.println("Conectado");
      mqttClient.subscribe(topic); // Assina o tópico MQTT
    } else {
      Serial.print("Falha ao conectar, código de erro: ");
      Serial.print(mqttClient.state()); // Imprime o código de erro de conexão MQTT
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida do tópico: ");
  Serial.println(topic); // Imprime o tópico da mensagem recebida

  DynamicJsonDocument doc(256);  // Cria um documento JSON dinâmico com tamanho de 256 bytes
  deserializeJson(doc, payload, length);  // Analisa o payload JSON recebido

  const JsonObject& data = doc["data"];  // Obtém o objeto JSON "data" do documento

  if (data.containsKey("TEMP")) {
    temperatura = data["TEMP"].as<int>();  // Extrai o valor de "TEMP" e atribui à variável temperatura
    Serial.print("Temperatura: ");
    Serial.println(temperatura); // Imprime a temperatura
  }

  if (data.containsKey("LIGA_AR")) {
    ligaAR = data["LIGA_AR"].as<String>();  // Extrai o valor de "LIGA_AR" e atribui à variável ligaAR
    Serial.print("Ligar AR: ");
    Serial.println(ligaAR); // Imprime o estado do ar condicionado
    if (ligaAR == "true"){
      digitalWrite(led, LOW);
    }
    else{
      digitalWrite(led, HIGH);
    }
  }
}
