/**
 * Header       BME280Sensor.h
 * Author       2021-10-22 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Declaration of the class BME280Sensor
 *              Inherits from Adafruit_BME280 class
 * 
 * Constructor
 * arguments    i2cAddress    Address of the I2C-Interface of the BME280 sensor
 */
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#ifndef _BME280SENSOR_H_
#define _BME280SENSOR_H_

class BME280SensorData
{
    public:
        float tCelsius    = 20.0;    // sensor reading
        float relHumidity = 40.0;    // sensor reading
        float pLocal      = 975.0;   // sensor reading
        float dewPoint    =   6.0;   // calculated dewpoint for temperature=20°C and humidity=40.0%
        float aLocal      = 453.0;   // user defined local altitude
        float npLocal     = 960.0;   // calculated normal air pressure for local altitude=453müM
        float pSeaLevel   = 1013.25; // standard air pressure at sealevel
};

class BME280Sensor : public Adafruit_BME280
{
    protected:
        uint8_t          _i2cAddress;
        BME280SensorData _sensorData;

    public:
        BME280Sensor(uint8_t i2cAddress) : Adafruit_BME280(), _i2cAddress(i2cAddress) {}
        bool begin();       // to be called in setup of main program
        void readSensor();  // updates measurements into BME280SensorData
        void getSensorData(BME280SensorData &data);  // copy measurements into the variable data
        void setLocalAltitude(float meter);          // set the local altitude in meters above sea level
        void printSensorData();                      // print measurements to the monitor

    private:
        float   _calculateDewPoint();
        float   _calculateNpLocal();
};
#endif