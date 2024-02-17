#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "Wokwi-GUEST";                    // SSID da sua rede Wi-Fi
const char* password = "";               // Senha da sua rede Wi-Fi

//////////////////////////////////////////////////////////////////////////////////
const char* broker = "broker.app.wnology.io";          // Endereço do broker MQTT
const int port = 1883;                                 // Porta do broker MQTT
const char* accessKey = "ef9231d3-f5ba-42e3-a9e2-2350c06e5a14";     // Chave de acesso ao broker MQTT
const char* accessSecret = "6c67680442d4a877040b5d3f4252e776a6faaf8cc59bebc3394d42e6da76cd28";  // Segredo de acesso ao broker MQTT
const char* topic = "wnology/65d0abf1caa6d9059dc9b624/state";         // Tópico MQTT a ser assinado
const char* clientID = "65d0abf1caa6d9059dc9b624";     // MQTT Client ID

WiFiClient wifiClient;              // Cliente WiFi
PubSubClient mqttClient(wifiClient); // Cliente MQTT


void setup() {
  Serial.begin(115200); // Inicializa a comunicação serial
  setupWiFi();        // Configura a conexão Wi-Fi
  mqttClient.setServer(broker, port); // Configura o servidor MQTT
  mqttClient.setCallback(callback);   // Configura a função de retorno de chamada MQTT
  pinMode(27, OUTPUT);
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
  int temperatura = 0; 
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

  DynamicJsonDocument doc(256);  // Cria um documento JSON dinâmico com tamanho de 256 bytes
  deserializeJson(doc, payload, length);  // Analisa o payload JSON recebido

  const JsonObject& data = doc["data"];  // Obtém o objeto JSON "data" do documento

  if (data.containsKey("TEMP")) {
    temperatura = data["TEMP"].as<int>();  // Extrai o valor de "TEMP" e atribui à variável temperatura
    Serial.print("Temperatura: ");
    Serial.println(temperatura); // Imprime a temperatura
    if (temperatura >= 50){
      digitalWrite(27, HIGH);
      Serial.println("LED LIGADO");
    }
    else{
      digitalWrite(27, LOW);
      Serial.println("LED DESLIGADO");
    }

  // Criar uma quebra de linha para separar mensagens
  Serial.println();
}
}
