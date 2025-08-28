// Button input pins 
const int buttons[] = {2, 3, 4, 5, 6, 7};

// LED output pins
const int leds[] = {A0, A1, A2, 10, 11, 12};

// Labels for Serial output 
const char* labels[] = {"2", "3", "4", "5", "6", "7"};

// Debounce / delay settings 
const unsigned long debounceDelay   = 50;   // ms (basic debounce)
const unsigned long nextPressDelay  = 500;  // ms (cooldown before next press)

//  Track button states 
bool prevState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastPress[6] = {0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(9600);

  // Setup input pins with pull-up resistors
  for (int i = 0; i < 6; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW); // all LEDs off
  }

  Serial.println("Queue & Feedback System Ready");
}

void loop() {
  for (int i = 0; i < 6; i++) {
    checkButton(i);
  }
}

void checkButton(int i) {
  bool currentState = digitalRead(buttons[i]);

  // LED follows button state directly
  digitalWrite(leds[i], currentState == LOW ? HIGH : LOW);

  // Detect falling edge (button press) with cooldown
  if (prevState[i] == HIGH && currentState == LOW &&
      (millis() - lastPress[i]) > (debounceDelay + nextPressDelay)) {
    
    lastPress[i] = millis();
    Serial.println(labels[i]);
  }

  prevState[i] = currentState;
}
