/*
  Grupo 16 4K1 - TP2 - TA 2025
*/
#include <HTTPClient.h>
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <PotentiometerReader.h>
#include <time.h>
#include "ThingSpeak.h"

// Configuración de tiempo NTP
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800; // GMT-3 (Argentina: -3 * 3600)
const int   daylightOffset_sec = 0; 

// #include <DHT.h> // Descomentar cuando tengamos la placa

// Conexión a Internet
const char* ssid = "";
const char* password = "";

// Inicializo el bot de telegram
#define BOTtoken ""
#define CHAT_ID ""

// Inicializo el display
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1  
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure secureClient; // Cliente Telegram (HTTPS)
WiFiClient standardClient;     // Cliente ThingSpeak (HTTP)
UniversalTelegramBot bot(BOTtoken, secureClient);

unsigned long myChannelNumber = ; //ID de canal teamspeak
const char * myWriteAPIKey = ""; // API de escritura

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = LOW;

const int POT_PIN = 34;
const float POT_MAX_RESISTANCE = 5000.0;
PotentiometerReader myPot(POT_PIN, POT_MAX_RESISTANCE);

// --- Descomentar si hay DHT22 ---
// #define DHTPIN 4
// #define DHTTYPE DHT22  
// DHT dht(DHTPIN, DHTTYPE);

enum DisplayMode {
  LED_STATUS,
  POT_STATUS,
  DHT_STATUS,
  OFF
};
DisplayMode currentDisplayMode = LED_STATUS; 
unsigned long lastDisplayUpdateTime;
const int displayUpdateDelay = 1000; 

bool isWifiConnected = false; 
unsigned long lastWifiCheckTime;
const int wifiCheckDelay = 5000;

// Variables para simular DHT22
float currentTemperature = 20.0; 
float currentHumidity = 40.0;
unsigned long lastSensorReadTime;
const int sensorReadDelay = 10000; 


// Funciones para estabilizar lecturas del potenciometro
float getStableResistance() {
  const int numReadings = 15;
  long total = 0;
  for (int j = 0; j < numReadings; j++) {
    total += myPot.readRaw();
  }
  int averageRaw = total / numReadings;
  float resistance = (averageRaw / 4095.0) * POT_MAX_RESISTANCE;
  return resistance;
}

void updateSensorReadings() {
  // --- Descomentar si hay DHT22 ---
  // currentTemperature = dht.readTemperature();
  // currentHumidity = dht.readHumidity();
  // if (isnan(currentTemperature) || isnan(currentHumidity)) {
  //   Serial.println("Failed to read from DHT!");
  //   // We'll just keep the old values if the read fails
  //   return; 
  // }

  // Codigo de sensor emulado
  currentTemperature = random(200, 250) / 10.0; // 20.0 - 25.0 C
  currentHumidity = random(400, 500) / 10.0;  // 40.0 - 50.0 %
}


