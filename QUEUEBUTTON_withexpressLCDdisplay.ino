#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize LCD (address 0x27 or 0x3F depending on module)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Button input pins ===
const int buttonPins[] = {2, 3, 4, 5, 6, 7};

// === LED output pins ===
const int ledPins[] = {A0, A1, A2, 10, 11, 12};

// === Labels for Serial & LCD ===
const char* labels[] = {"Regular", "Priority", "VIP", "Excellent", "Good", "Poor"};

const int numButtons = 6;
const unsigned long debounceDelay = 50; // ms

bool prevStates[numButtons];
unsigned long lastPress[numButtons];

void setup() {
  Serial.begin(9600);

  // Init LCD
  lcd.init();
  lcd.backlight();
  showDefaultMessage();

  // Setup pins
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
    prevStates[i] = HIGH;
    lastPress[i] = 0;
  }

  Serial.println("Queue & Feedback System Ready");
}

void loop() {
  for (int i = 0; i < numButtons; i++) {
    checkButton(buttonPins[i], &prevStates[i], &lastPress[i], ledPins[i], labels[i]);
  }
}

bool checkButton(int pin, bool *prevState, unsigned long *lastPressTime, int ledPin, const char* label) {
  bool currentState = digitalRead(pin);
  bool wasPressed = false;

  // LED follows button
  digitalWrite(ledPin, currentState == LOW ? HIGH : LOW);

  // Debounced press detection
  if (*prevState == HIGH && currentState == LOW && (millis() - *lastPressTime) > debounceDelay) {
    *lastPressTime = millis();

    // Print to Serial
    Serial.println(label);

    // Show on LCD
    lcd.clear();
    //lcd.setCursor(0, 0);
    //lcd.print("You pressed:");
    lcd.setCursor(4, 0);
    lcd.print(label);
    

    wasPressed = true;
  }

  *prevState = currentState;
  return wasPressed;
}

void showDefaultMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lightfuels");
  lcd.setCursor(0, 1);
  lcd.print("Queueing System");
}
