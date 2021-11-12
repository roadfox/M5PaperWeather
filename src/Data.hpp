/**
  * @file Data.h
  * 
  * Class with all the global runtime data.
  */
#pragma once

#include "Weather.hpp"
#include <nvs.h>


/**
  * Class for collecting all the global data.
  */
class MyData
{
public:
   uint16_t nvsCounter;      //!< Non volatile counter

   int     wifiRSSI;         //!< The wifi signal strength
   float   batteryVolt;      //!< The current battery voltage
   int     batteryCapacity;  //!< The current battery capacity
   int     sht30Temperatur;  //!< SHT30 temperature
   int     sht30Humidity;    //!< SHT30 humidity

   Weather weather;          //!< All the openweathermap data

public:
   MyData()
      : wifiRSSI(0)
      , batteryVolt(0.0)
      , batteryCapacity(0)
      , sht30Temperatur(0)
      , sht30Humidity(0)
   {
   }

   /* helper function to dump all the collected data */
   void Dump()
   {
      Serial.println("DateTime: "        + getRTCDateTimeString());
      
      Serial.println("Latitude: "        + String(LATITUDE));
      Serial.println("Longitude: "       + String(LONGITUDE));
      Serial.println("WifiRSSI: "        + String(wifiRSSI));
      Serial.println("BatteryVolt: "     + String(batteryVolt));
      Serial.println("BatteryCapacity: " + String(batteryCapacity));
      Serial.println("Sht30Temperatur: " + String(sht30Temperatur));
      Serial.println("Sht30Humidity: "   + String(sht30Humidity));
      
      Serial.println("Sunrise: "         + getDateTimeString(weather.sunrise));
      Serial.println("Sunset: "          + getDateTimeString(weather.sunset));
      Serial.println("Winddir: "         + String(weather.winddir));
      Serial.println("Windspeed: "       + String(weather.windspeed));
   }

   /* Load the NVS data from the non volatile memory */
   void LoadNVS()
   {
      nvs_handle nvs_arg;
      nvs_open("Setting", NVS_READONLY, &nvs_arg);
      nvs_get_u16(nvs_arg, "nvsCounter", &nvsCounter);
      nvs_close(nvs_arg);
   }
   
   /* Store the NVS data to the non volatile memory */
   void SaveNVS()
   {
      nvs_handle nvs_arg;
      nvs_open("Setting", NVS_READWRITE, &nvs_arg);
      nvs_set_u16(nvs_arg, "nvsCounter", nvsCounter);
      nvs_commit(nvs_arg);
      nvs_close(nvs_arg);
   }
};
