/**
 * Header       BME280Sensor.h
 * Author       2021-10-25 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Declaration of the class BME280Thermostat
 *              Inherits from BME280Sensor class
 * 
 * Constructor
 * arguments    i2cAddress    Address of the I2C-Interface of the BME280 sensor
 */
#include "BME280Sensor.h"

#ifndef _BME280THERMOSTAT_H
#define _BME280THERMOSTAT_H

typedef void(*CallbackFunction)();

class BME280ThermostatData
{
    public:
        uint32_t msPrevious = 0;
        uint32_t msRefresh  = 10000;   // user defined refresh interval
        float    tLimitHigh = 20.0;    // user defined upper temperature limit
        float    tLimitLow  = 19.0;    // user defined lower temperature limit (or calculated, when temperature range is set)
        float    tDelta     =  1.0;    // user defined temperature range (or calculated, when lower temperature is set)
};

class BME280Thermostat : public BME280Sensor
{
    private:
        static void _nop(){};
        BME280ThermostatData  _data;
        CallbackFunction _onLowTempReached  = _nop;
        CallbackFunction _onHighTempReached = _nop;
        CallbackFunction _onDataReady       = _nop;

    public:
        BME280Thermostat(uint8_t i2cAddress) : BME280Sensor(i2cAddress) {}

        void setRefreshInterval(uint32_t msRefresh);
        void setLimitHigh(float tHigh);
        void setLimitLow(float tLow);
        void setTempDelta(float delta);
        void getThermostatData(BME280ThermostatData &data);
        void loop();
        void addOnLowTempReachedCB(CallbackFunction cb);
        void addOnHighTempReachedCB(CallbackFunction cb);
        void addOnDataReadyCB(CallbackFunction cb);
        void printThermostatData();
};
#endif