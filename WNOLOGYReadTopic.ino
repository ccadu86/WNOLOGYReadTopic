#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "";                    // SSID da sua rede Wi-Fi
const char* password = "";               // Senha da sua rede Wi-Fi

//////////////////////////////////////////////////////////////////////////////////
const char* broker = "broker.app.wnology.io";          // Endereço do broker MQTT
const int port = 1883;                                 // Porta do broker MQTT
const char* accessKey = "";     // Chave de acesso ao broker MQTT
const char* accessSecret = "";  // Segredo de acesso ao broker MQTT
const char* topic = "wnology//state";         // Tópico MQTT a ser assinado
const char* clientID = "";     // MQTT Client ID

WiFiClient wifiClient;              // Cliente WiFi
PubSubClient mqttClient(wifiClient); // Cliente MQTT

void setup() {
  Serial.begin(115200); // Inicializa a comunicação serial
  setupWiFi();        // Configura a conexão Wi-Fi
  mqttClient.setServer(broker, port); // Configura o servidor MQTT
  mqttClient.setCallback(callback);   // Configura a função de retorno de chamada MQTT
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
  Serial.println("Mensagem recebida:");

  // Imprimir o tópico
  Serial.print("Tópico: ");
  Serial.println(topic);

  // Imprimir o payload
  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Criar uma quebra de linha para separar mensagens
  Serial.println();
}
