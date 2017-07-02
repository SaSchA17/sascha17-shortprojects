#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

#include "config.h"


namespace Messenger{

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
    void initialize();
    void debug_msg(String msg);
}
