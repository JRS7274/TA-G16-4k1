#ifndef DisplayManager_h
#define DisplayManager_h

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h> // This parent library is still correct

enum DisplayMode {
  MODE_HOME,
  MODE_LED,
  MODE_POT,
  MODE_DHT,
  MODE_OFF
};

class DisplayManager {
  public:
    // --- MODIFIED ---
    // Changed to accept an Adafruit_SH1106G pointer
    DisplayManager(Adafruit_SH1106G* display, uint8_t i2c_addr = 0x3C);
    
    void begin();
    void setMode(DisplayMode newMode);
    DisplayMode getMode();
    void clear();

    // Specific screen-drawing functions
    void showHomeScreen();
    void showLEDStatus(bool ledState);
    void showPotStatus(float resistanceInKOhms);
    void showDHTStatus(float temp, float humidity);
    
  private:
    // --- MODIFIED ---
    Adafruit_SH1106G* _display; // Changed to Adafruit_SH1106G
    DisplayMode _currentMode;
    uint8_t _i2c_addr; 
};

#endif