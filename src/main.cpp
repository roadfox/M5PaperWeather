/**
  * @file Weather.h
  * 
  * Main file with setup() and loop()
  */
  
#include <M5EPD.h>
#include "Config.hpp"
#include "Data.hpp"
#include "Display.hpp"
#include "Battery.hpp"
#include "EPD.hpp"
#include "EPDWifi.hpp"
#include "SHT30.hpp"
#include "Time.hpp"
#include "Utils.hpp"
#include "Weather.hpp"

// Refresh the M5Paper info more often.
// #define REFRESH_PARTLY 1

MyData         myData;            // The collection of the global data
WeatherDisplay myDisplay(myData); // The global display helper class

/* Start and M5Paper instance */
void setup()
{
#ifndef REFRESH_PARTLY
   InitEPD(true);
   if (StartWiFi(myData.wifiRSSI)) {
      GetBatteryValues(myData);
      GetSHT30Values(myData);
      if (myData.weather.Get()) {
         SetRTCDateTime(myData);
      }
      myData.Dump();
      myDisplay.Show();
      StopWiFi();
   }
   ShutdownEPD(60 * 60); // every 1 hour
   //SleepEPD(3600);  // every 60 min
#else 
   myData.LoadNVS();
   if (myData.nvsCounter == 1) {
      InitEPD(true);
      if (StartWiFi(myData.wifiRSSI)) {
         GetBatteryValues(myData);
         GetSHT30Values(myData);
         if (myData.weather.Get()) {
            SetRTCDateTime(myData);
         }
         myData.Dump();
         myDisplay.Show();
         StopWiFi();
      }
   } else {
      InitEPD(false);
      GetSHT30Values(myData);
      myDisplay.ShowM5PaperInfo();
      if (myData.nvsCounter >= 60) {
         myData.nvsCounter = 0;
      }
   }
   myData.nvsCounter++;
   myData.SaveNVS();
   ShutdownEPD(60); // 1 minute
#endif // REFRESH_PARTLY   
}

/* Main loop. Never reached because of shutdown */
void loop()
{
}
