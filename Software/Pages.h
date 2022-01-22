#ifndef _PAGES_H_
#define _PAGES_H_

#include "ST75256.h"
#include "R8025.h"
#include "SHT20.h"
#include "WeatherNow.h"
#include "WeatherForecast.h"
#include "AirQuality.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

void NTP_Start(void);
unsigned char Leap_Year_Judge(unsigned short year);
unsigned char last_day_of_mon(unsigned char month, unsigned short year);
bool RTC_Update(void);

void Home(void);

void Weather_Init(void);
void Disp_weather_icon(unsigned int x, unsigned char y, unsigned int icon_num);
void Disp_weather(void);

#endif
