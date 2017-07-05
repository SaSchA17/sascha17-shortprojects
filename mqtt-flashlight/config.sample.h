#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15
#define D9  3
#define D10 1


const String MQTT_PREFIX = "rgb-led";
const IPAddress MQTT_HOST = IPAddress(192, 168, 1, 999);
const unsigned int MQTT_PORT = 1883;

const int R_PIN = D3;
const int G_PIN = D5;
const int B_PIN = D6;

const String WIFI_SSID = "my-wifi-ssid";
const String WIFI_KEY = "my-wifi-key";
