#pragma once

// Messenger for NodeMCU boards
// ============================
// aufgebaut auf die Bibliothek AsyncMqttClient von Marvin Roger
// 
// (c) 2015-2017 Marvin Roger
// (c) 2017 Sonja Bleymehl, Simon Leiner
//
// herausgegeben unter der MIT-Lizenz
//
// -------------------------------------------------------------
//
// Diese Bibliothek ermöglicht das einfache Aufbauen einer 
// MQTT-Verbindung sowie das asynchrone Veröffentlichen und
// Reagieren auf Nachrichten. Dazu stellt die Bibliothek 
// zusätzlich eine (ggf. verschlüsselte) Verbindung zu einem
// WLAN-AP her.
//
//
// BENUTZUNG DER Bibliothek
// ========================
//
// Zunächst muss die Funktion Messenger::initialize aufgerufen werden
// Diese benötigt folgende Argumente (in dieser Reihenfolge):
//  - const String WIFI_SSID
//  - const String WIFI_KEY
//  - const IPAddress MQTT_HOST
//  - const unsigned int MQTT_PORT
//
//
//
// Eine Nachricht kann man mit der Funktion
// Messenger::mqttClient.publish veröffentlichen.
// 



#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>


namespace Messenger{

    struct configuration {
        String WIFI_SSID;
        String WIFI_KEY;

        IPAddress MQTT_HOST;
        unsigned int MQTT_PORT;
    };

    extern struct configuration config;

    extern AsyncMqttClient mqttClient;
    extern Ticker mqttReconnectTimer;

    extern WiFiEventHandler wifiConnectHandler;
    extern WiFiEventHandler wifiDisconnectHandler;
    extern Ticker wifiReconnectTimer;

    extern const bool DEBUG;

    void connectToWifi();
    void onWifiConnect(const WiFiEventStationModeGotIP& event);
    void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
    void connectToMqtt();
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttSubscribe(uint16_t packetId, uint8_t qos);
    void onMqttUnsubscribe(uint16_t packetId);
    void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    void onMqttPublish(uint16_t packetId);  
    void initialize(const String WIFI_SSID, const String WIFI_KEY, const IPAddress MQTT_HOST, const unsigned int MQTT_PORT);
    void debug_msg(String msg);
}
