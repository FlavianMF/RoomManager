// const char* ssid = "Edson NMRK";
// const char* pass = "35lotus54Be";

// const char* ssid = "FlvShRy";
// const char* pass = "Flv0m3g4";

// const char* ssid = "CHUPA TROIA";
// const char* pass = "omega2019";


void OTAon() {
    if (!stateOTA) {
        stateOTA = true;
        enableOTA();
        OTA = true;
    } else {
        ArduinoOTA.handle();
    }
}

void enableOTA() {
    Serial.println("starting OTA");
    blinkLedStatus(2, false);
    setupWifi();
    Serial.print("IP adress: ");
    Serial.println(WiFi.localIP());
    ArduinoOTA.setHostname("stRoomManager");
    ArduinoOTA.onStart(startOTA);
        ArduinoOTA.onEnd(endOTA);
    ArduinoOTA.onProgress(progressOTA);
    ArduinoOTA.onError(errorOTA);
    ArduinoOTA.begin();
    OTA = true;
    Serial.println("OTA started");
    blinkLedStatus(2, true);
}

void finishOTA() {
    Serial.println("finishing OTA");
    blinkLedStatus(2, false);
    OTA = false;
    // ArduinoOTA.end();
    // finishWifi();
    Serial.println("OTA finished");
    blinkLedStatus(2, true);
}

void setupWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! \nRebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.println("Wifi Connected");
}

void finishWifi() { WiFi.mode(WIFI_OFF); }

void startOTA() {
    String type;

    if (ArduinoOTA.getCommand() == U_FLASH)
        type = "flash";
    else
        type = "filesystem";

    Serial.println("Start updating " + type);

    // digitalWrite(ledOnBoard, HIGH);
}

void progressOTA(unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void endOTA() {
    Serial.println("\nUpdate completed");
    // digitalWrite(ledOnBoard, LOW);
}

void errorOTA(ota_error_t error) {
    Serial.printf("Error[%u]: ", error);

    if (error == OTA_AUTH_ERROR)
        Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
}