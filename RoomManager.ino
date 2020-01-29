#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
// #include <WiFi.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// #define ssid "FABLAB"
// #define pass "F@bl@b2017"

#define ssid "COWORKING"
#define pass "coworking2018"

// #define ssid "NEW HOUSE"
// #define pass "E22C4156"

const String ORG = "20qyjr";
const String DEVICE_TYPE = "Esp";
const String DEVICE_ID = "0001";
#define DEVICE_TOKEN "sp24-iV(jAgHekMO-Y"

#define ledOnBoard 2
#define relay1Pin 1
#define relay2Pin 3

#define COMMAND_TOPIC_ROOMLIGHT "iot-2/cmd/roomLight/fmt/json"
#define COMMAND_TOPIC_LOADING "iot-2/cmd/loading/fmt/json"
#define COMMAND_TOPIC_OTA_ON "iot-2/cmd/OTA_ON/fmt/json"
#define COMMAND_TOPIC_OTA_OFF "iot-2/cmd/OTA_OFF/fmt/json"

#define COMMAND_EVENT_LOADING "iot-2/evt/loading/fmt/json"
#define COMMAND_EVENT_CONFIRM "iot-2/evt/confirm/fmt/json"

const String CLIENT_ID = "d:" + ORG + ":" + DEVICE_TYPE + ":" + DEVICE_ID;
const String MQTT_SERVER = ORG + ".messaging.internetofthings.ibmcloud.com";

bool stateLight = false;
bool stateLoading = false;

bool stateOTA = false;
bool OTA = false;

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER.c_str(), 1883, wifiClient);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Setup Init");

    pinMode(ledOnBoard, OUTPUT);
    pinMode(relay1Pin, OUTPUT);
    pinMode(relay2Pin, OUTPUT);

    connectWiFi();
    delay(2000);

    connectMQTTServer();
    Serial.println("Setup Finish");
}

void loop() {
    if (OTA) OTAon();
    // put your main code here, to run repeatedly:
    client.loop();
    if (stateLoading) {
        stateLoading = false;
        // Inicia uma String associando ao endereço
        String payload = "{\"d\":{\"adc\":";
        payload += 1;     // Atribui o valor de leitura de cont a String
        payload += "}}";  // Finaliza a String
        Serial.print("Enviando payload: ");
        Serial.println(payload);  // Escreve a String no monitor Serial
        client.publish(COMMAND_EVENT_LOADING, (char*)payload.c_str());
    }
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
        client.subscribe(COMMAND_TOPIC_LOADING);
        client.subscribe(COMMAND_TOPIC_OTA_ON);
        client.subscribe(COMMAND_TOPIC_OTA_OFF);
        digitalWrite(ledOnBoard, HIGH);
    } else {
        // Se ocorreu algum erro
        Serial.print("error = ");
        Serial.println(client.state());
        delay(2000);
        return connectMQTTServer();  // tenta conectar novamente
    }
}

void callback(char* topic, unsigned char* payload, unsigned int length) {
    Serial.print("topic: ");
    Serial.println(topic);
    String _buffer = "";

    StaticJsonDocument<200> jsonBuffer;

    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    String value = jsonBuffer["command"];
    jsonBuffer.remove("command");
    if (strcmp(topic, COMMAND_TOPIC_ROOMLIGHT) == 0) {
        stateLight = !stateLight;
        digitalWrite(relay1Pin, stateLight);
        digitalWrite(relay2Pin, stateLight);
        if (stateLight) {
            jsonBuffer["_id"] = "MainDatabase";
            jsonBuffer["status"]["roomLight"] = "on";
        } else {
            jsonBuffer["_id"] = "MainDatabase";
            jsonBuffer["status"]["roomLight"] = "off";
        }
        serializeJson(jsonBuffer, _buffer);
        Serial.println(_buffer);
        client.publish(COMMAND_EVENT_CONFIRM, (char*)_buffer.c_str());
    }else if (strcmp(topic, COMMAND_TOPIC_LOADING) == 0) {
        jsonBuffer["roomLight"] = stateLight;
        // jsonBuffer["loading"] = stateLoading;
        serializeJson(jsonBuffer, _buffer);
        Serial.println(_buffer);
        client.publish(COMMAND_EVENT_LOADING, (char*)_buffer.c_str());
    }else if(strcmp(topic, COMMAND_TOPIC_OTA_ON) == 0){
        OTA = true;
        jsonBuffer["OTA"] = OTA;
        serializeJson(jsonBuffer, _buffer);
        Serial.println(_buffer);
        client.publish(COMMAND_EVENT_CONFIRM, (char*)_buffer.c_str());
    }else if(strcmp(topic, COMMAND_TOPIC_OTA_OFF) == 0){
        finishOTA();
        jsonBuffer["OTA"] = OTA;
        serializeJson(jsonBuffer, _buffer);
        Serial.println(_buffer);
        client.publish(COMMAND_EVENT_CONFIRM, (char*)_buffer.c_str());
    }
}

void connectWiFi() {
    WiFi.mode(WIFI_STA);
    if (!WiFi.begin(ssid, pass)) {
        Serial.println("Wifi Not Cennected");
    }
    Serial.println("Wifi Connected");
}
