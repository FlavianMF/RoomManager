bool connectWiFi() {
    WiFi.mode(WIFI_STA);
    if (!WiFi.begin(ssid, pass)) {
        Serial.println("Wifi Not Connected");
        return false;
    }
    Serial.println("Wifi Connected");
    return true;
}

