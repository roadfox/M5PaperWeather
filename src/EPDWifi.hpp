/**
  * @file EPDWifi.h
  * 
  * Helper functions for the wifi connection.
  */
#pragma once
#include <WiFi.h>

/* Start and connect to the wifi */
bool StartWiFi(int &rssi) 
{
   IPAddress dns(8, 8, 8, 8); // Google DNS
   
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   WiFi.setAutoConnect(true);
   WiFi.setAutoReconnect(true);

   Serial.print("Connecting to ");
   Serial.println(WIFI_SSID);
   delay(100);
   
   WiFi.begin(WIFI_SSID, WIFI_PW);

   for (int retry = 0; WiFi.status() != WL_CONNECTED && retry < 30; retry++) {
      delay(500);
      Serial.print(".");
   }

   rssi = 0;
   if (WiFi.status() == WL_CONNECTED) {
      rssi = WiFi.RSSI();
      Serial.println("WiFi connected at: " + WiFi.localIP().toString());
      return true;
   } else {
      Serial.println("WiFi connection *** FAILED ***");
      return false;
   }
}

/* Stop the wifi connection */
void StopWiFi() 
{
   Serial.println("Stop WiFi");
   WiFi.disconnect();
   WiFi.mode(WIFI_OFF);
}
