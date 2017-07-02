#pragma once

#include "mqtt.h"

const bool DEBUG = true;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
    debug_msg("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID.c_str(), WIFI_KEY.c_str());
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
    debug_msg("Connected to Wi-Fi network: " + WIFI_SSID);
    connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
    debug_msg("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  debug_msg("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  debug_msg("Connected to MQTT.");
  debug_msg("Session present: " + (String) sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  debug_msg("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    debug_msg("Connecting to MQTT...");
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  debug_msg("Subscribe acknowledged.");
  debug_msg("  packetId: " + (String) packetId);
  debug_msg("  qos: " + (String) qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  debug_msg("Unsubscribe acknowledged.");
  Serial.print("  packetId: " + (String) packetId);
}

void onMqttPublish(uint16_t packetId) {
  debug_msg("Publish acknowledged.");
  Serial.print("  packetId: " + (String) packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    debug_msg("Received message.");
    debug_msg("Topic: " + (String) topic);
    debug_msg("Payload: " + (String) payload);
    debug_msg("");
}

void initWiFi() {
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void debug_msg(String msg) {
    if(DEBUG) {
        Serial.println(msg);
    }
}