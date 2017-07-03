#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

#include "messenger.h"
#include "config.h"

unsigned int r = 0;
unsigned int g = 0;
unsigned int b = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println();

    Messenger::initialize();
    Messenger::mqttClient.onConnect(mqttOnConnect);
    Messenger::mqttClient.onMessage(mqttOnMessage);

    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    // mit WLAN verbinden
    Messenger::connectToWifi();    
}

// Eine Nachricht an $PREFIX/$TONE (wobei $TONE entweder r, g, oder b) setzt die Helligkeit
// der R, G, oder B-Komponenten. Alle Werte befinden sich zwischen 0 und 1023.
void mqttOnMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    Serial.println("Received message.");
    Serial.println("Topic: " + (String) topic);
    Serial.println("Payload: " + (String) payload);
    Serial.println("");

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
        Serial.println("r set to " + String(brightness));
    }
    else if((MQTT_PREFIX + "/g").equals(topic)) {
        g = brightness;
        Serial.println("g set to " + String(brightness));
    }
    else if((MQTT_PREFIX + "/b").equals(topic)) {
        b = brightness;
        Serial.println("b set to " + String(brightness));
    }
}

void mqttOnConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.println("Session present: " + (String) sessionPresent);
  Messenger::mqttClient.subscribe((MQTT_PREFIX + "/#").c_str(), 1);
}

void loop()
{
	analogWrite(R_PIN, r);
    analogWrite(G_PIN, g);
    analogWrite(B_PIN, b);
}
