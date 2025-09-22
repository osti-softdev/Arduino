// FINAL.
// TOPLINE - FINAL INTEGRATED
// Arduino UNO WiFi S3 – Queueing system with LCD, Server Integration, JSON-based ticket display

#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// ================= WiFi =================
// const char* ssid = "Lawrenzo XR"; 
// const char* password = "123321Cade";

const char* ssid     = "OSTI_WIFI";
const char* password = "P@$$w0rd2021";

const char* serverAddress = "192.168.1.8";
int serverPort = 12345;

WiFiClient wifi;

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= Button & LED Setup =================
// (Match UNO pins)
const int buttons[] = {2, 3, 4, 5, 6, 7, 8, 9};
const int leds[]    = {A0, A1, A2, A3, 10, 11, 12, 13};
const char* labels[] = {"2", "3", "0", "B", "C", "%23"}; 

const int NUM_BTNS = 8;

// ================= Timing =================
const unsigned long debounceDelay   = 50;
const unsigned long nextPressDelay  = 500;

bool prevState[NUM_BTNS];
unsigned long lastPress[NUM_BTNS];

// Ticket display timeout
unsigned long lastTicketTime = 0;
bool ticketDisplayed = false;

void setup() {
  //Serial.begin(115200);

  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
    prevState[i] = HIGH;
    lastPress[i] = 0;
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("No Ticket");

  // Connect to WiFi
  connectWiFi();
}

void loop() {
  // WiFi reconnect if needed
  if (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
    connectWiFi();
  }

  // Check buttons
  for (int i = 0; i < NUM_BTNS; i++) {
    checkButton(i);
  }

  // Timeout revert to "No Ticket" if needed
  // if (ticketDisplayed && (millis() - lastTicketTime >= 60000)) {
  //   lcd.clear();
  //   lcd.setCursor(3, 0);
  //   lcd.print("No Ticket");
  //   ticketDisplayed = false;
  // }
}

// ================= Button Check =================
void checkButton(int i) {
  bool currentState = digitalRead(buttons[i]);
  digitalWrite(leds[i], currentState == LOW ? HIGH : LOW);
 
  if (prevState[i] == HIGH && currentState == LOW &&
      (millis() - lastPress[i]) > (debounceDelay + nextPressDelay)) {

    lastPress[i] = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sending...");

    // Send to server
    String response = sendButtonPress(labels[i]);

    // Parse server response JSON
    handleJson(response);
  }

  prevState[i] = currentState;
}

// ================= WiFi =================
void connectWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();
  while ((WiFi.status() != WL_CONNECTED ||
          WiFi.localIP() == IPAddress(0, 0, 0, 0)) &&
         millis() - startAttempt < 15000) {
    delay(500);
  }

  IPAddress ip = WiFi.localIP();
  if (WiFi.status() == WL_CONNECTED && ip != IPAddress(0, 0, 0, 0)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(ip);
    delay(2000);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("No Ticket");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    delay(2000);
  }
}

// ================= HTTP =================
String sendButtonPress(const char* label) {
  HttpClient client = HttpClient(wifi, serverAddress, serverPort);

  String endpoint = "/api/key/";
  endpoint += label;

  client.beginRequest();
  client.get(endpoint);
  client.sendHeader("Connection", "close");
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (statusCode != 200) {
    return "";
  }
  return response;
}

// ================= JSON handling for LCD =================
void handleJson(String json) {
  if (json.length() == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error/No Resp");
    delay(1500);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("No Ticket");
    return;
  }

  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, json);

  if (err) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parse Error");
    lcd.setCursor(0, 1);
    lcd.print(json);
    lastTicketTime = millis();
    ticketDisplayed = true;
    return;
  }

  lcd.clear();

  // ========== CASE 1: server gives displayText (NEW_TICKET) ==========
  if (doc.containsKey("displayText")) {
    const char* displayText = doc["displayText"]; // e.g. MW16
    lcd.setCursor(4, 0);
    lcd.print("Ticket:");
    lcd.setCursor(5, 1);
    lcd.print(displayText);

    lastTicketTime = millis();
    ticketDisplayed = true;
    return;
  }

  // ========== CASE 2: server gives row object (CALLING / CALLED) ==========
  JsonObject row = doc["row"];

  const char* status  = row["status"];           // calling / called
  const char* service = row["ticketservice"];    // e.g. RT
  int number          = row["ticketnum"];        // e.g. 2

  if (status && String(status) == "calling") {
    lcd.setCursor(0, 0);
    lcd.print("Calling Ticket:");
  } else if (status && String(status) == "called") {
    lcd.setCursor(0, 0);
    lcd.print("Called Ticket:");
  } else if (status && String(status) == "voided") {
    lcd.setCursor(0,0);
    lcd.print("Voided Ticket:");
  }
  else {
    lcd.setCursor(3, 0);
    lcd.print("Ticket:");
  }

  lcd.setCursor(6, 1);
  if (service) { 
    lcd.print(service);
  }
  lcd.print(number);

  lastTicketTime = millis();
  ticketDisplayed = true;
}
