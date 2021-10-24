/**
 * Class        BME280Thermostat.cpp
 * Author       2021-10-22 Charles Geiser (https://www.dodeka.ch)
 *
 * Purpose      Implements a class for the BME280 temperature, humidity and air pressure sensor
 *              It also calculates the dewpoint and the local standard air pressure when local
 *              altitude is known.
 *              The sensor readings are stored in the BME280SensorData data structure. 
 *              They can be copied to a variable of the same type in the main program 
 *              using the getSensorData() method. 
 * 
 * Methods      bool begin();       // to be called in setup of main program
 *              void readSensor();  // updates measurements into BME280SensorData
 *              void getSensorData(BME280SensorData &data);  // copy measurements into the variable data
 *              void setLocalAltitude(float meter);          // set the local altitude in meters above sea level
 *              void printSensorData();                      // print measurements to the monitor
 * 
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