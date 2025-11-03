#include "Arduino.h"
#include "PotentiometerReader.h"

// Constructor implementation
PotentiometerReader::PotentiometerReader(uint8_t pin, float maxResistance) {
  _pin = pin;
  _maxResistance = maxResistance;
}

// Reads the raw integer value from the ADC
int PotentiometerReader::readRaw() {
  return analogRead(_pin);
}

// Converts the raw ADC value to a voltage
float PotentiometerReader::readVoltage() {
  // Maps the 0-4095 range to the 0-3.3V range
  return (readRaw() / _adcMax) * _vcc;
}

// Calculates the resistance based on the wiper's position
float PotentiometerReader::readResistance() {
  // The resistance is directly proportional to the ADC reading
  return (readRaw() / _adcMax) * _maxResistance;
}

// Calculates the position as a percentage
float PotentiometerReader::readPercentage() {
  return (readRaw() / _adcMax) * 100.0;
}