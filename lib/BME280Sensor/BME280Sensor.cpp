/**
 * Class        BME280Sensor.cpp
 * Author       2021-10-25 Charles Geiser (https://www.dodeka.ch)
 *
 * Purpose      Implements a class for the BME280 temperature, humidity and air pressure sensor
 *              It also calculates the dewpoint and the local standard air pressure when local
 *              altitude is known.
 *              The sensor readings are stored in the BME280SensorData data structure. 
 *              They can be copied to a variable of the same type in the main program 
 *              using the getSensorData() method. 
 * 
 * Methods      bool begin();                                // To be called in setup of main program
 *              void readSensor();                           // Updates the measured values in the data structure BME280SensorData
 *              void getSensorData(BME280SensorData &data);  // Copy measured values into the variable data
 *              void setLocalAltitude(float meter);          // Sets the local altitude in meters above sea level
 *              void printSensorData();                      // Outputs the measured values to the monitor
 * 
 * 
 * Board        Arduino UNO
 * 
 * Remarks      
 * 
 * References    
 */
#include "BME280Sensor.h"

/**
 * Calculate dewpoint from given temperature and rel. humidity
 */
float BME280Sensor::_calculateDewPoint() 
{
    float k;
    k = log(_sensorData.relHumidity/100) + (17.62 * _sensorData.tCelsius) / (243.12 + _sensorData.tCelsius);
    return 243.12 * k / (17.62 - k);
}


/**
 * Calculate local normal air pressure at given altitude
 * 
 * kapa = 1.235
 * K0 = kapa / (kapa -1) = 5.255
 * T0 = 288.15 °K = 15 °C (According an international convention this temperatur is used as reference)
 * gradT = 0.0065 K/m (Temperature gradient Kelvin / Meter
 * P0 = 1013.25 hPa (accord. int. convention, P0 = pSeaLevel))
 * H0 = T0 / gradT = 44330 m
 * pLocal = P0 * (1 - h/H0) ^ K0
 */ 
float BME280Sensor::_calculateNpLocal()
{
    return 1013.25 * pow( (1.0 - _sensorData.aLocal / 44330.0), 5.255);
}

void BME280Sensor::readSensor()
{
    _sensorData.tCelsius    = readTemperature();
    _sensorData.relHumidity = readHumidity();
    _sensorData.pLocal      = readPressure() / 100.0;
    _sensorData.dewPoint    = _calculateDewPoint();
    _sensorData.npLocal     = _calculateNpLocal();      
}

/**
 * Initialize the sensor and read available data
 */
bool BME280Sensor::begin()
{
    char buf[64];
    if (! Adafruit_BME280::begin(_i2cAddress))
    {
        snprintf(buf, sizeof(buf), R"(===> BME280Sensor not found on i2c address %u / 0x%x)", _i2cAddress, _i2cAddress);
        Serial.println(buf);
        return false;
    }
    else
    {
        readSensor();
        return true;
    }    
}

/**
 * Set the local altitude in meters above sea level (masl)
 * and calculate the standard air pressure for that altitude
 */
void BME280Sensor::setLocalAltitude(float meterAltitude)
{
    _sensorData.aLocal  = meterAltitude;
    _sensorData.npLocal = _calculateNpLocal();
}

/**
 * Print measured sensor data
 */
void BME280Sensor::printSensorData()
{
    char buf[208];

    snprintf(buf, sizeof(buf), R"(
    Temperature   %6.1f °C
    Humidity      %6.1f %%rF
    Air pressure  %6.1f hPa
    Dewpoint      %6.1f °C
    SeaLevel prs  %6.1f hPa
    Altitude      %6.1f masl
    Standard prs  %6.1f hPa)", 
        (double)_sensorData.tCelsius,
        (double)_sensorData.relHumidity,
        (double)_sensorData.pLocal,
        (double)_sensorData.dewPoint,
        (double)_sensorData.pSeaLevel,
        (double)_sensorData.aLocal,
        (double)_sensorData.npLocal);
    Serial.println(buf);
}

/**
 * Get sensor readings into the variable data
 */
void BME280Sensor::getSensorData(BME280SensorData &data)
{
    data = _sensorData;
}