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
        // digitalWrite(relay2Pin, stateLight);
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
    } else if (strcmp(topic, COMMAND_TOPIC_LOADING) == 0) {
        jsonBuffer["roomLight"] = stateLight;
        // jsonBuffer["loading"] = stateLoading;
        serializeJson(jsonBuffer, _buffer);
        Serial.println(_buffer);
        client.publish(COMMAND_EVENT_LOADING, (char*)_buffer.c_str());
    } else if (strcmp(topic, COMMAND_TOPIC_OTA_ON) == 0) {
        enableOTA();
        jsonBuffer["OTA"] = OTA;
        serializeJson(jsonBuffer, _buffer);
        Serial.println(_buffer);
        client.publish(COMMAND_EVENT_CONFIRM, (char*)_buffer.c_str());
    } else if (strcmp(topic, COMMAND_TOPIC_OTA_OFF) == 0) {
        finishOTA();
        jsonBuffer["OTA"] = OTA;
        serializeJson(jsonBuffer, _buffer);
        Serial.println(_buffer);
        client.publish(COMMAND_EVENT_CONFIRM, (char*)_buffer.c_str());
    }
}

bool connectMQTTServer() {
    Serial.println("Connecting to MQTT Server...");
    blinkLedStatus(2, false);
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
        blinkLedStatus(2, true);
        return true;
    } else {
        // Se ocorreu algum erro
        Serial.print("error = ");
        Serial.println(client.state());
        delay(2000);
        blinkLedStatus(2, false);
        return false;
    }
}