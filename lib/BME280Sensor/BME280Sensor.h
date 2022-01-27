/**
 * BME280Sensor.h
 * 
 * Declaration of the class BME280Sensor. It inherits from Adafruit_BME280.
 * The constructors argument is the I2C address of the sensor. 
 */ 
#ifndef _BME280SENSOR_H_
#define _BME280SENSOR_H_
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class BME280Sensor : public Adafruit_BME280
{  
    public:
        BME280Sensor(uint8_t i2cAddress) : Adafruit_BME280(), _i2cAddress(i2cAddress)
        {}

        void  setup();
        void  calibrateForAltitude(float altitude);
        void  setLocalAltitude(float altitude);
        float getCelsius();
        float getFahrenheit();
        float getRelHumidity();
        float getDewPoint();
        float getLocalPressure();
        float getLocalAltitude();
        void  printSensorValues();

    private:
        void  _readSensor();
        float _calculateDewPoint();
        float _calculateNpLocal();

        uint8_t _i2cAddress;
        float _tCelsius;
        float _tFahrenheit;
        float _relHumidity;
        float _pLocal;
        float _pSeaLevel = 1013.25F; // normal air pressure at sea level
        float _npLocal;              // local normal air pressure at given local altitude
        float _altLocal = 400.0F;
        float _dewPoint;
};
#endif