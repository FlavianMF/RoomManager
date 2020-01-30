#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

// #define ssid "FABLAB"
// #define pass "F@bl@b2017"

// #define ssid "COWORKING"
// #define pass "coworking2018"

#define ssid "NEW HOUSE"
#define pass "E22C4156"

const String ORG = "20qyjr";
const String DEVICE_TYPE = "Esp";
const String DEVICE_ID = "0001";
#define DEVICE_TOKEN "sp24-iV(jAgHekMO-Y"

#define ledOnBoard 2
#define relay1Pin 0
// #define relay2Pin 3

#define COMMAND_TOPIC_ROOMLIGHT "iot-2/cmd/roomLight/fmt/json"
#define COMMAND_TOPIC_LOADING "iot-2/cmd/loading/fmt/json"
#define COMMAND_TOPIC_OTA_ON "iot-2/cmd/OTA_ON/fmt/json"
#define COMMAND_TOPIC_OTA_OFF "iot-2/cmd/OTA_OFF/fmt/json"

#define COMMAND_EVENT_LOADING "iot-2/evt/loading/fmt/json"
#define COMMAND_EVENT_CONFIRM "iot-2/evt/confirm/fmt/json"

const String CLIENT_ID = "d:" + ORG + ":" + DEVICE_TYPE + ":" + DEVICE_ID;
const String MQTT_SERVER = ORG + ".messaging.internetofthings.ibmcloud.com";

bool shouldSvaeConfig = false;
// const int PIN_AP = 2;

bool stateLight = false;
bool stateLoading = false;

bool stateOTA = false;
bool OTA = false;

WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER.c_str(), 1883, wifiClient);

void blinkLedStatus(uint8_t times, bool finishStatus, uint8_t width = 50);

void setup() {
    int i = 0;

    Serial.begin(115200);
    Serial.println("Setup Init");

    EEPROM.begin(32);

    pinMode(ledOnBoard, OUTPUT);
    pinMode(relay1Pin, OUTPUT);
    // pinMode(relay2Pin, OUTPUT);

    digitalWrite(ledOnBoard, LOW);
    digitalWrite(relay1Pin, LOW);
    // digitalWrite(relay2Pin, LOW);

    WiFiManager wifiManager;

    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.autoConnect("ESP_AP", "12345678");
    wifiManager.setDebugOutput(false);

    i = 0;
    while (!connectWiFi()) {
        delay(1000);
        i++;
        if (i >= 5) ESP.restart();
    }
    delay(2000);

    i = 0;
    while (!connectMQTTServer()) {
        delay(1000);
        i++;
        if (i >= 6) ESP.restart();
    }
    Serial.println("Setup Finish");
    blinkLedStatus(5, true, 200);
}

void loop() {
    // WiFiManager wifiManager;
    if (OTA) OTAon();

    if (!client.loop()) ESP.restart();

    if (stateLoading) {
        stateLoading = false;
        // Inicia uma String associando ao endereço
        String payload = "{\"d\":{\"adc\":";
        payload += 1;     // Atribui o valor de leitura de cont a String
        payload += "}}";  // Finaliza a String
        Serial.print("Enviando payload: ");
        Serial.println(payload);  // Escreve a String no monitor Serial
        client.publish(COMMAND_EVENT_LOADING, (char *)payload.c_str());
    }
}
//-------------------------------------------------------------

// callback que indica que o ESP entrou no modo AP
void configModeCallback(WiFiManager *myWiFiManager) {
    Serial.println("Entrou no modo de configuração");
    Serial.println(WiFi.softAPIP());  // imprime o IP do AP
    Serial.println(
        myWiFiManager->getConfigPortalSSID());  // imprime o SSID criado da rede
}

// callback que indica que salvamos uma nova rede para se conectar (modo
// estação)
void saveConfigCallback() {
    Serial.println("Configuração salva");
    Serial.println(WiFi.softAPIP());  // imprime o IP do AP
}

void blinkLedStatus(uint8_t times, bool finishStatus, uint8_t width) {
    while (times-- > 0) {
        digitalWrite(ledOnBoard, LOW);
        delay(width);
        digitalWrite(ledOnBoard, HIGH);
        delay(width);
    }
    if (!finishStatus) digitalWrite(ledOnBoard, LOW);
}
