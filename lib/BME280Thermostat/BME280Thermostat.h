/**
 * BME280Thermostat.h
 * 
 * Declaration of the derived class BME280Thermostat. It inherits from the base class
 * BME280Sensor. The callbacks onLowTemp(), onHighTemp and onDataReady() must be supplied
 * by the application program.
 */
#ifndef _BME280THERMOSTAT_H_
#define _BME280THERMOSTAT_H_
#include "BME280Sensor.h"

using Callback = void(&)();

class BME280Thermostat : public BME280Sensor 
{
  public:
    BME280Thermostat(uint8_t i2cAddress, 
      Callback onLowTemp, Callback onHighTemp, Callback onDataReady) : 
      BME280Sensor(i2cAddress), _onLowTemp(onLowTemp), _onHighTemp(onHighTemp), _onDataReady(onDataReady)
    {}

    void setup();
    void loop();
    void enable();
    void disable();
    bool isEnabled();
    void setRefreshInterval(uint32_t msRefresh);  // msec
    void setLimitLow(float tLimitLow);            // °C
    void setLimitHigh(float tLimitHigh);          // °C
    float getLimitLow();
    float getLimitHigh();
    uint32_t getRefreshInterval();
    void printSettings();

  private:
    bool     _isEnabled  = false;
    float    _tLimitLow  = 18.0;
    float    _tLimitHigh = 21.0;
    uint32_t _msRefresh  = 5000;
    Callback _onLowTemp;
    Callback _onHighTemp;
    Callback _onDataReady;
};
#endif