/**
  * @file Display.h
  * 
  * Main class for drawing the content to the e-paper display.
  */
#pragma once
#include "Data.hpp"
#include "Icons.hpp"


M5EPD_Canvas canvas(&M5.EPD); // Main canvas of the e-paper

/* Main class for drawing the content to the e-paper display. */
class WeatherDisplay
{
protected:
   MyData &myData; //!< Reference to the global data
   int     maxX;   //!< Max width of the e-paper
   int     maxY;   //!< Max height of the e-paper

protected:
   void DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom = 0, int32_t degTo = 360);
   void Arrow(int x, int y, int asize, float aangle, int pwidth, int plength);
   void DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int radius);    
   
   void DrawIcon(int x, int y, const uint16_t *icon, int dx = 64, int dy = 64, bool highContrast = false);
   
   void DrawHead();
   void DrawRSSI(int x, int y);
   void DrawBattery(int x, int y);

   void DrawWeatherInfo(int x, int y, int dx, int dy);
   void DrawSunInfo(int x, int y, int dx, int dy);
   void DrawWindInfo(int x, int y, int dx, int dy);
   void DrawM5PaperInfo(int x, int y, int dx, int dy);

   void DrawDaily(int x, int y, int dx, int dy, Weather &weather, int index);
   
   void DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], float values2[]);
   void DrawDualGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], int offsetB, int yMinB, int yMaxB, float valuesB[]);

public:
   WeatherDisplay(MyData &md, int x = 960, int y = 540)
      : myData(md)
      , maxX(x)
      , maxY(y)
   {
   }

   void Show();

   void ShowM5PaperInfo();
};

/* Draw a circle with optional start and end point */
void WeatherDisplay::DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom /* = 0 */, int32_t degTo /* = 360 */)
{
   for (int i = degFrom; i < degTo; i++) {
      double radians = i * PI / 180;
      double px      = x + r * cos(radians);
      double py      = y + r * sin(radians);
      
      canvas.drawPixel(px, py, color);
   }
} 

/* Draw a the rssi value as circle parts */
void WeatherDisplay::DrawRSSI(int x, int y)
{
   int iQuality = WifiGetRssiAsQualityInt(myData.wifiRSSI);

   if (iQuality >= 80) DrawCircle(x + 12, y, 16, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 40) DrawCircle(x + 12, y, 12, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 20) DrawCircle(x + 12, y,  8, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 10) DrawCircle(x + 12, y,  4, M5EPD_Canvas::G15, 225, 315); 
   DrawCircle(x + 12, y, 2, M5EPD_Canvas::G15, 225, 315); 
}

/* Draw a the battery icon */
void WeatherDisplay::DrawBattery(int x, int y)
{
   canvas.drawRect(x, y, 40, 16, M5EPD_Canvas::G15);
   canvas.drawRect(x + 40, y + 3, 4, 10, M5EPD_Canvas::G15);
   for (int i = x; i < x + 40; i++) {
      canvas.drawLine(i, y, i, y + 15, M5EPD_Canvas::G15);
      if ((i - x) * 100.0 / 40.0 > myData.batteryCapacity) {
         break;
      }
   }
}

/* Draw a the head with version, city, rssi and battery */
void WeatherDisplay::DrawHead()
{
   canvas.drawString(VERSION, 20, 10);
   canvas.drawCentreString(CITY_NAME, maxX / 2, 10, 1);
   canvas.drawString(WifiGetRssiAsQuality(myData.wifiRSSI) + "%", maxX - 200, 10);
   DrawRSSI(maxX - 155, 25);
   canvas.drawString(String(myData.batteryCapacity) + "%", maxX - 110, 10);
   DrawBattery(maxX - 65, 10);
}

/* Draw one icon from the binary data */
void WeatherDisplay::DrawIcon(int x, int y, const uint16_t *icon, int dx /*= 64*/, int dy /*= 64*/, bool highContrast /*= false*/)
{
   for (int yi = 0; yi < dy; yi++) {
      for (int xi = 0; xi < dx; xi++) {
         uint16_t pixel = icon[yi * dx + xi];

         if (highContrast) {
            if (15 - (pixel / 4096) > 0) canvas.drawPixel(x + xi, y + yi, M5EPD_Canvas::G15);
         } else {
            canvas.drawPixel(x + xi, y + yi, 15 - (pixel / 4096));
         }
      }
   }
}

