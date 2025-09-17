#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

// ================= WiFi =================
const char* ssid     = "OSTI_WIFI";
const char* password = "P@$$w0rd2021";

const char* serverAddress = "192.168.1.7";
int serverPort = 12345;                   

WiFiClient wifi;

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= Button & LED Setup =================
const int buttons[] = {2, 3, 4, 5, 6, 7};
const int leds[]    = {A0, A1, A2, 10, 11, 12};
const char* labels[] = {"2", "3", "4", "5", "6", "7"};

const int NUM_BTNS = 6;

// ================= Timing =================
const unsigned long debounceDelay   = 50;   // debounce
const unsigned long nextPressDelay  = 500;  // cooldown

// Track button states
bool prevState[NUM_BTNS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastPress[NUM_BTNS] = {0, 0, 0, 0, 0, 0};

// ================= Ticket Display =================
unsigned long lastTicketTime = 0;
bool ticketDisplayed = false;

void setup() {
  Serial.begin(115200);

  // Setup inputs and outputs
  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Queue System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  delay(1000);

  // Connect to WiFi with status messages
  connectWiFi();
}

void loop() {
  // Check WiFi status, reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Check all buttons
  for (int i = 0; i < NUM_BTNS; i++) {
    checkButton(i);
  }

  // Reset LCD
  if (ticketDisplayed && (millis() - lastTicketTime >= 60000)) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("No Ticket");
    ticketDisplayed = false;
  }
}

// ================= Button Check =================
void checkButton(int i) {
  bool currentState = digitalRead(buttons[i]);

  // long press behavior
  digitalWrite(leds[i], currentState == LOW ? HIGH : LOW);

  // Detect falling edge (button press) with debounce + cooldown
  if (prevState[i] == HIGH && currentState == LOW &&
      (millis() - lastPress[i]) > (debounceDelay + nextPressDelay)) {

    lastPress[i] = millis();
    Serial.println(labels[i]);

    // Send to server over WiFi and display ticket from response
    String ticket = sendButtonPress(labels[i]);
    if (ticket.length() > 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ticket:");
      lcd.setCursor(8, 0);
      lcd.print(ticket);

      lastTicketTime = millis();
      ticketDisplayed = true;
    }
  }

  prevState[i] = currentState;
}

// ================= WiFi Functions =================
void connectWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  Serial.print("Connecting to WiFi ");
  Serial.println(ssid);

  unsigned long startAttempt = millis();
  int dots = 0;

  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    lcd.setCursor(strlen(ssid), 1);
    lcd.print('.');
    dots++;
    if (dots > 5) {
      lcd.setCursor(strlen(ssid), 1);
      lcd.print("     ");
      dots = 0;
    }

    Serial.print(".");
    delay(500);

    if (millis() - startAttempt > 10000) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Failed");
      Serial.println("\nWiFi Failed");
      delay(2000);
      return;
    }
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("No Ticket");
}

// ================= HTTP Send =================
String sendButtonPress(const char* label) {
  HttpClient client = HttpClient(wifi, serverAddress, serverPort);

  String endpoint = "/api/key/";
  endpoint += label;

  client.get(endpoint); // GET request

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("HTTP Status: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  if (statusCode == 200 && response.length() > 0) {
    // Extract only displayText from JSON
    int start = response.indexOf("\"displayText\":\"");
    if (start >= 0) {
      start += 15; // skip to value
      int end = response.indexOf("\"", start);
      if (end > start) {
        String ticket = response.substring(start, end);
        return ticket; 
      }
    }
  }
  return "";
}
