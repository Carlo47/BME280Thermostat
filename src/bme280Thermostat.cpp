/**
 * Program      bme280Thermostat.cpp
 * Author       2022-01-24 Charles Geiser
 * 
 * Purpose      The program shows how a Bosch BME280 temperature, humidity 
 *              and air pressure sensor is queried.  
 *              The sensor updates the measured values ​​every msRefresh 
 *              milliseconds and one of the user supplied callbacks 
 *              onDataReady(), onLowTemp() or onHighTemp() is called.
 * 
 *              The on an off of a heating is simulatetd by the built in led
 * 
 * Board        ESP32 DoIt DevKit V1 / Wemos D1 R2 / Uno R3
 *      
 * Wiring                  -----.
 *              BME280 Sensor   |-- Vcc --> 3.3 V  ESP32
 *              I2C             |-- GND --> GND
 *              Temperature     |-- SCL --> 22
 *              rel Humidity    |-- SDA --> 21
 *              air pressure    |-- CSB --> nc                        
 *                              |-- SDO --> nc
 *                         -----´
 * 
 * Remarks      1 Pa   = 7.50064 E-3 mmHG
 *              1 mmHG = 133.322 Pa
 * 
 *              kapa = 1.235
 *              K0 = kapa / (kapa -1) = 5.255
 *              1/K0 = 0.1903
 *              T0 = 288.15 °K = 15 °C (According an international convention this temperatur is used as reference)
 *              gradT = 0.0065 K/m (Temperature gradient Kelvin / Meter
 *              H0 = T0 / gradT = 44330 m
 * 
 *              Sea level air pressure at given local altitude:
 *              Used to calibrate the barometer to a known height
 *              P0 = pLocal / ((1 - hLocal/H0) ^ K0)
 *              P0 = 1013.25 hPa (nach int.Konvention, P0 = pSeaLevel))
 * 
 *              Local altitude for a given local air pressure:
 *              hLocal = H0 * (1 - (pLocal/P0) ^ 1/K0)
 * 
 *              Local air pressure at a given local altitude (international barometric hight formula):
 *              pLocal = P0 * (1 - h/H0) ^ K0
 * 
 * References
 */

#include "BME280Thermostat.h"

#define CLR_LINE    "\r                                                                                \r"
#define I2C_ADDRESS 0x76 

void turnHeatingOn();
void turnHeatingOff();
void processData();

void setLowerLimit();
void setUpperLimit();
void setDeltaTemp();
void setAltitude();
void setInterval();
void toggleThermostat();
void showValues();
void showMenu();

bool heatingIsOn = false;

BME280Thermostat myThermostat(I2C_ADDRESS, turnHeatingOn, turnHeatingOff, processData);

using MenuItem = struct { const char key; const char *txt; void (&action)(); };

MenuItem menu[] = 
{
  { 'l', "[l] Set lower limit      [°C]",         setLowerLimit },
  { 'u', "[u] Set upper limit      [°C]",         setUpperLimit },
  { 'a', "[a] Set local altitude   [m.a.s.l.]",   setAltitude },
  { 'i', "[i] Set refresh interval [ms]",         setInterval },
  { 't', "[t] Toggle thermostat enable/disable",  toggleThermostat },
  { 'v', "[v] Show values",                       showValues },
  { 'S', "[S] Show menu",                         showMenu },
};
constexpr uint8_t nbrMenuItems = sizeof(menu) / sizeof(menu[0]);

void showMenu()
{
  // title is packed into a raw string
  Serial.print(
  R"(
-----------------------------
    BME280Thermostat Demo 
-----------------------------
)");

  for (int i = 0; i < nbrMenuItems; i++)
  {
    Serial.println(menu[i].txt);
  }
}

void doMenu()
{
  char key = Serial.read();

  for (int i = 0; i < nbrMenuItems; i++)
  {
    if (key == menu[i].key)
    {
      menu[i].action();
      break;
    }
  } 
}

void setLowerLimit()
{
  float value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseFloat();
  }
  myThermostat.setLimitLow(value);
}

void setUpperLimit()
{
  float value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseFloat();
  }
  myThermostat.setLimitHigh(value);
}

void setAltitude()
{
  float value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseFloat();
  }
  myThermostat.setLocalAltitude(value);
}

void setInterval()
{
   long value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  myThermostat.setRefreshInterval(value);
}

/**
 * Enable or disable thermostat
 */
void toggleThermostat()
{
  char buf[24];
  myThermostat.isEnabled() ? myThermostat.disable() : myThermostat.enable();
  snprintf(buf, sizeof(buf), "Thermostat is %s\n", myThermostat.isEnabled() ? "enabled" : "disabled");
  Serial.println(buf);
}

void showValues()
{
  char buf[32];
  myThermostat.printSensorValues();
  myThermostat.printSettings();
  snprintf(buf, sizeof(buf), "Heating is %s", heatingIsOn ? "ON" : "OFF");
  Serial.println(buf);
}

/**
 * Called when temperature drops below low limit
 */
void turnHeatingOn()
  {
    char buf[80];
    if (! heatingIsOn)
    {
      snprintf(buf, sizeof(buf), "Turn heating on, temperature dropped below limit of %4.1f : %4.1f °C", 
              (double)myThermostat.getLimitLow(), (double)myThermostat.getCelsius());
      Serial.println(buf);
      digitalWrite(LED_BUILTIN, HIGH);            // Simulates turning heating on
      heatingIsOn = true;
    }
  }

/**
 * Called when tempereature exceeds high limit
 */
void turnHeatingOff()
  {
    char buf[80];
    if (heatingIsOn)
    {
      snprintf(buf, sizeof(buf), "Turn heating off, temperature exeeds limit of %4.1f : %4.1f °C", 
              (double)myThermostat.getLimitHigh(), (double)myThermostat.getCelsius());
      Serial.println(buf);
      digitalWrite(LED_BUILTIN, LOW);  // Simulates turning off heating
      heatingIsOn = false;
    }
  }

/**
 * Called every msRefresh milliseconds
 * Do something with the sensor readings
 */
void processData()
{
  showValues();
}

/**
 * Show default pins of SPI and I2C interface
 */
void ShowSPIandI2CdefaultPins()
{
  char buf[160];
  snprintf(buf, sizeof(buf), R"(--- SPI / I2C default pins ---
SPI MOSI = %d
SPI MISO = %d
SPI SCK  = %d
SPI SS   = %d
I2C SCL  = %d
I2C SDA  = %d
)", MOSI, MISO, SCK, SS, SCL, SDA);
  Serial.print(buf);
  // Serial.printf("SPI MOSI = %d\n", MOSI);
  // Serial.printf("SPI MISO = %d\n", MISO);
  // Serial.printf("SPI SCK  = %d\n", SCK);
  // Serial.printf("SPI SS   = %d\n", SS);
  // Serial.printf("I2C SCL  = %d\n", SCL);
  // Serial.printf("I2C SDA  = %d\n", SDA);
}

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  ShowSPIandI2CdefaultPins();

  myThermostat.setup();
  myThermostat.setLocalAltitude(410.0); // default is 400 m.a.s.l.
  myThermostat.setLimitHigh(22.0);      // default is 21 °C
  myThermostat.setLimitLow(21.0);       // default is 18 °C
  showMenu();
}

void loop() 
{
  if(Serial.available()) doMenu();
  myThermostat.loop(); 
}