/* Draw the sun information with sunrise and sunset */
void WeatherDisplay::DrawSunInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(4);
   canvas.drawCentreString("Sun", x + dx / 2, y + 9, 1);
   canvas.drawLine(x, y + 42, x + dx, y + 42, M5EPD_Canvas::G15);

   canvas.setTextSize(4);
   DrawIcon(x + dx / 2 - 32, y + 55, (uint16_t *) SUNRISE64x64);
   canvas.drawCentreString(getHourMinString(myData.weather.sunrise), x + dx / 2, y + 130, 1);
   
   DrawIcon(x + dx / 2 - 32, y + 170, (uint16_t *) SUNSET64x64);
   canvas.drawCentreString(getHourMinString(myData.weather.sunset),  x + dx / 2, y + 245, 1);
}

/* Draw current weather information */
void WeatherDisplay::DrawWeatherInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(4);
   canvas.drawCentreString("Weather", x + dx / 2, y + 9, 1);
   canvas.drawLine(x, y + 42, x + dx, y + 42, M5EPD_Canvas::G15);

   String icon = myData.weather.hourlyIcon[0];
   int iconX = x + dx / 2 - 32;
   int iconY = y + 50;

        if (icon == "01d") DrawIcon(iconX, iconY, (uint16_t *) image_data_01d, 64, 64, true);
   else if (icon == "01n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "02d") DrawIcon(iconX, iconY, (uint16_t *) image_data_02d, 64, 64, true);
   else if (icon == "02n") DrawIcon(iconX, iconY, (uint16_t *) image_data_02n, 64, 64, true);
   else if (icon == "03d") DrawIcon(iconX, iconY, (uint16_t *) image_data_03d, 64, 64, true);
   else if (icon == "03n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "04d") DrawIcon(iconX, iconY, (uint16_t *) image_data_04d, 64, 64, true);
   else if (icon == "04n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "09d") DrawIcon(iconX, iconY, (uint16_t *) image_data_09d, 64, 64, true);
   else if (icon == "09n") DrawIcon(iconX, iconY, (uint16_t *) image_data_09n, 64, 64, true);
   else if (icon == "10d") DrawIcon(iconX, iconY, (uint16_t *) image_data_10d, 64, 64, true);
   else if (icon == "10n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "11d") DrawIcon(iconX, iconY, (uint16_t *) image_data_11d, 64, 64, true);
   else if (icon == "11n") DrawIcon(iconX, iconY, (uint16_t *) image_data_11n, 64, 64, true);
   else if (icon == "13d") DrawIcon(iconX, iconY, (uint16_t *) image_data_13d, 64, 64, true);
   else if (icon == "13n") DrawIcon(iconX, iconY, (uint16_t *) image_data_13n, 64, 64, true);
   else if (icon == "50d") DrawIcon(iconX, iconY, (uint16_t *) image_data_50d, 64, 64, true);
   else if (icon == "50n") DrawIcon(iconX, iconY, (uint16_t *) image_data_50n, 64, 64, true);
   else DrawIcon(iconX, iconY, (uint16_t *) image_data_unknown, 64, 64, true);

   canvas.drawCentreString(myData.weather.hourlyMain[0], x + dx / 2, y + 115, 1);

   // temp
   canvas.setTextSize(7);
   char buff[8];
   sprintf(buff,"%.0f",myData.weather.hourlyMaxTemp[0]);
   canvas.drawRightString(buff, x + dx / 2 + 20, y + 170, 1);
   canvas.setTextSize(4);
   canvas.drawString(      "C", x + dx / 2 + 20, y + 170, 1);
   // rain
   canvas.setTextSize(4);
   canvas.drawCentreString(getFloatString(myData.weather.hourlyRain[0], "mm"),  x + dx / 2, y + 240, 1);
}

/* Draw the in the wind section
 * The wind section drawing was from the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 * 
 * @param x position of the center the arrow rotates arround
 * @param y position of the center the arrow rotates arround
 * @param asize displacement radius from the center
 * @param aangle clockwise rotation of the arrow
 * @param pwidth width of the arrow base
 * @param plength length of the arrow
 * 
 */
void WeatherDisplay::Arrow(int x, int y, int asize, float aangle, int pwidth, int plength) 
{
   float dx = (asize + 21) * cos((aangle - 90) * PI / 180) + x; // calculate X position
   float dy = (asize + 21) * sin((aangle - 90) * PI / 180) + y; // calculate Y position
   float x1 = 0;           float y1 = plength;
   float x2 = pwidth / 2;  float y2 = pwidth / 2;
   float x3 = -pwidth / 2; float y3 = pwidth / 2;
   float angle = aangle * PI / 180;
   float xx1 = x1 * cos(angle) - y1 * sin(angle) + dx;
   float yy1 = y1 * cos(angle) + x1 * sin(angle) + dy;
   float xx2 = x2 * cos(angle) - y2 * sin(angle) + dx;
   float yy2 = y2 * cos(angle) + x2 * sin(angle) + dy;
   float xx3 = x3 * cos(angle) - y3 * sin(angle) + dx;
   float yy3 = y3 * cos(angle) + x3 * sin(angle) + dy;
   canvas.fillTriangle(xx1, yy1, xx3, yy3, xx2, yy2, M5EPD_Canvas::G15);
}

/* Draw the wind circle with the windspeed data
 * The wind section drawing was from the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 */
void WeatherDisplay::DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int cradius) 
{
   int dxo, dyo, dxi, dyi;

   canvas.setTextSize(3);
   canvas.drawLine(0, 15, 0, y + cradius + 30, M5EPD_Canvas::G15);
   canvas.drawCircle(x, y, cradius, M5EPD_Canvas::G15);     // Draw compass circle
   canvas.drawCircle(x, y, cradius + 1, M5EPD_Canvas::G15); // Draw compass circle
   canvas.drawCircle(x, y, cradius * 0.7, M5EPD_Canvas::G15); // Draw compass inner circle
   for (float a = 0; a < 360; a = a + 22.5) {
      dxo = cradius * cos((a - 90) * PI / 180);
      dyo = cradius * sin((a - 90) * PI / 180);
      if (a == 45)  canvas.drawCentreString("NE", dxo + x + 15, dyo + y - 15, 1);
      if (a == 135) canvas.drawCentreString("SE", dxo + x + 15, dyo + y  + 5, 1);
      if (a == 225) canvas.drawCentreString("SW", dxo + x - 15, dyo + y  + 5, 1);
      if (a == 315) canvas.drawCentreString("NW", dxo + x - 15, dyo + y - 15, 1);
      dxi = dxo * 0.9;
      dyi = dyo * 0.9;
      canvas.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, M5EPD_Canvas::G15);
      dxo = dxo * 0.7;
      dyo = dyo * 0.7;
      dxi = dxo * 0.9;
      dyi = dyo * 0.9;
      canvas.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, M5EPD_Canvas::G15);
   }
   canvas.drawCentreString("N", x, y - cradius - 20, 1);
   canvas.drawCentreString("S", x, y + cradius + 5, 1);
   canvas.drawCentreString("W", x - cradius - 15, y - 3, 1);
   canvas.drawCentreString("E", x + cradius + 15,  y - 3, 1);
   canvas.setTextSize(4);
   canvas.drawCentreString(String(windspeed, 1), x, y - 30, 1);
   canvas.setTextSize(3);
   canvas.drawCentreString("m/s", x, y + 10, 1);

   Arrow(x, y, cradius - 10, angle, 23, 55);
}

