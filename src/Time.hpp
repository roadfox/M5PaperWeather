/**
  * @file Time.h
  * 
  * Helper function to set the internal RTC date and time.
  */
#pragma once
#include "Data.hpp"

/* Set the internal RTC clock with the weather timestamp */
bool SetRTCDateTime(MyData &myData)
{
   time_t time = myData.weather.currentTime;

   if (time > 0) {
      rtc_time_t RTCtime;
      rtc_date_t RTCDate;
   
      Serial.println("Epochtime: " + String(time));
      
      RTCDate.year = year(time);
      RTCDate.mon  = month(time);
      RTCDate.day  = day(time);
      M5.RTC.setDate(&RTCDate);
   
      RTCtime.hour = hour(time);
      RTCtime.min  = minute(time);
      RTCtime.sec  = second(time);
      M5.RTC.setTime(&RTCtime);
      return true;
   } 
   return false;
}
