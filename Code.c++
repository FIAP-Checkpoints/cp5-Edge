// Bibliotecas
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

// Rede de conexão
const char* ssid = "Wokwi-GUEST";  
const char* password = "";         
const char* mqtt_server = "4.228.58.205";  

// Pinos
const int DHT_PIN = 23;        // Pino do DHT22
const int LDR_PIN = 35;        // Pino do LDR sensor

// Cria objeto para o sensor DHT22
DHTesp dhtSensor;

// Cliente Wi-Fi e MQTT
WiFiClient WOKWI_client;
PubSubClient client(WOKWI_client);

// Função para configurar o Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Função para reconectar ao servidor MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Função para pegar os dados do sensor LDR (luminosidade)
void luminosity_dados() {
  int ldrValue = analogRead(LDR_PIN); // Lê o valor analógico do pino LDR_PIN
  
  // Mapeia o valor de 0-4095 para 0-100
  int mappedLdrValue = map(ldrValue, 0, 4095, 0, 100);

  client.publish("Azure/Luminosity", String(mappedLdrValue).c_str()); // Publica o valor de luminosidade no tópico MQTT
  delay(1000);
}

// Função para coletar dados do DHT22 e enviar para o MQTT
void DHT_dados() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  client.publish("Azure/Temperature", String(data.temperature).c_str());
  client.publish("Azure/Humidity", String(data.humidity).c_str());
  delay(1000);
}

// Função executada ao iniciar o ESP32
void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);

  // Configura o sensor DHT22
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
}

// Função executada em loop que chama outras funções
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Envia dados dos sensores
  DHT_dados();                // Envia dados do DHT22
  luminosity_dados();          // Envia dados do sensor de luminosidade (LDR)
}