/* Draw the wind information part */
void WeatherDisplay::DrawWindInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(4);
   canvas.drawCentreString("Wind", x + dx / 2, y + 9, 1);
   canvas.drawLine(x, y + 42, x + dx, y + 42, M5EPD_Canvas::G15);

   DisplayDisplayWindSection(x + dx / 2, y + dy / 2 + 20, myData.weather.winddir, myData.weather.windspeed, 95);
}

/* Draw the M5Paper environment and RTC information */
void WeatherDisplay::DrawM5PaperInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(4);
   canvas.drawCentreString("Indoor", x + dx / 2, y + 9, 1);
   canvas.drawLine(x, y + 42, x + dx, y + 42, M5EPD_Canvas::G15);

   canvas.setTextSize(4);
   canvas.drawCentreString(getRTCDateString(), x + dx / 2, y + 55, 1);
   canvas.drawCentreString(getRTCTimeString(), x + dx / 2, y + 95, 1);
   canvas.setTextSize(3);
   canvas.drawCentreString("updated", x + dx / 2, y + 130, 1);

   DrawIcon(x + dx / 4 - 32, y + 170, (uint16_t *) TEMPERATURE64x64);
   canvas.setTextSize(7);
   canvas.drawRightString(String(myData.sht30Temperatur - 2), x + dx / 4 + 30, y + 240, 1);
   canvas.setTextSize(4);
   canvas.drawString("C", x + dx / 4 + 30, y + 240, 1);

   DrawIcon(x + dx / 4 * 3 - 40, y + 170, (uint16_t *) HUMIDITY64x64);
   canvas.setTextSize(7);
   canvas.drawRightString(String(myData.sht30Humidity), x + dx / 4 * 3 + 20, y + 240, 1);
   canvas.setTextSize(4);
   canvas.drawString("%", x + dx / 4 * 3 + 20, y + 240, 1);
   
}

