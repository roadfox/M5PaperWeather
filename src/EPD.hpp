/**
  * @file EPD.h
  * 
  * Helper functions for initialisizing and shutdown of the M5Paper.
  */
#pragma once

/* Initialize the M5Paper */
void InitEPD(bool clearDisplay = true)
{
   M5.begin(false, false, true, true, false);
   M5.RTC.begin();
   
   M5.EPD.SetRotation(0);
   M5.TP.SetRotation(0);
   if (clearDisplay) {
      M5.EPD.Clear(true);
   }

//   disableCore0WDT();
}

/* 
 *  Shutdown the M5Paper 
 *  NOTE: the M5Paper could not shutdown while on usb connection.
 *        In this case use the esp_deep_sleep_start() function.
*/
void ShutdownEPD(int sec)
{
   Serial.println("Shutdown");
/*
   M5.disableEPDPower();
   M5.disableEXTPower();
   M5.disableMainPower();
   esp_sleep_enable_timer_wakeup(sec * 1000000);
   esp_deep_sleep_start();   
*/   

   M5.shutdown(sec);
}

void SleepEPD(int sec)
{
   Serial.println("Going to Sleep");

      // M5Paper deep sleep with touch wakeup
      M5.disableEPDPower();
      M5.disableEXTPower();
      //M5.disableMainPower();
      //esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, LOW); // TOUCH_INT
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_38, LOW); // Button
      esp_sleep_enable_timer_wakeup(1800*1000000);  // wakeup every x sec
      gpio_hold_en(GPIO_NUM_2); // M5EPD_MAIN_PWR_PIN
      gpio_deep_sleep_hold_en();
      esp_deep_sleep_start();

      // M5Paper light sleep with touch wakeup
      //esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, LOW); // TOUCH_INT
      //esp_light_sleep_start();
}
