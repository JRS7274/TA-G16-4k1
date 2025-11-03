#ifndef PotentiometerReader_h
#define PotentiometerReader_h

#include "Arduino.h"

class PotentiometerReader {
  public:
    // Constructor: Initializes the reader with the analog pin and the pot's max resistance.
    PotentiometerReader(uint8_t pin, float maxResistance);

    // Methods to get different readings from the potentiometer.
    int readRaw();            // Reads the raw ADC value (0-4095)
    float readVoltage();      // Reads the voltage from the wiper (0-3.3V)
    float readResistance();   // Calculates the current resistance in Ohms
    float readPercentage();   // Calculates the wiper position as a percentage (0-100%)

  private:
    uint8_t _pin;             // GPIO pin the potentiometer is connected to
    float _maxResistance;     // Maximum resistance of the potentiometer in Ohms
    const float _vcc = 3.3;   // ESP32 reference voltage
    const float _adcMax = 4095.0; // ESP32's 12-bit ADC resolution (0-4095)
};

#endif