/* Draw one daily weather information */
void WeatherDisplay::DrawDaily(int x, int y, int dx, int dy, Weather &weather, int index)
{
   time_t time = weather.forecastTime[index];
   int    tMin = weather.forecastMinTemp[index];
   int    tMax = weather.forecastMaxTemp[index];
   int    pop  = weather.forecastPop[index];
   String icon = weather.forecastIcon[index];
   
   canvas.setTextSize(3);
   canvas.drawCentreString(index == 0 ? "Today" : getShortDayOfWeekString(time), x + dx / 2, y + 5, 1);

   int iconX = x + dx / 2 - 32;
   int iconY = y + 33;
   
        if (icon == "01d") DrawIcon(iconX, iconY, (uint16_t *) image_data_01d, 64, 64, true);
   else if (icon == "01n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "02d") DrawIcon(iconX, iconY, (uint16_t *) image_data_02d, 64, 64, true);
   else if (icon == "02n") DrawIcon(iconX, iconY, (uint16_t *) image_data_02n, 64, 64, true);
   else if (icon == "03d") DrawIcon(iconX, iconY, (uint16_t *) image_data_03d, 64, 64, true);
   else if (icon == "03n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "04d") DrawIcon(iconX, iconY, (uint16_t *) image_data_04d, 64, 64, true);
   else if (icon == "04n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "09d") DrawIcon(iconX, iconY, (uint16_t *) image_data_09d, 64, 64, true);
   else if (icon == "09n") DrawIcon(iconX, iconY, (uint16_t *) image_data_09n, 64, 64, true);
   else if (icon == "10d") DrawIcon(iconX, iconY, (uint16_t *) image_data_10d, 64, 64, true);
   else if (icon == "10n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "11d") DrawIcon(iconX, iconY, (uint16_t *) image_data_11d, 64, 64, true);
   else if (icon == "11n") DrawIcon(iconX, iconY, (uint16_t *) image_data_11n, 64, 64, true);
   else if (icon == "13d") DrawIcon(iconX, iconY, (uint16_t *) image_data_13d, 64, 64, true);
   else if (icon == "13n") DrawIcon(iconX, iconY, (uint16_t *) image_data_13n, 64, 64, true);
   else if (icon == "50d") DrawIcon(iconX, iconY, (uint16_t *) image_data_50d, 64, 64, true);
   else if (icon == "50n") DrawIcon(iconX, iconY, (uint16_t *) image_data_50n, 64, 64, true);
   else DrawIcon(iconX, iconY, (uint16_t *) image_data_unknown, 64, 64, true);

   canvas.drawCentreString(String(tMin)+"/"+String(tMax), x + dx / 2, y + 100, 1);
   canvas.drawCentreString(String(pop)+"%", x + dx / 2, y + 135, 1);
}

/* Draw a graph with x- and y-axis and values */
void WeatherDisplay::DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], float values2[])
{
   String yMinString = String(yMin);
   String yMaxString = String(yMax);
   int    textWidth  = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int    graphX     = x + 5 + textWidth + 5;
   int    graphY     = y + 35;
   int    graphDX    = dx - textWidth - 20;
   int    graphDY    = dy - 35 - 20;
   float  xStep      = graphDX / xMax;
   float  yStep      = graphDY / (yMax - yMin);
   int    iOldX      = 0;
   int    iOldY      = 0;

   canvas.setTextSize(3);
   canvas.drawCentreString(title, x + dx / 2, y + 10, 1);
   canvas.setTextSize(2);
   canvas.drawString(yMaxString, x + 5, graphY - 5);   
   canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);   
   for (int i = 0; i <= xMax; i++) {
      canvas.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);   
   }
   
   canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);   
   if (yMin < 0 && yMax > 0) { // null line?
      float yValueDX = (float) graphDY / (yMax - yMin);
      int   yPos     = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);   
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);         
      }
   }
   for (int i = xMin; i <= xMax; i++) {
      float yValue   = values[i];
      float yValueDY = (float) graphDY / (yMax - yMin);
      int   xPos     = graphX + graphDX / xMax * i;
      int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G15);
      if (i > xMin) {
         canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);         
      }
      iOldX = xPos;
      iOldY = yPos;
   }
   if (values2 != NULL) {
      for (int i = xMin; i <= xMax; i++) {
         float yValue   = values2[i];
         float yValueDY = (float) graphDY / (yMax - yMin);
         int   xPos     = graphX + graphDX / xMax * i;
         int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

         if (yPos > graphY + graphDY) yPos = graphY + graphDY;
         if (yPos < graphY)           yPos = graphY;

         if (i > xMin) {
            canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);         
            canvas.drawLine(iOldX, iOldY+1, xPos, yPos+1, M5EPD_Canvas::G0);         
         }
         canvas.fillCircle(xPos, yPos, 3, M5EPD_Canvas::G15);
         canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G0);
         iOldX = xPos;
         iOldY = yPos;
      }
   }
}

