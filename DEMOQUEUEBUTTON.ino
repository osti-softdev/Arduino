// Button input pins
const int BTN_REGULAR   = 2;
const int BTN_PRIORITY  = 3;
const int BTN_VIP       = 4;
const int BTN_EXCELLENT = 5;
const int BTN_GOOD      = 6;
const int BTN_POOR      = 7;

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

void setup() {
  Serial.begin(9600);

  // Setup input pins with pull-up resistors
  pinMode(BTN_REGULAR, INPUT_PULLUP);
  pinMode(BTN_PRIORITY, INPUT_PULLUP);
  pinMode(BTN_VIP, INPUT_PULLUP);
  pinMode(BTN_EXCELLENT, INPUT_PULLUP);
  pinMode(BTN_GOOD, INPUT_PULLUP);
  pinMode(BTN_POOR, INPUT_PULLUP);

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

  Serial.println("Queue & Feedback System Ready");
}

void loop() {
  if (currentLED == -1) {
    if (digitalRead(BTN_REGULAR) == LOW) {
      activateLED(LED_REGULAR, 1000, "2");

    } else if (digitalRead(BTN_PRIORITY) == LOW) {
      activateLED(LED_PRIORITY, 1000, "3");

    } else if (digitalRead(BTN_VIP) == LOW) {
      activateLED(LED_VIP, 1000, "4");

    } else if (digitalRead(BTN_EXCELLENT) == LOW) {
      activateLED(LED_EXCELLENT, 1000, "5");

    } else if (digitalRead(BTN_GOOD) == LOW) {
      activateLED(LED_GOOD, 1000, "6");

    } else if (digitalRead(BTN_POOR) == LOW) {
      activateLED(LED_POOR, 1000, "7");
    }
  }

  // Turn off LED after duration
  if (currentLED != -1 && millis() - ledOnTime >= ledDuration) {
    digitalWrite(currentLED, LOW);
    currentLED = -1;
    // Serial.println("LED OFF — Ready for next input");
  }
}

// Helper function to activate LED
void activateLED(int ledPin, int duration, const char* label) {
  digitalWrite(ledPin, HIGH);
  ledOnTime = millis();
  ledDuration = duration;
  currentLED = ledPin;

  // Determine whether it's a Ticket or Feedback
  if (strcmp(label, "2") == 0 || strcmp(label, "3") == 0 || strcmp(label, "4") == 0) {
    // Serial.print("Ticket: ");
  } else {
   // Serial.print("Feedback: ");
  }
  Serial.println(label);
}

