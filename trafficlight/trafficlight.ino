/*
INTERACTIVE TRAFFIC LIGHT WITH footbridge
(c) 2014-2017, Simon Leiner
*/

#include "config.h"
#include "mqtt.h"

// Constants
const int ledCar_g = D8;     // led pin for green
const int ledCar_y = D7;     // led pin for yellow
const int ledCar_r = D6;     // led pin for red

const int ledPed_g = D2;     // led pin for green (footbridge)
const int ledPed_y = D1;     // led pin for yellow (footbridge)
const int ledPed_r = D0;     // led pin for red (footbridge)

const int button = D3;       // button pin
const int led_notify = D4;   // notifying led pin
const int flash_ms = 125;    // duration of a blink (ms)

// duration of phases in ms
const int duration_ms[8] = {
  1000,         // 0 = traffic light red, footbridge red
  3000,         // 1 = traffic light red-yellow, footbridge red
  10000,        // 2 = traffic light green, footbridge red (minimum duration!)
  3000,         // 3 = traffic light yellow, footbridge red
  1000,         // 4 = traffic light red, footbridge red
  0,            // 5 = traffic light red, footbridge red-yellow
  9000,         // 6 = traffic light red, footbridge green
  0};           // 7 = traffic light red, footbridge yellow

// running variables
int state = -1;                   // stores numeric coded state
int lastState = -1;               // stores last state (for act() function)
long long lastSwitch_ms = -10000; // time at last switch (starts with -10000 so lights switch immediatly if button is pressed short after starting the program
                                  // using long long so variable can store all unsigned long values, as well as negative numbers
unsigned long lastFlash_ms = -1;  // time at last notifying led blink
bool goRequest = false;           // footwalk requested
bool pushed = false;              // button pushed at last read-in
bool notifyOn = false;            // notifying blink


int nextState(int s) {
  // returns state after int s
  if (s >= 0 && s < 7) {
    s++;
  }
  else if (s == 7) {
    s = 0;
  }
  else {
    Serial.print("nextState(");
    Serial.print(s);
    Serial.println("): state not recognized! Switchung to state 0");
    s = 0;
  }

  return s;
}

void act(int s) {
  // depending on state switches leds 
  if (s != lastState) {   // check if state was updated

    // log to serial output
    Serial.print("Switching to state ");
    Serial.print(state);
    Serial.print(" : ");

    if (s == 0) {
      Serial.println("traffic light red, footbridge red");
      digitalWrite(ledCar_g, LOW);       // Update traffic light status
      digitalWrite(ledCar_y, LOW);
      digitalWrite(ledCar_r, HIGH);
      digitalWrite(ledPed_g, LOW);        // Update footbridge status
      digitalWrite(ledPed_y, LOW);
      digitalWrite(ledPed_r, HIGH);
    }
    else if (s == 1) {
      Serial.println("traffic light red-yellow, footbridge red");
      digitalWrite(ledCar_g, LOW);       // Update traffic light status
      digitalWrite(ledCar_y, HIGH);
      digitalWrite(ledCar_r, HIGH);
      digitalWrite(ledPed_g, LOW);        // Update footbridge status
      digitalWrite(ledPed_y, LOW);
      digitalWrite(ledPed_r, HIGH);
    }
    else if (s == 2) {
      Serial.println("traffic light green, footbridge red");
      digitalWrite(ledCar_g, HIGH);        // Update traffic light status
      digitalWrite(ledCar_y, LOW);
      digitalWrite(ledCar_r, LOW);
      digitalWrite(ledPed_g, LOW);        // Update footbridge status
      digitalWrite(ledPed_y, LOW);
      digitalWrite(ledPed_r, HIGH);
    }
    else if (s == 3) {
      Serial.println("traffic light yellow, footbridge red");
      digitalWrite(ledCar_g, LOW);       // Update traffic light status
      digitalWrite(ledCar_y, HIGH);
      digitalWrite(ledCar_r, LOW);
      digitalWrite(ledPed_g, LOW);        // Update footbridge status
      digitalWrite(ledPed_y, LOW);
      digitalWrite(ledPed_r, HIGH);
    }
    else if (s == 4) {
      Serial.println("traffic light red, footbridge red");
      digitalWrite(ledCar_g, LOW);       // Update traffic light status
      digitalWrite(ledCar_y, LOW);
      digitalWrite(ledCar_r, HIGH);
      digitalWrite(ledPed_g, LOW);        // Update footbridge status
      digitalWrite(ledPed_y, LOW);
      digitalWrite(ledPed_r, HIGH);
    }
    else if (s == 5) { 
      Serial.println("traffic light red, footbridge red-yellow");
      digitalWrite(ledCar_g, LOW);       // Update traffic light status
      digitalWrite(ledCar_y, LOW);
      digitalWrite(ledCar_r, HIGH);
      digitalWrite(ledPed_g, LOW);        // Update footbridge status
      digitalWrite(ledPed_y, HIGH);
      digitalWrite(ledPed_r, HIGH);
    }
    else if (s == 6) {
      Serial.println("traffic light red, footbridge green");
      digitalWrite(ledCar_g, LOW);       // Update traffic light status
      digitalWrite(ledCar_y, LOW);
      digitalWrite(ledCar_r, HIGH);
      digitalWrite(ledPed_g, HIGH);       // Update footbridge status
      digitalWrite(ledPed_y, LOW);
      digitalWrite(ledPed_r, LOW);
      goRequest = false;              // delete Request
    }
    else if (s == 7) {
      Serial.println("traffic light red, footbridge yellow");
      digitalWrite(ledCar_g, LOW);       // Update traffic light status
      digitalWrite(ledCar_y, LOW);
      digitalWrite(ledCar_r, HIGH);
      digitalWrite(ledPed_g, LOW);        // Update footbridge status
      digitalWrite(ledPed_y, HIGH);
      digitalWrite(ledPed_r, LOW);
    }
    // Update variable
    lastState = s;
  }
}

