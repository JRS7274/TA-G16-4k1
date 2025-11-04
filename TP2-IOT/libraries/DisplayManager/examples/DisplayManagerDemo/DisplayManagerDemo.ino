#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DisplayManager.h> // Our new custom library

// --- Configuration from your working sketch ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // No reset pin
#define I2C_ADDRESS 0x3D // The address you found (e.g., 0x3D)

// --- MODIFIED ---
// Create the display object using your working constructor
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- MODIFIED ---
// Pass the display object AND the correct I2C address
// This now matches the library's constructor
DisplayManager displayManager(&display, I2C_ADDRESS);

void setup(void) {
  Serial.begin(115200);
  
  // Initialize the display manager. This also calls display.begin()
  displayManager.begin();
  
  // Show the home screen on boot
  displayManager.showHomeScreen();
  delay(2000);
}

void loop(void) {
  // Cycle through all the display modes as a test

  Serial.println("Showing LED Status (ON)");
  displayManager.showLEDStatus(true);
  delay(3000);

  Serial.println("Showing LED Status (OFF)");
  displayManager.showLEDStatus(false);
  delay(3000);

  Serial.println("Showing Potentiometer Status");
  displayManager.showPotStatus(47.2); // Show a dummy value
  delay(3000);

  Serial.println("Showing DHT Status");
  displayManager.showDHTStatus(24.5, 65.1); // Show dummy values
  delay(3000);

  Serial.println("Showing DHT Error");
  displayManager.showDHTStatus(NAN, NAN); // Show error screen
  delay(3000);

  Serial.println("Showing Home Screen");
  displayManager.showHomeScreen();
  delay(3000);
}