void updateDisplay() {
display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  display.setTextSize(1);
  struct tm timeinfo;
  
  if(getLocalTime(&timeinfo)){
    char timeString[6]; 
    strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);
    display.setCursor(0, 0);
    display.print(timeString);

    char dateString[9];
    strftime(dateString, sizeof(dateString), "%d:%m:%y", &timeinfo);
    display.setCursor(80, 0); 
    display.print(dateString);
    
  } else {
    display.setCursor(0, 0);
    display.print("Syncing Time...");
  }

  int wifiX = 61; 
  int wifiY = 0;
  display.drawFastVLine(wifiX,     wifiY + 5, 3, SH110X_WHITE); // Bar 1
  display.drawFastVLine(wifiX + 3, wifiY + 3, 5, SH110X_WHITE); // Bar 2
  display.drawFastVLine(wifiX + 6, wifiY + 1, 7, SH110X_WHITE); // Bar 3

  if (!isWifiConnected) {
    display.drawLine(wifiX - 1, wifiY, wifiX + 8, wifiY + 8, SH110X_WHITE);
    display.drawLine(wifiX - 1, wifiY + 8, wifiX + 8, wifiY, SH110X_WHITE);
  }

  display.drawFastHLine(0, 9, SCREEN_WIDTH, SH110X_WHITE); 
  
  switch (currentDisplayMode) {
    
    case LED_STATUS: {
      display.setTextSize(2);
      display.setCursor(0, 12); 
      display.println("LED State");
      display.setTextSize(3);
      display.setCursor(20, 30);
      if (ledState) {
        display.println("ON");
      } else {
        display.println("OFF");
      }
      break;
    }

    case POT_STATUS: {
      display.setTextSize(2);
      display.setCursor(0, 12); 
      display.println("Potentiom.");

      // Lectura directa de resistencia
      float res_ohms = getStableResistance();
      
      display.setTextSize(3);
      display.setCursor(0, 30);
      
      // Codigo para ajustar unidades
      if (POT_MAX_RESISTANCE < 10000) {
        display.print(res_ohms, 0); 
        display.print(" O"); 
      } else {
        float res_kOhms = res_ohms / 1000.0;
        display.print(res_kOhms, 1); 
        display.print("k"); 
      }

      
      int barWidth = map(res_ohms, 0, POT_MAX_RESISTANCE, 0, 128);
      
      display.fillRect(0, 62, barWidth, 2, SH110X_WHITE);
      break;
    }

    case DHT_STATUS: {
      float t = currentTemperature; 
      float h = currentHumidity;
      
      display.setTextSize(2);
      display.setCursor(0, 12); 
      if (isnan(t) || isnan(h)) {
        display.println("DHT Error");
      } else {
        display.print(t, 1); display.println(" C");
        display.setCursor(0, 35);
        display.print(h, 1); display.println(" %");
      }
      break;
    }

    case OFF: {
      
      break;
    }
  }
  
  display.display(); 
}

// Seccion de manejo de mensajes

