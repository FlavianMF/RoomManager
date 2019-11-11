#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define ssid "FABLAB"
#define pass "F@bl@b2017"

const String ORG = "20qyjr";
const String DEVICE_TYPE = "Esp";
const String DEVICE_ID = "0001";
#define DEVICE_TOKEN "sp24-iV(jAgHekMO-Y"

#define ledOnBoard 2

#define COMMAND_TOPIC_ROOMLIGHT "iot-2/cmd/roomLight/fmt/json"

const String CLIENT_ID = "d:" + ORG + ":" + DEVICE_TYPE + ":" + DEVICE_ID;

const String MQTT_SERVER = ORG + ".messaging.internetofthings.ibmcloud.com";

bool stateLight = false;

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER.c_str(), 1883, wifiClient);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Setup Init");

    pinMode(ledOnBoard, OUTPUT);
    connectWiFi();
    delay(2000);

    connectMQTTServer();
    Serial.println("Setup Finish");
}

void loop() {
    // put your main code here, to run repeatedly:
    client.loop();
}

void connectMQTTServer() {
    Serial.println("Connecting to MQTT Server...");
    // Se conecta com as credenciais obtidas no site do Watson IoT
    // quando cadastramos um novo device
    if (client.connect(CLIENT_ID.c_str(), "use-token-auth", DEVICE_TOKEN)) {
        // Se a conexão foi bem sucedida
        Serial.println("Connected to MQTT Broker");
        // Quando algo for postado em algum tópico que estamos inscritos
        // a função "callback" será executada
        client.setCallback(callback);
        // Se inscreve nos tópicos de interesse
        client.subscribe(COMMAND_TOPIC_ROOMLIGHT);
        digitalWrite(ledOnBoard, HIGH);
    } else {
        // Se ocorreu algum erro
        Serial.print("error = ");
        Serial.println(client.state());
        connectMQTTServer();  // tenta conectar novamente
    }
}

void callback(char* topic, unsigned char* payload, unsigned int length) {
    Serial.print("topic: ");
    Serial.println(topic);

    StaticJsonDocument<30> jsonBuffer;

    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    String value = jsonBuffer["command"];
    if (strcmp(topic, COMMAND_TOPIC_ROOMLIGHT) == 0) {
        digitalWrite(ledOnBoard, stateLight);
        stateLight = !stateLight;
    }
}

void connectWiFi() {
    WiFi.mode(WIFI_STA);
    if (!WiFi.begin(ssid, pass)) {
        Serial.println("Wifi Not Cennected");
    }
    Serial.println("Wifi Connected");
}
