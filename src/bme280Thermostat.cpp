/**
 * Program      bme280Thermostat.cpp
 * Author       2021-10-22 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Demonstrates how to use the class BME280Thermostat. 
 *              It turns a heating (LED_BUILTIN) on and off according
 *              the measured temperature and the set limits for
 *              upper and lower temperature. The program is operated
 *              via a CLI Menue.
 * 
 * Board        Arduino UNO
 * 
 * Wiring         BME280              Arduino UNO
 *              .--------.          .-------------
 *              |    Vcc o --->     o 5V
 *              |    GND o --->     o GND
 *              |    SCK o --->     o A5
 *              |    SDA o --->     o A4
 *              |    CSB o          |
 *              |    SDO o          |
 *              `--------´          `-------------
 *
 * Remarks
 *
 * References
 */
#include "BME280Thermostat.h"
#define CLR_LINE    "\r                                                                                \r"

#define BME280_I2C_ADDR 0x76

BME280Thermostat     thermostat(BME280_I2C_ADDR);
BME280SensorData     sensorData;
BME280ThermostatData thermostatData;
bool heatingIsOn = false;

void setLowerLimit();
void setUpperLimit();
void setDeltaTemp();
void setAltitude();
void toggleHeating();
void setInterval();
void showValues();
void showMenu();
bool heatingIsEnabled = false;

typedef struct { const char key; const char *txt; void (&action)(); } MenuItem;
MenuItem menu[] = 
{
  { 'l', "[l] Set lower limit [°C]",   setLowerLimit },
  { 'u', "[u] Set upper limit [°C]",   setUpperLimit },
  { 'd', "[d] Set delta T",            setDeltaTemp },
  { 'a', "[a] Set local altitude [m]", setAltitude },
  { 'i', "[i] Set refresh interval",   setInterval },
  { 't', "[t] Toggle heating on/off",  toggleHeating },
  { 'v', "[v] Show values",            showValues },
  { 'S', "[S] Show menu",              showMenu },
};
constexpr uint8_t nbrMenuItems = sizeof(menu) / sizeof(menu[0]);

void showMenu()
{
  // title is packed into a raw string
  Serial.print(
  R"TITLE(
-----------------------
 BME280Thermostat Demo 
-----------------------
)TITLE");

  for (int i = 0; i < nbrMenuItems; i++)
  {
    Serial.println(menu[i].txt);
  }
  Serial.print("\nPress a key: ");
}

void doMenu()
{
  char key = Serial.read();
  Serial.print(CLR_LINE);
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
  thermostat.setLimitLow(value);
  thermostat.getThermostatData(thermostatData);
}

void setUpperLimit()
{
  float value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseFloat();
  }
  thermostat.setLimitHigh(value);
  thermostat.getThermostatData(thermostatData);
}

void setDeltaTemp()
{
  float value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseFloat();
  }
  thermostat.setTempDelta(value);
  thermostat.getThermostatData(thermostatData);
}

void setAltitude()
{
  float value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseFloat();
  }
  thermostat.setLocalAltitude(value);
}

void setInterval()
{
   long value = 0;

  delay(2000);
  while (Serial.available())
  {
    value = Serial.parseInt();
  }
  thermostat.setRefreshInterval(value);
  thermostat.getThermostatData(thermostatData);
}

void toggleHeating()
{
  heatingIsEnabled = !heatingIsEnabled;
}

void showValues()
{
  thermostat.printSensorData();
  thermostat.printThermostatData();
}

/**
 * Do something with the measured values
 * e.g. show actual temperature and the 
 * set limits of the thermostat and also
 * the state of the heating
 */
void processData()
{
  char buf[48];
  thermostat.getSensorData(sensorData);
  snprintf(buf, sizeof(buf), R"(
%4.1f [%4.1f ... %4.1f] heating is %s)",
    (double)sensorData.tCelsius,
    (double)thermostatData.tLimitLow,
    (double)thermostatData.tLimitHigh,
    heatingIsOn ? "ON" : "OFF"
    );
  Serial.println(buf);
}

/**
 * Turn heating on when temperature 
 * is below lower limit
 */
void turnHeatingOn()
{
  if (! heatingIsOn)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    heatingIsOn = true;
    Serial.println("Heating turned on");
  }
}

/**
 * Turn heating off when temperature 
 * is beyond upper limit
 */
void turnHeatingOff()
{
  if (heatingIsOn)
  {
    digitalWrite(LED_BUILTIN, LOW);
    heatingIsOn = false;
    Serial.println("Heating turned off");
  }
}

void initThermostat()
{
  while (! thermostat.begin()) { delay(1000); }
  thermostat.setLocalAltitude(405.0);
  thermostat.addOnDataReadyCB(processData);   // monitors the heating
  thermostat.addOnHighTempReachedCB(turnHeatingOff);
  thermostat.addOnLowTempReachedCB(turnHeatingOn);
  thermostat.setLimitHigh(23.0);              // set the limits so that you can watch the heating turn on and off
  thermostat.getThermostatData(thermostatData);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  initThermostat();
  showMenu();
}

void loop()
{
  if(Serial.available()) doMenu();
  if (heatingIsEnabled) thermostat.loop();
}