void handleNewMessages(int numNewMessages) {
Serial.println("handleNewMessages");

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      Serial.println("DEBUG: /start command received. Bypassing library.");
      
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the buttons below to control the ESP32.\n";
      welcome += "Type /hide to remove the keyboard.";

      StaticJsonDocument<1024> payloadDoc;
      payloadDoc["chat_id"] = chat_id;
      payloadDoc["text"] = welcome;

      JsonObject markup = payloadDoc.createNestedObject("reply_markup");
      JsonArray keyboard = markup.createNestedArray("keyboard");
      markup["resize_keyboard"] = true;

      
      JsonArray row1 = keyboard.createNestedArray();
      row1.createNestedObject()["text"] = "/led_on";
      row1.createNestedObject()["text"] = "/led_off";
      row1.createNestedObject()["text"] = "/state";
      
      JsonArray row2 = keyboard.createNestedArray();
      row2.createNestedObject()["text"] = "/dht";
      row2.createNestedObject()["text"] = "/resistance";
      
      JsonArray row3 = keyboard.createNestedArray();
      row3.createNestedObject()["text"] = "/displayLED";
      row3.createNestedObject()["text"] = "/displayPote";
      row3.createNestedObject()["text"] = "/displayDHT";
      
      JsonArray row4 = keyboard.createNestedArray();
      row4.createNestedObject()["text"] = "/senddata";
      row4.createNestedObject()["text"] = "/displayOff";

      String fullPayload;
      serializeJson(payloadDoc, fullPayload);

      // Se envia el request usando HTTPClient
      HTTPClient http;
      String url = "https://api.telegram.org/bot" + String(BOTtoken) + "/sendMessage";
      
      
      http.begin(secureClient, url); 
      http.addHeader("Content-Type", "application/json");
      
      Serial.println("DEBUG: Sending manual keyboard request...");
      Serial.println(fullPayload); 
      
      int httpCode = http.POST(fullPayload);
      
      Serial.print("DEBUG: Telegram response code: ");
      Serial.println(httpCode);
      if (httpCode > 0) {
        String response = http.getString();
        Serial.println(response); 
      }
      
      http.end();
    } 
    
    else if (text == "/hide") {
      
      String payload = "{\"chat_id\":\"" + chat_id + "\",\"text\":\"Keyboard hidden.\",\"reply_markup\":{\"remove_keyboard\":true}}";
      
      HTTPClient http;
      String url = "https://api.telegram.org/bot" + String(BOTtoken) + "/sendMessage";
      http.begin(secureClient, url);
      http.addHeader("Content-Type", "application/json");
      http.POST(payload);
      http.end();
    }
    
    else if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    } 
    
    else if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    } 
    
    else if (text == "/state") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED is ON", "");
      } else {
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }

    else if (text == "/resistance") {
      float resistance = getStableResistance();
      String message = "Stable Resistance: ";
      message += String(resistance / 1000.0, 2); 
      message += " kOhms";
      bot.sendMessage(chat_id, message, "");
    }
    
    else if (text == "/dht") {
      float t = currentTemperature;
      float h = currentHumidity;

      String message;
      if (isnan(t) || isnan(h)) {
        message = "Failed to read from DHT sensor!";
      } else {
        message = "(EMULATED)\n"; 
        message += "Temp: " + String(t, 1) + "°C\n";
        message += "Humidity: " + String(h, 1) + "%";
      }
      bot.sendMessage(chat_id, message, "");
    }

    // comandos display
    else if (text == "/displayLED"){
      currentDisplayMode = LED_STATUS;
      bot.sendMessage(chat_id, "Display now showing LED status.", "");
    }
    else if (text == "/displayPote"){
      currentDisplayMode = POT_STATUS;
      bot.sendMessage(chat_id, "Display now showing Potentiometer.", "");
    }
    else if (text == "/displayDHT"){
      currentDisplayMode = DHT_STATUS;
      bot.sendMessage(chat_id, "Display now showing DHT sensor.", "");
    }
    else if (text == "/displayOff"){
      currentDisplayMode = OFF;
      bot.sendMessage(chat_id, "Display Off.", "");
    }
    
    else if (text == "/senddata") { 
      bot.sendMessage(chat_id, "Gathering data, sending to ThingSpeak...", "");
      
      float led = (ledState) ? 1.0 : 0.0; 
      float pot = getStableResistance() / 1000.0;
      float temp = currentTemperature;
      float hum = currentHumidity;

      ThingSpeak.setField(1, temp); 
      ThingSpeak.setField(2, hum);
      ThingSpeak.setField(3, led);  
      ThingSpeak.setField(4, pot);  

      int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      if(httpCode == 200){
        bot.sendMessage(chat_id, "Data sent successfully.", "");
      }
      else{
        String errorMsg = "Problem sending data. HTTP error: " + String(httpCode);
        bot.sendMessage(chat_id, errorMsg, "");
      }
    }
    else {
      if (text.startsWith("/")) {
        bot.sendMessage(chat_id, "Unknown command. Type /start for help.", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  Wire.begin(); 
  ThingSpeak.begin(standardClient);
  
  // Descomentar si hay DHT22
  // dht.begin();
  
  if (!display.begin(i2c_Address, true)) {
    Serial.println(F("SH1106G allocation failed"));
    for(;;);
  }
  display.setContrast (10); 
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 BOT");
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.println("Connecting...");
  display.display();
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  analogSetPinAttenuation(POT_PIN, ADC_11db);

  // Conexion a wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    secureClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  isWifiConnected = true;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  updateSensorReadings();

  currentDisplayMode = LED_STATUS; 
  updateDisplay();
}

void loop() {
 // Fijarse si hay mensajes nuevos
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  // Actualizar display
  if (millis() > lastDisplayUpdateTime + displayUpdateDelay) {
    updateDisplay();
    lastDisplayUpdateTime = millis();
  }

  // Verificar conexion wifi
  if (millis() > lastWifiCheckTime + wifiCheckDelay) {
    if (WiFi.status() != WL_CONNECTED) {
      if (isWifiConnected) {
        Serial.println("WARNING: Wi-Fi connection lost!");
        isWifiConnected = false;
        WiFi.reconnect();
      }
    } else {
      if (!isWifiConnected) {
        Serial.println("INFO: Wi-Fi connection re-established.");
        isWifiConnected = true;
      }
    }
    lastWifiCheckTime = millis();
  }
  
  // ---Leer sensor periodicamente
  if (millis() > lastSensorReadTime + sensorReadDelay) {
    updateSensorReadings();
    lastSensorReadTime = millis();
  }
}