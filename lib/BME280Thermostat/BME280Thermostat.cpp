/**
 * Class        BME280Thermostat.cpp
 * Author       2021-10-25 Charles Geiser (https://www.dodeka.ch)
 *
 * Purpose      Implements a class BME280Thermostat that allows to switch a heater on or off
 * 
 * Methods      void loop();                                     // To be called in the loop of your main program
 *              void setRefreshInterval(uint32_t msRefresh)      // Sets the update interval between measurements
 *              void setLimitLow(float tLow)                     // Sets the lower temperature limit
 *              void setLimitHigh(float tHigh)                   // Sets the upper temperature limit
 *              void setTempDelta(float delta)                   // Sets the allowed temperature range
 *              void addOnLowTempReachedCB(CallbackFunction cb)  // Adds a callback to be called when temperature drops below lower limit
 *              void addOnHighTempReachedCB(CallbackFunction cb) // Adds a callback to be called when temperature exceeds the upper limit
 *              void addOnDataReadyCB(CallbackFunction cb)       // Adds a callback to be called regularly every interval
 *              void printThermostatData()                       // Outputs the parameters of the thermostat to the monitor
 * 
 * Board        Arduino UNO
 * 
 * Remarks      
 * 
 * References    
 */
#include "BME280Thermostat.h"

void BME280Thermostat::setRefreshInterval(uint32_t msRefresh)
{
    _data.msRefresh = msRefresh;
}

void BME280Thermostat::setLimitLow(float tLow)
{
    _data.tLimitLow = tLow;
    _data.tLimitHigh = _data.tLimitLow + _data.tDelta;
}

void BME280Thermostat::setLimitHigh(float tHigh)
{
    _data.tLimitHigh = tHigh;
    _data.tLimitLow = _data.tLimitHigh - _data.tDelta;
}

void BME280Thermostat::setTempDelta(float delta)
{
    _data.tDelta = delta;
    _data.tLimitLow = _data.tLimitHigh - _data.tDelta;
}

void BME280Thermostat::loop()
{
    if (millis() > _data.msPrevious + _data.msRefresh )
    {
        readSensor();
        _onDataReady();
        if (_sensorData.tCelsius >= _data.tLimitHigh) _onHighTempReached();
        if (_sensorData.tCelsius < _data.tLimitLow)  _onLowTempReached();
        _data.msPrevious = millis();
    }
}

void BME280Thermostat::addOnLowTempReachedCB(CallbackFunction cb)
{
    _onLowTempReached = cb;
}

void BME280Thermostat::addOnHighTempReachedCB(CallbackFunction cb)
{
    _onHighTempReached = cb;
}

void BME280Thermostat::addOnDataReadyCB(CallbackFunction cb)
{
    _onDataReady = cb;
}

void BME280Thermostat::printThermostatData()
{
    char buf[120];

    snprintf(buf, sizeof(buf), R"(
    Lower limit   %6.1f °C
    Upper limit   %6.1f °C
    Delta T       %6.1f °C
    Refresh interval %5d)",
        (double)_data.tLimitLow,
        (double)_data.tLimitHigh,
        (double)_data.tDelta,
        (int)_data.msRefresh);
    Serial.println(buf);
}

void BME280Thermostat::getThermostatData(BME280ThermostatData &data)
{
    data = _data;
}