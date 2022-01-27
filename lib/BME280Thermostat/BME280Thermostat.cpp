/**
 * Class        BME280Thermostat.cpp
 * Author       2022-01-24 Charles Geiser
 * 
 * Purpose      Implements a class BME280thermostat which is derived from the
 *              base class BME280Sensor. 
 *              The constructors arguments are the I2C address of the sensor 
 *              and references to the 3 callbacks onDataReady(), onLowTemp() 
 *              and onHighTemp() which must be supplied by the application  
 *              program of the class.      
 * 
 * Board        ESP32 DoIt DevKit V1
 * 
 * Remarks        
 * References
 */
#include "BME280Thermostat.h"

void BME280Thermostat::setup()
{
  BME280Sensor::setup();         // Call setup of base class
}

void BME280Thermostat::loop()
{
  if((millis() % _msRefresh) == 0 && _isEnabled) 
  {
    float t = getCelsius();
    _onDataReady();
    if (t < _tLimitLow) _onLowTemp();
    if (t > _tLimitHigh) _onHighTemp();
  }
}

void BME280Thermostat::enable()
{
  _isEnabled = true;
}

void BME280Thermostat::disable()
{
  _isEnabled = false;
}

bool BME280Thermostat::isEnabled()
{
  return _isEnabled;
}

void BME280Thermostat::setRefreshInterval(uint32_t msRefresh)
{
  _msRefresh = msRefresh;
}

  void BME280Thermostat::setLimitLow(float tLow)
  {
    _tLimitLow = tLow;
  }

    void BME280Thermostat::setLimitHigh(float tHigh)
  {
    _tLimitHigh = tHigh;
  }

float BME280Thermostat::getLimitLow()
{
  return _tLimitLow;
}

float BME280Thermostat::getLimitHigh()
{
  return _tLimitHigh;
}

void BME280Thermostat::printSettings()
{
  char buf[140];
  // printf("Upper limit      %6.1f °C\n", _tLimitHigh);
  // printf("Lower limit      %6.1f °C\n", _tLimitLow);
  // printf("Refresh interval %6d ms\n", _msRefresh);
  // printf("Thermostat is %s\n", _isEnabled ? "enabled" : "disabled");
  snprintf(buf, sizeof(buf), R"(--- Thermostat settings ---
Upper limit      %6.1f °C
Lower limit      %6.1f °C
Refresh interval %6ld ms
Thermostat is %s
)", (double)_tLimitHigh, (double)_tLimitLow, _msRefresh, _isEnabled ? "enabled" : "disabled");
Serial.println(buf);
}


