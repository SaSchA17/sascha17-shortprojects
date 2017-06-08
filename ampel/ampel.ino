/*
INTERAKTIVE FUSSGÄNGERAMPEL
(c) 2014, Simon Leiner
*/

// Constants
const int led_green = 11;    // led pin for green
const int led_yellow = 12;  // led pin for yellow
const int led_red = 13;     // led pin for red

const int led2_green = 5;   // led pin for green (footbridge)
const int led2_yellow = 6;  // led pin for yellow (footbridge)
const int led2_red = 7;     // led pin for red (footbridge)

const int button = 2;       // button pin
const int led_notify = 8;   // notifying led pin
const int flash_ms = 125;   // duration of a blink (ms)

// duration of phases in ms
const int duration_ms[8] = {
  1000,         // 0 = traffic light red, footbridge red
  3000,         // 1 = traffic light red-yellow, footbridge red
  10000,        // 2 = traffic light green, footbridge red (minimum duration!)
  3000,         // 3 = traffic light yellow, footbridge red
  500,          // 4 = traffic light red, footbridge red
  500,          // 5 = traffic light red, footbridge red-yellow
  8000,         // 6 = traffic light red, footbridge green
  1000};        // 7 = traffic light red, footbridge yellow

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
      digitalWrite(led_green, LOW);       // Update traffic light status
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
      digitalWrite(led2_green, LOW);        // Update footbridge status
      digitalWrite(led2_yellow, LOW);
      digitalWrite(led2_red, HIGH);
    }
    else if (s == 1) {
      Serial.println("traffic light red-yellow, footbridge red");
      digitalWrite(led_green, LOW);       // Update traffic light status
      digitalWrite(led_yellow, HIGH);
      digitalWrite(led_red, HIGH);
      digitalWrite(led2_green, LOW);        // Update footbridge status
      digitalWrite(led2_yellow, LOW);
      digitalWrite(led2_red, HIGH);
    }
    else if (s == 2) {
      Serial.println("traffic light green, footbridge red");
      digitalWrite(led_green, HIGH);        // Update traffic light status
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, LOW);
      digitalWrite(led2_green, LOW);        // Update footbridge status
      digitalWrite(led2_yellow, LOW);
      digitalWrite(led2_red, HIGH);
    }
    else if (s == 3) {
      Serial.println("traffic light yellow, footbridge red");
      digitalWrite(led_green, LOW);       // Update traffic light status
      digitalWrite(led_yellow, HIGH);
      digitalWrite(led_red, LOW);
      digitalWrite(led2_green, LOW);        // Update footbridge status
      digitalWrite(led2_yellow, LOW);
      digitalWrite(led2_red, HIGH);
    }
    else if (s == 4) {
      Serial.println("traffic light red, footbridge red");
      digitalWrite(led_green, LOW);       // Update traffic light status
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
      digitalWrite(led2_green, LOW);        // Update footbridge status
      digitalWrite(led2_yellow, LOW);
      digitalWrite(led2_red, HIGH);
    }
    else if (s == 5) {
      Serial.println("traffic light red, footbridge red-yellow");
      digitalWrite(led_green, LOW);       // Update traffic light status
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
      digitalWrite(led2_green, LOW);        // Update footbridge status
      digitalWrite(led2_yellow, HIGH);
      digitalWrite(led2_red, HIGH);
    }
    else if (s == 6) {
      Serial.println("traffic light red, footbridge green");
      digitalWrite(led_green, LOW);       // Update traffic light status
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
      digitalWrite(led2_green, HIGH);       // Update footbridge status
      digitalWrite(led2_yellow, LOW);
      digitalWrite(led2_red, LOW);
      goRequest = false;              // delete Request
    }
    else if (s == 7) {
      Serial.println("traffic light red, footbridge yellow");
      digitalWrite(led_green, LOW);       // Update traffic light status
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
      digitalWrite(led2_green, LOW);        // Update footbridge status
      digitalWrite(led2_yellow, HIGH);
      digitalWrite(led2_red, LOW);
    }
    // Update variable
    lastState = s;
  }
}

bool updateState() {
  // if needed, updates status variable
  if (state == 2) { // green phase
    if ((goRequest == true) && (millis() >= (lastSwitch_ms + duration_ms[state]))) {  // if footbridge was requested and green was on long enough
      state = nextState(state);               // update state
      lastSwitch_ms = millis();                                                         // update timer
      return true;                  // return true: status switched
    }

  }

  else if (millis() >= (lastSwitch_ms + duration_ms[state])) {                         // other phases: if enough time has passed
    state = nextState(state);                                                          // update state
    lastSwitch_ms = millis();                                                          // update timer
    return true;                       // return true: status switched
    }
    
   else {
    return false;                                                                    // return false: status not switched
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
  Serial.begin(9600);
  
  // initialise led pins as output
  Serial.print("Initialising pins... ");
  pinMode(led_green, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(led_red, OUTPUT);
  
  pinMode(led2_green, OUTPUT);
  pinMode(led2_yellow, OUTPUT);
  pinMode(led2_red, OUTPUT);
  
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
  }

  // check if new state is required
  updateState();
}

