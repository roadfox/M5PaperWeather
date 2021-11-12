/**
  * @file Battery.h
  * 
  * Helperfunctions for reading the battery value.
  */
#pragma once

#include "Data.hpp"

/**
  * Read the battery voltage
  */
bool GetBatteryValues(MyData &myData)
{
   uint32_t vol = M5.getBatteryVoltage();

   if (vol < 3300) {
      vol = 3300;
   } else if (vol > 4350) {
      vol = 4350;
   }
  
   float battery = (float)(vol - 3300) / (float)(4350 - 3300);

   myData.batteryVolt = vol / 1000.0f;
   Serial.println("batteryVolt: " + String(myData.batteryVolt));
   
   if (battery <= 0.01) {
      battery = 0.01;
   }
   if (battery > 1) {
      battery = 1;
   }
   myData.batteryCapacity = (int) (battery * 100);
   Serial.println("batteryCapacity: " + String(myData.batteryCapacity));
   
   return true;
}
