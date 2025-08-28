#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeSansBold24pt7b.h>

MCUFRIEND_kbv tft;

// Button input pins
const int BTN_REGULAR   = 30;
const int BTN_PRIORITY  = 31;
const int BTN_VIP       = 32;
const int BTN_EXCELLENT = 33;
const int BTN_GOOD      = 34;
const int BTN_POOR      = 35;

// LED output pins
const int LED_REGULAR   = 36;
const int LED_PRIORITY  = 37;
const int LED_VIP       = 38;
const int LED_EXCELLENT = 39;
const int LED_GOOD      = 40;
const int LED_POOR      = 41;

// Timing control
unsigned long ledOnTime = 0;
unsigned long ledDuration = 0;
int currentLED = -1;
bool isMessageDisplayed = false;

void setup() {
  Serial.begin(9600);

  pinMode(BTN_REGULAR, INPUT_PULLUP);
  pinMode(BTN_PRIORITY, INPUT_PULLUP);
  pinMode(BTN_VIP, INPUT_PULLUP);
  pinMode(BTN_EXCELLENT, INPUT_PULLUP);
  pinMode(BTN_GOOD, INPUT_PULLUP);
  pinMode(BTN_POOR, INPUT_PULLUP);

  pinMode(LED_REGULAR, OUTPUT);
  pinMode(LED_PRIORITY, OUTPUT);
  pinMode(LED_VIP, OUTPUT);
  pinMode(LED_EXCELLENT, OUTPUT);
  pinMode(LED_GOOD, OUTPUT);
  pinMode(LED_POOR, OUTPUT);

  digitalWrite(LED_REGULAR, LOW);
  digitalWrite(LED_PRIORITY, LOW);
  digitalWrite(LED_VIP, LOW);
  digitalWrite(LED_EXCELLENT, LOW);
  digitalWrite(LED_GOOD, LOW);
  digitalWrite(LED_POOR, LOW);

  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1); // Landscape
  tft.fillScreen(0X0000);
  tft.setFont(&FreeSansBold24pt7b);
 

  showWelcomeMessage();

  Serial.println("Queue & Feedback System Ready");
}

void loop() {
  if (currentLED == -1) {
    if (digitalRead(BTN_REGULAR) == LOW) {
      activateLED(LED_REGULAR, 2500, "Your ticket:\n \nRegular", true);
    } else if (digitalRead(BTN_PRIORITY) == LOW) {
      activateLED(LED_PRIORITY, 2500, "Your ticket:\n \nPriority", true);
    } else if (digitalRead(BTN_VIP) == LOW) {
      activateLED(LED_VIP, 2500, "Your ticket:\n \nVIP Card", true);
    } else if (digitalRead(BTN_EXCELLENT) == LOW) {
      activateLED(LED_EXCELLENT, 2500, "Feedback:\n\nExcellent.\n\nThank you!", false);
    } else if (digitalRead(BTN_GOOD) == LOW) {
      activateLED(LED_GOOD, 2500, "Feedback: Good\n\nThank you!", false);
    } else if (digitalRead(BTN_POOR) == LOW) {
      activateLED(LED_POOR, 2500, "Feedback: Poor\n\nWe're sorry to \n\nhear that!", false);
    }
  }

  if (currentLED != -1 && millis() - ledOnTime >= ledDuration) {
    digitalWrite(currentLED, LOW);
    currentLED = -1;

    if (isMessageDisplayed) {
      showWelcomeMessage();
      isMessageDisplayed = false;
    }
  }
}

void activateLED(int ledPin, int duration, const char* message, bool isTicket) {
  digitalWrite(ledPin, HIGH);
  ledOnTime = millis();
  ledDuration = duration;
  currentLED = ledPin;
  isMessageDisplayed = true;

  tft.fillScreen(0x0000);
  tft.setCursor(20, 100);
  tft.setTextColor(isTicket ? 0x07E0 : 0x07FF);
  printMultiline(message);

  Serial.println(message);
}

void showWelcomeMessage() {
  tft.fillScreen(0x07E0); // Green background

  const char* welcomeText = "Queueing System";

  // Get text width and height using getTextBounds
  int16_t x1, y1;
  uint16_t w, h;
  tft.setFont(&FreeSansBold24pt7b);
  tft.getTextBounds(welcomeText, 0, 0, &x1, &y1, &w, &h);

  int16_t xCentered = (480 - w) / 2;
  int16_t yCentered = (320 + h) / 2;

  tft.setTextColor(0xFFFF); // White
  tft.setCursor(xCentered, yCentered);
  tft.println(welcomeText);
}


// Print multiline messages
void printMultiline(const char* msg) {
  int x = 70;
  int y = 100;
  tft.setCursor(x, y);

  for (int i = 0; msg[i] != '\0'; i++) {
    if (msg[i] == '\n') {
      y += 30; // Move to next line
      tft.setCursor(x, y);
    } else {
      tft.print(msg[i]);
    }
  }
}
