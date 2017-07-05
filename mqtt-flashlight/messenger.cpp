#pragma once

// (c) 2015 Marvin Roger
// (c) 2017 Sonja Bleymehl, Simon Leiner

#include "messenger.h"

using namespace Messenger;

const bool Messenger::DEBUG = true;

struct configuration Messenger::config;

AsyncMqttClient Messenger::mqttClient;
Ticker Messenger::mqttReconnectTimer;

WiFiEventHandler Messenger::wifiConnectHandler;
WiFiEventHandler Messenger::wifiDisconnectHandler;
Ticker Messenger::wifiReconnectTimer;

void Messenger::connectToWifi() {
    debug_msg("Connecting to Wi-Fi...");
    WiFi.begin(config.WIFI_SSID.c_str(), config.WIFI_KEY.c_str());
}

void Messenger::onWifiConnect(const WiFiEventStationModeGotIP& event) {
    debug_msg("Connected to Wi-Fi network: " + config.WIFI_SSID);
    connectToMqtt();
}

void Messenger::onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
    debug_msg("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void Messenger::connectToMqtt() {
    debug_msg("Connecting to MQTT...");
    mqttClient.connect();
}

void Messenger::onMqttConnect(bool sessionPresent) {
    debug_msg("Connected to MQTT.");
    debug_msg("Session present: " + (String) sessionPresent);

    // Man könnte hier zu bestimmten Themen subscriben:
    //
    // int qos = 0; // im Zweifelsfall ist 0 okay ;-)
    // mqttClient.subscribe("mein/tolles/thema", qos);
}

void Messenger::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    debug_msg("Disconnected from MQTT.");

    if (WiFi.isConnected()) {
        debug_msg("Connecting to MQTT...");
        mqttReconnectTimer.once(2, connectToMqtt);
    }
}

void Messenger::onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    debug_msg("Subscribe acknowledged.");
    debug_msg("  packetId: " + (String) packetId);
    debug_msg("  qos: " + (String) qos);
}

void Messenger::onMqttUnsubscribe(uint16_t packetId) {
    debug_msg("Unsubscribe acknowledged.");
    Serial.print("  packetId: " + (String) packetId);
}

void Messenger::onMqttPublish(uint16_t packetId) {
    debug_msg("Publish acknowledged.");
    Serial.print("  packetId: " + (String) packetId);
}

void Messenger::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    debug_msg("Received message.");
    debug_msg("Topic: " + (String) topic);
    debug_msg("Payload: " + (String) payload);
    debug_msg("");
}

void Messenger::initialize(
    const String WIFI_SSID,
    const String WIFI_KEY,
    const IPAddress MQTT_HOST,
    const unsigned int MQTT_PORT) {

    config.WIFI_SSID = WIFI_SSID;
    config.WIFI_KEY = WIFI_KEY;
    config.MQTT_HOST = MQTT_HOST;
    config.MQTT_PORT = MQTT_PORT;

    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(config.MQTT_HOST, config.MQTT_PORT);
}

void Messenger::debug_msg(String msg) {
    if(Messenger::DEBUG) {
        Serial.println(msg);
    }
}