/* Draw a dual graph */
void WeatherDisplay::DrawDualGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[], int offset, int yMinB, int yMaxB, float valuesB[])
{
   String yMinString = String(yMinB);
   String yMaxString = String(yMaxB);
   int    textWidth  = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int    graphX     = x + 5 + textWidth + 5;
   int    graphY     = y + 35;
   int    graphDX    = dx - textWidth - 20;
   int    graphDY    = dy - 35 - 20;
   float  xStep      = graphDX / xMax;
   float  yStep      = graphDY / (yMax - yMin);
   int    iOldX      = 0;
   int    iOldY      = 0;

   canvas.setTextSize(2);
   canvas.drawCentreString(title, x + dx / 2, y + 10, 1);
   canvas.setTextSize(2);
   canvas.drawString(yMaxString, x + 5, graphY - 5);   
   canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);   
   for (int i = 0; i <= xMax; i++) {
      canvas.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);   
   }
   
   canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);   
   if (yMin < 0 && yMax > 0) { // null line?
      float yValueDX = (float) graphDY / (yMax - yMin);
      int   yPos     = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);   
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);         
      }
   }
   for (int i = xMin; i < xMax; i++) {
      float yValue   = valuesB[i - xMin];
      float yValueDY = (float) graphDY / (float)(yMaxB - yMinB);
      int   xPos     = graphX + graphDX / (xMax - xMin) * i;
      int   yPos     = graphY + graphDY - ((yValue - (float)yMinB) * yValueDY);

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      int width = graphDX / (xMax - xMin);
      int height = (graphY + graphDY) - yPos;
      if (height > 0) {
         canvas.fillRect(xPos, yPos, width, height, M5EPD_Canvas::G15);
      }
   }
   for (int i = xMin + offset; i <= xMax; i++) {
      float yValue   = values[i];
      float yValueDY = (float) graphDY / (yMax - yMin);
      int   xPos     = graphX + graphDX / xMax * i;
      int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      if (i > xMin + offset) {
         canvas.drawLine(iOldX, iOldY,   xPos, yPos,   M5EPD_Canvas::G15);
         canvas.drawLine(iOldX, iOldY+1, xPos, yPos+1, M5EPD_Canvas::G0);
      }
      canvas.fillCircle(xPos, yPos, 3, M5EPD_Canvas::G0);
      canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G15);
      iOldX = xPos;
      iOldY = yPos;
   }
}