bool updateState() {
  // if needed, updates status variable
  if (state == 2) { // traffic light green phase, the only phase that does not automatically time out
    if ((goRequest == true) && (millis() >= (lastSwitch_ms + duration_ms[state]))) {  // if footbridge was requested and green was on long enough
      state = nextState(state);     // update state
      lastSwitch_ms = millis();     // update timer
      return true;                  // true => status switched
    }

  }

  else if (millis() >= (lastSwitch_ms + duration_ms[state])) {  // other phases: if enough time has passed
    state = nextState(state);       // update state
    lastSwitch_ms = millis();       // update timer
    return true;                    // true => status switched
    }
    
   else {
    return false;                   // false => status not switched
    }
}

bool getButton() {
  // return true if button was pressed
  if (digitalRead(button) == HIGH) {
    if (pushed) {                                                   // filtering continous button pressing                
      return false;
    }
    else {
      pushed = true;
      delay(20);                                                                      // debouncing
      return true;
    }
  }
  else {
    pushed = false;
    return false;
  }
}

void setup() {
  // initialise serial output
  Serial.begin(115200);

  // setup WiFi and MQTT connection
  setupMQTT();
  setupWiFi();
  connectToWifi();  

  delay(3000);
  
  // initialise led pins as output
  Serial.print("Initialising pins... ");
  pinMode(ledCar_g, OUTPUT);
  pinMode(ledCar_y, OUTPUT);
  pinMode(ledCar_r, OUTPUT);
  
  pinMode(ledPed_g, OUTPUT);
  pinMode(ledPed_y, OUTPUT);
  pinMode(ledPed_r, OUTPUT);
  
  pinMode(led_notify, OUTPUT);
  pinMode(button, INPUT);
  Serial.println("Done!");

  // start with green
  state = 2;
}

void loop() {
  // act depending on state
  act(state);

  // trigger goRequest if button was pressed
  if (state != 6 && getButton() == true && goRequest == false) {    // ignore if footbridge light is already green or request already taken
    goRequest = true;
    Serial.println("Button pushed! Requesting green light for footbridge!");
    mqttClient.publish(MqttNotificationLight, 1, true, "true");
  }

  // blink if signal is coming
  if (goRequest == true) {
    if (millis() >= (lastFlash_ms + flash_ms)) {
      notifyOn = !notifyOn;
      digitalWrite(led_notify, notifyOn);
      lastFlash_ms = millis();
    }
  }
  // turn notify led off else
  else {
    digitalWrite(led_notify, LOW);
    notifyOn = false;
    mqttClient.publish(MqttNotificationLight, 1, true, "false");
  }

  // check if new state is required
  updateState();
}
