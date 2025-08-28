#include <Keypad.h>

// with A B C D input 

// LED output pins
const int LED_REGULAR   = A0;
const int LED_PRIORITY  = A1;
const int LED_VIP       = A2;
const int LED_EXCELLENT = 10;
const int LED_GOOD      = 11;
const int LED_POOR      = 12;
const int LED_A      = 13;
const int LED_B      = 14;
const int LED_C      = 15;
const int LED_D      = 16;



// Timing control
unsigned long ledOnTime = 0;
unsigned long ledDuration = 0;
int currentLED = -1;

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

  // Setup LED pins as outputs
  pinMode(LED_REGULAR, OUTPUT);
  pinMode(LED_PRIORITY, OUTPUT);
  pinMode(LED_VIP, OUTPUT);
  pinMode(LED_EXCELLENT, OUTPUT);
  pinMode(LED_GOOD, OUTPUT);
  pinMode(LED_POOR, OUTPUT);

  // Ensure all LEDs are off
  digitalWrite(LED_REGULAR, LOW);
  digitalWrite(LED_PRIORITY, LOW);
  digitalWrite(LED_VIP, LOW);
  digitalWrite(LED_EXCELLENT, LOW);
  digitalWrite(LED_GOOD, LOW);
  digitalWrite(LED_POOR, LOW);

  Serial.println("Queue & Feedback System Ready (Keypad Version)");
}

void loop() {
  char key = keypad.getKey();

  if (key && currentLED == -1) {
    switch (key) {
      case '2': activateLED(LED_REGULAR, 10, "2"); break;
      case '3': activateLED(LED_PRIORITY, 10, "3"); break;
      case '4': activateLED(LED_VIP, 10, "4"); break;
      case '5': activateLED(LED_EXCELLENT, 10, "5"); break;
      case '6': activateLED(LED_GOOD, 10, "6"); break;
      case '7': activateLED(LED_POOR, 10, "7"); break;
      case 'A': activateLED(LED_A, 10, "A"); break;
      case 'B': activateLED(LED_B, 10, "B"); break;
      case 'C': activateLED(LED_C, 10, "C"); break;
      case 'D': activateLED(LED_D, 10, "D"); break;
    }
  }

  // Turn off LED after duration
  if (currentLED != -1 && millis() - ledOnTime >= ledDuration) {
    digitalWrite(currentLED, LOW);
    currentLED = -1;
  }
}

// Helper function to activate LED
void activateLED(int ledPin, int duration, const char* label) {
  digitalWrite(ledPin, HIGH);
  ledOnTime = millis();
  ledDuration = duration;
  currentLED = ledPin;

  // Optional: print to serial
  Serial.println(label);
}