/* Main function to show all the data to the e-paper */
void WeatherDisplay::Show()
{
   Serial.println("WeatherDisplay::Show");

   canvas.createCanvas(960, 540);

   canvas.setTextSize(3);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   DrawHead();

   // x = 960 y = 540
   // 540 - oben 35 - unten 10 = 495
   
   // big row with current info
   int current_box_top = 35;
   int current_box_height = 320;
   int xPos0 = 15;
   int xPos1 = 232 + 15;
   int xPos2 = 2 * 232 - 35;
   int xPos3 = 3 * 232 - 5;

   int row1width = xPos1 - xPos0;
   int row2width = xPos2 - xPos1;
   int row3width = xPos3 - xPos2;
   int row4width =  maxX - xPos3;

   
   DrawWeatherInfo(xPos0, current_box_top, row1width, current_box_height);
   DrawSunInfo    (xPos1, current_box_top, row2width, current_box_height);
   DrawWindInfo   (xPos2, current_box_top, row3width, current_box_height);
   DrawM5PaperInfo(xPos3, current_box_top, row4width, current_box_height);
   // current info border
   canvas.drawRect(xPos0, current_box_top,             maxX - 30, current_box_height+35, M5EPD_Canvas::G15);
   canvas.drawLine(xPos1, current_box_top, xPos1, current_box_height+35, M5EPD_Canvas::G15);
   canvas.drawLine(xPos2, current_box_top, xPos2, current_box_height+35, M5EPD_Canvas::G15);
   canvas.drawLine(xPos3, current_box_top, xPos3, current_box_height+35, M5EPD_Canvas::G15);


   // draw daily weather forcasts
   int daily_box_height = 175;
   int daily_box_width = 135;
   int daily_box_top = maxY - daily_box_height;
   int daily_box_bottom = daily_box_top + daily_box_height;
   
   canvas.drawRect(15, daily_box_top, maxX - 30, daily_box_height, M5EPD_Canvas::G15);
   for (int x = 15, i = 0; i <= 4; x += daily_box_width, i++) {
      DrawDaily(x, daily_box_top, daily_box_width, daily_box_height, myData.weather, i);
      canvas.drawLine(x + daily_box_width, daily_box_top, x + daily_box_width, daily_box_bottom, M5EPD_Canvas::G15);
   }
   DrawDualGraph(713, daily_box_top, 232, daily_box_height, "Rain 7days (mm/%)", 0,  7,   0,  100, myData.weather.forecastPop, 0, 0, myData.weather.forecastMaxRain, myData.weather.forecastRain);

// some graphs disabled to gain screen space, leaving here for reference
//   canvas.drawRect(15, 408, maxX - 30, 122, M5EPD_Canvas::G15);
//   DrawGraph( 15, 408, 232, 122, "Temp 12h (C)", 0, 12, myData.weather.hourlyTempRange[0], myData.weather.hourlyTempRange[1], myData.weather.hourlyMaxTemp, NULL);
//   DrawDualGraph(247, 408, 232, 122, "Rain 12h (mm/%)", 0, 12,   0,  100, myData.weather.hourlyPop, 1, 0, myData.weather.hourlyMaxRain, myData.weather.hourlyRain);
//   canvas.drawLine(480, 408, 480, 530, M5EPD_Canvas::G15);
//   DrawGraph(481, 408, 232, 122, "Temp 7days (C)", 0,  7, myData.weather.forecastTempRange[0], myData.weather.forecastTempRange[1], myData.weather.forecastMinTemp, myData.weather.forecastMaxTemp);

   // outer border
   canvas.drawRect( 14, 34, maxX - 28, maxY - 43, M5EPD_Canvas::G15);

   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
   delay(1000);
}

/* Update only the M5Paper part of the global data */
void WeatherDisplay::ShowM5PaperInfo()
{
   Serial.println("WeatherDisplay::ShowM5PaperInfo");

   canvas.createCanvas(245, 251);

   canvas.setTextSize(3);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   canvas.drawRect(0, 0, 245, 251, M5EPD_Canvas::G15);
   DrawM5PaperInfo(0, 0, 245, 251);
   
   canvas.pushCanvas(697, 35, UPDATE_MODE_GC16);
   delay(1000);
}
