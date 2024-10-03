
# Monitoramento IoT com ESP32 usando DHT22, LDR e MQTT

**Membros do Projeto:**
- **Caio S. F. da Silva**: RM 554763
- **Matheus R. Montovaneli**: RM 555499
- **Lucas Vasquez Silva**: RM 555159
- **Guilherme L. F. R. Gozzi**: RM 555768
- **André Nakamatsu Rocha**: RM 555004

---

Este projeto utiliza um **ESP32** para ler dados de dois sensores: um **DHT22** (para temperatura e umidade) e um **LDR** (para luminosidade). Esses dados são enviados para um **broker MQTT**, onde podem ser visualizados ou utilizados em outros sistemas.

## Link para o projeto
https://wokwi.com/projects/410551824578875393

## Componentes Usados

- **ESP32**: O microcontrolador usado para executar o código.
- **DHT22**: Sensor para medir temperatura e umidade.
- **LDR**: Sensor para medir a luminosidade (nível de luz).
- **MQTT Broker**: Servidor que recebe os dados publicados pelos sensores.

## Como Funciona

1. **Conexão Wi-Fi**: O ESP32 se conecta à rede Wi-Fi configurada.
2. **MQTT**: Uma vez conectado ao Wi-Fi, o ESP32 se conecta a um broker MQTT.
3. **Leitura de Sensores**:
   - O **DHT22** coleta a temperatura e umidade.
   - O **LDR** coleta o nível de luminosidade ambiente.
4. **Publicação de Dados**: Os dados dos sensores são enviados para o broker MQTT em tópicos específicos:
   - Temperatura é publicada em `Azure/Temperature`.
   - Umidade é publicada em `Azure/Humidity`.
   - Luminosidade é publicada em `Azure/Luminosity`.

## Configurações Necessárias

### 1. Rede Wi-Fi
No código, você precisará definir o SSID e a senha da sua rede Wi-Fi:

```cpp
const char* ssid = "Wokwi-GUEST";  
const char* password = "";  
```

### 2. Broker MQTT
Você também precisará especificar o endereço IP do **broker MQTT**. Neste exemplo, estamos usando o IP `4.228.58.205` na porta 1883:

```cpp
const char* mqtt_server = "4.228.58.205";  
```

### 3. Pinos dos Sensores
- O **DHT22** está conectado ao pino **23** do ESP32.
- O **LDR** está conectado ao pino **35**.

## Código Explicado

### Função `setup_wifi()`

Esta função configura a conexão Wi-Fi do ESP32. Ela tenta conectar o dispositivo à rede especificada até obter sucesso.

```cpp
void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
```

### Função `reconnect()`

Esta função garante que o ESP32 esteja sempre conectado ao **broker MQTT**. Se a conexão for perdida, o ESP32 tenta reconectar automaticamente.

```cpp
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Client")) {
      Serial.println("connected");
    } else {
      delay(5000);
    }
  }
}
```

### Função `DHT_dados()`

Esta função lê a temperatura e umidade do sensor **DHT22** e publica esses dados nos tópicos MQTT correspondentes (`Azure/Temperature` e `Azure/Humidity`).

```cpp
void DHT_dados() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  client.publish("Azure/Temperature", String(data.temperature).c_str());
  client.publish("Azure/Humidity", String(data.humidity).c_str());
}
```

### Função `luminosity_dados()`

Esta função lê os dados de luminosidade do sensor **LDR**, mapeia os valores lidos (que variam de 0 a 4095) para uma escala de 0 a 100, e publica esses dados no tópico MQTT `Azure/Luminosity`.

```cpp
void luminosity_dados() {
  int ldrValue = analogRead(LDR_PIN);  
  int mappedLdrValue = map(ldrValue, 0, 4095, 0, 100);
  client.publish("Azure/Luminosity", String(mappedLdrValue).c_str());
}
```

### Função `setup()`

A função `setup()` é executada uma vez quando o ESP32 é ligado. Ela configura o Wi-Fi, o cliente MQTT e inicializa o sensor **DHT22**.

```cpp
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
}
```

### Função `loop()`

A função `loop()` mantém o ESP32 conectado ao broker MQTT e envia os dados dos sensores em intervalos regulares.

```cpp
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  DHT_dados();
  luminosity_dados();
}
```

## Como Usar

1. **Conecte o ESP32 aos sensores DHT22 e LDR**:
   - DHT22 no pino 23.
   - LDR no pino 35.
   
2. **Configure o broker MQTT**:
   - Certifique-se de que o broker MQTT esteja rodando no IP e porta corretos.

3. **Faça upload do código no ESP32**:
   - Use o Arduino IDE para carregar o código no ESP32.

4. **Assine os tópicos no broker MQTT**:
   - Você pode usar ferramentas como **MQTT Explorer**, **Node-RED**, ou um aplicativo móvel MQTT para visualizar os dados publicados nos tópicos:
     - `Azure/Temperature` para temperatura.
     - `Azure/Humidity` para umidade.
     - `Azure/Luminosity` para luminosidade.

## Licença

Este projeto é de código aberto e pode ser modificado livremente. Atribuição é apreciada.

