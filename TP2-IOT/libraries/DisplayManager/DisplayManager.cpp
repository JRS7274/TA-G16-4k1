#include "DisplayManager.h"

// --- MODIFIED ---
// This now correctly matches the .h file by using Adafruit_SH1106G
DisplayManager::DisplayManager(Adafruit_SH1106G* display, uint8_t i2c_addr) {
  _display = display;
  _i2c_addr = i2c_addr; // Save the address
  _currentMode = MODE_HOME;
}

void DisplayManager::begin() {
  if (!_display->begin(_i2c_addr, true)) { 
    Serial.println(F("SH1106G allocation failed")); 
    for(;;);
  }
  _display->clearDisplay();
  _display->display();
  delay(100); 
}

// Set the current display mode
void DisplayManager::setMode(DisplayMode newMode) {
  _currentMode = newMode;
}

// Get the current display mode
DisplayMode DisplayManager::getMode() {
  return _currentMode;
}

// Clear the display
void DisplayManager::clear() {
  _display->clearDisplay();
  _display->display();
}

// --- Specific Screen Functions ---
// (These are all correct and don't need changes)

void DisplayManager::showHomeScreen() {
  _display->clearDisplay();
  _display->setTextSize(2);
  _display->setTextColor(SH110X_WHITE);
  _display->setCursor(0,0);
  _display->println("ESP32 BOT");
  _display->setTextSize(1);
  _display->setCursor(0, 20);
  _display->println("Send /start for");
  _display->println("a list of commands");
  _display->display();
}

void DisplayManager::showLEDStatus(bool ledState) {
  _display->clearDisplay();
  _display->setTextSize(2);
  _display->setTextColor(SH110X_WHITE);
  _display->setCursor(0,0);
  _display->println("LED State");
  _display->setTextSize(3);
  _display->setCursor(20, 30);
  if (ledState) { _display->println("ON"); } else { _display->println("OFF"); }
  _display->display();
}

void DisplayManager::showPotStatus(float res) {
  _display->clearDisplay();
  _display->setTextSize(2);
  _display->setTextColor(SH110X_WHITE);
  _display->setCursor(0,0);
  _display->println("Potentiom.");
  _display->setTextSize(3);
  _display->setCursor(0, 30);
  _display->print(res, 1);
  _display->print("k");
  _display->display();
}

void DisplayManager::showDHTStatus(float t, float h) {
  _display->clearDisplay();
  _display->setTextSize(2);
  _display->setTextColor(SH110X_WHITE);
  _display->setCursor(0,0);
  
  if (isnan(h) || isnan(t)) {
    _display->println("DHT Error");
  } else {
    _display->print(t, 1); _display->println(" C");
    _display->setCursor(0, 35);
    _display->print(h, 1); _display->println(" %");
  }
  _display->display();
}