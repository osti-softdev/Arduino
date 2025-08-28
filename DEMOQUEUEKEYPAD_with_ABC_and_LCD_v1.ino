#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD I2C address (commonly 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// LED output pins
const int LED_REGULAR   = A0;
const int LED_PRIORITY  = A1;
const int LED_VIP       = A2;
const int LED_EXCELLENT = 10;
const int LED_GOOD      = 11;
const int LED_POOR      = 12;

// Timing control
unsigned long ledOnTime = 0;
unsigned long ledDuration = 0;
int currentLED = -1;

// Ticket display timer
unsigned long lastTicketTime = 0;
bool ticketDisplayed = false;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = {9, 8, 7, 6};    
byte colPins[COLS] = {5, 4, 3, 2};    

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);

  // LED pins setup
  pinMode(LED_REGULAR, OUTPUT);
  pinMode(LED_PRIORITY, OUTPUT);
  pinMode(LED_VIP, OUTPUT);
  pinMode(LED_EXCELLENT, OUTPUT);
  pinMode(LED_GOOD, OUTPUT);
  pinMode(LED_POOR, OUTPUT);

  // Turn LEDs off
  digitalWrite(LED_REGULAR, LOW);
  digitalWrite(LED_PRIORITY, LOW);
  digitalWrite(LED_VIP, LOW);
  digitalWrite(LED_EXCELLENT, LOW);
  digitalWrite(LED_GOOD, LOW);
  digitalWrite(LED_POOR, LOW);

  // LCD init
  lcd.init();      
  lcd.backlight(); 
  lcd.setCursor(2, 0);
  lcd.print("No Ticket");

  Serial.println("Queue & Feedback System Ready (Keypad + LCD I2C)");
}

void loop() {
  char key = keypad.getKey();

  if (key && currentLED == -1) {
    switch (key) {
      case '0': activateLED(LED_REGULAR, 10, key); break;
      case '1': activateLED(LED_REGULAR, 10, key); break;
      case '2': activateLED(LED_REGULAR, 10, key); break;
      case '3': activateLED(LED_PRIORITY, 10, key); break;
      case '4': activateLED(LED_VIP, 10, key); break;
      case '5': activateLED(LED_EXCELLENT, 10, key); break;
      case '6': activateLED(LED_GOOD, 10, key); break;
      case '7': activateLED(LED_POOR, 10, key); break;
      case '8': activateLED(LED_REGULAR, 100, key); break;
      case '9': activateLED(LED_REGULAR, 10, key); break;
      case 'A': activateLED(LED_POOR, 10, key); break;
      case 'B': activateLED(LED_POOR, 10, key); break;
      case 'C': activateLED(LED_POOR, 10, key); break;
      case 'D': activateLED(LED_POOR, 10, key); break;
      case '#': activateLED(LED_POOR, 10, key); break;
      case '*': activateLED(LED_REGULAR, 10, key); break;
    }
  }

  // Always check for Serial messages
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ticket:");
    lcd.setCursor(8, 0);
    lcd.print(message);

    // Start ticket display timer
    lastTicketTime = millis();
    ticketDisplayed = true;
  }

  // Check if 30 seconds passed since ticket was shown
  if (ticketDisplayed && (millis() - lastTicketTime >= 30000)) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("No Ticket");
    ticketDisplayed = false; // Reset
  }

  // Turn off LED after duration
  if (currentLED != -1 && millis() - ledOnTime >= ledDuration) {
    digitalWrite(currentLED, LOW);
    currentLED = -1;
  }
}

// Helper function
void activateLED(int ledPin, int duration, char label) {
  digitalWrite(ledPin, HIGH);
  ledOnTime = millis();
  ledDuration = duration;
  currentLED = ledPin;
  Serial.println(label);
}
