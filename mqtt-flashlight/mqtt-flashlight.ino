#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

#include "mqtt.h"
#include "config.h"

unsigned int r = 0;
unsigned int g = 0;
unsigned int b = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println();

    initWiFi();
    mqttClient.onConnect(mqttOnConnect);
    mqttClient.onMessage(mqttOnMessage);

    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    // mit WLAN verbinden
    connectToWifi();    
}

// Eine Nachricht an $PREFIX/$TONE (wobei $TONE entweder r, g, oder b) setzt die Helligkeit
// der R, G, oder B-Komponenten. Alle Werte befinden sich zwischen 0 und 1023.
void mqttOnMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    debug_msg("Received message.");
    debug_msg("Topic: " + (String) topic);
    debug_msg("Payload: " + (String) payload);
    debug_msg("");

    // payload sollte eine Zahl zwischen 0 und 1023 sein
    int brightness = atoi(payload);
    if(brightness < 0) {
        brightness = 0;
    }
    if(brightness > PWMRANGE) {
        brightness = PWMRANGE;
    }

    if((MQTT_PREFIX + "/r").equals(topic)) {
        r = brightness;
        debug_msg("r set to " + String(brightness));
    }
    else if((MQTT_PREFIX + "/g").equals(topic)) {
        g = brightness;
        debug_msg("g set to " + String(brightness));
    }
    else if((MQTT_PREFIX + "/b").equals(topic)) {
        b = brightness;
        debug_msg("b set to " + String(brightness));
    }
}

void mqttOnConnect(bool sessionPresent) {
  debug_msg("Connected to MQTT.");
  debug_msg("Session present: " + (String) sessionPresent);
  mqttClient.subscribe((MQTT_PREFIX + "/#").c_str(), 1);
}

void loop()
{
	analogWrite(R_PIN, r);
    analogWrite(G_PIN, g);
    analogWrite(B_PIN, b);
}
