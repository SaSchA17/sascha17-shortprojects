#pragma once

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "config.h"

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

bool connectedToWiFi = false;
bool connectedToMQTT = false;

const char* MqttPrefix = "trafficlight";
const char* MqttNotificationLight = /*MqttPrefix + */"/notification_light";
const char* MqttFootbridgeStatus = /*MqttPrefix + */"/footbridge_status";



// Prototypes
void connectToWifi();
void onWifiConnect(const WiFiEventStationModeGotIP& event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
void setupWiFi();
void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);
void setupMQTT();
