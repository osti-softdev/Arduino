// FINAL. ARDUINO UNO WITH  LCD, BUTTONS

#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buttons and LEDs (same as before) …
const int buttons[] = {2,3,4,5,6,7,8,9};
const int leds[]    = {A0,A1,A2,A3,10,11,12,13};
const char* labels[] = {"2","3","0","B","C","#"};

const int NUM_BTNS=8;
const unsigned long debounceDelay=50;
const unsigned long nextPressDelay=500;
bool prevState[NUM_BTNS]={HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
unsigned long lastPress[NUM_BTNS]={0,0,0,0,0,0,0,0};

unsigned long lastTicketTime=0;
bool ticketDisplayed=false;

void setup(){
  Serial.begin(9600);
  for(int i=0;i<NUM_BTNS;i++){
    pinMode(buttons[i],INPUT_PULLUP);
    pinMode(leds[i],OUTPUT);
    digitalWrite(leds[i],LOW);
  }
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("No Ticket");
}

void loop(){
  for(int i=0;i<NUM_BTNS;i++){
    checkButton(i);
  }

  if(Serial.available()>0){
    String json=Serial.readStringUntil('\n');
    handleJson(json);
  }

  // if(ticketDisplayed && (millis()-lastTicketTime>=60000)){
  //   lcd.clear();
  //   lcd.setCursor(2,0);
  //   lcd.print("No Ticket");
  //   ticketDisplayed=false;
  // }
}

void checkButton(int i){
  bool currentState=digitalRead(buttons[i]);
  digitalWrite(leds[i],currentState==LOW?HIGH:LOW);
  if(prevState[i]==HIGH && currentState==LOW && 
     (millis()-lastPress[i])>(debounceDelay+nextPressDelay)){
    lastPress[i]=millis();
    Serial.println(labels[i]);
  }
  prevState[i]=currentState;
}

void handleJson(String json){
  // Prepare a JSON document
  StaticJsonDocument<200> doc;
  DeserializationError err = deserializeJson(doc,json);
  if(err){
    // fallback to raw message (maybe just ticketnum)
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Ticket:");
    lcd.print(json);
    lastTicketTime = millis();
    ticketDisplayed = true;
    return;
  }

  const char* status  = doc["status"];          // "pending", "calling", "called"
  const char* service = doc["ticketservice"];   // e.g. "RT"
  int number          = doc["ticketnum"];       // ticket number

  lcd.clear();

  // Show text depending on status
  if(status && String(status) == "calling"){
    lcd.setCursor(0,0);
    lcd.print("Calling Ticket:");
  } 
  else if(status && String(status) == "called"){
    lcd.setCursor(0,0);
    lcd.print("Called Ticket:");
  } 
  else {
    lcd.setCursor(0,0);
    lcd.print("Ticket:");
  }

  // Second line: service + number
  lcd.setCursor(6,1);
  if(service) lcd.print(service);
  lcd.print(number);

  lastTicketTime = millis();
  ticketDisplayed = true;
}
