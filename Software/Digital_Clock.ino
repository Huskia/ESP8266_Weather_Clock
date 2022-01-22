/*
 Name:		Digital_Clock.ino
 Created:	2020/8/18 10:55:46
 Author:	Huskie
*/
#include <WiFiManager.h>
#include <Ticker.h>
#include "Pages.h"

WiFiClient espClient;
Ticker Sys_Tick;

/*----------------------------------------------The callback function to give system tick--------------------------------------------*/
bool timestamp = 0;
void Tick_Callback() {
	static unsigned int counter = 0;
	counter++;
	if (counter > 99) {
		counter = 0;
		timestamp = 1;
	}
}

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Serial.print("\n");
	ST75256_Init();
	LCD_CLS();
	analogWrite(LCD_Light, 1023);
	DispStr0816(0, 0, "Power on!");
	Disp_BMP(444, 0, 459, 1, Server_NC, 0);
	Disp_BMP(464, 0, 479, 1, WIFI_ICON_0, 0);
	Disp_BMP(424, 0, 439, 1, NTP_Wait, 0);

	DispStr0816(0, 2, "Connecting WiFi......");
	String Chip_ID = "ID:ESP" + String(ESP.getChipId());
	DispStr0816(0, 10, (char*)Chip_ID.c_str());
	WiFiManager wifiManager;
	wifiManager.setDebugOutput(true);
	if (!wifiManager.autoConnect())
	{
		Serial.println(F("Failed! Reseting......"));
		DispStr0816(0, 4, "Failed! Reseting......");
		//Serial.printf("W0\n");
		delay(3000);
		//If failed, reset module
		ESP.reset();
		delay(5000);
	}
	DispStr0816(0, 4, "WiFi connected!");
	NTP_Start();
	Wire.begin(4, 5);
	if (R8025_Init()) {
		DispStr0816(0, 6, "RTC Power Error! Calibrating via NTP...");
		RTC_Update();
	}
	else {
		DispStr0816(0, 6, "RTC Power OK!");
	}

	delay(2000);
	LCD_CLS();
	Disp_BMP(444, 0, 459, 1, Server_NC, 0);
	Disp_BMP(464, 0, 479, 1, WIFI_ICON_4, 0);
	Disp_BMP(424, 0, 439, 1, NTP_OK, 0);

	Sys_Tick.attach_ms(1, Tick_Callback);

	Weather_Init();

	SHT20_Reset();

	Disp_weather();
}

// the loop function runs over and over again until power down or reset
void loop() {
	static char keepconnect = 1;
	static char counter_report = 0;

	if (timestamp) {
		timestamp = 0;

		counter_report++;
		if (counter_report > 30) {
			counter_report = 0;
			int RSSI = WiFi.RSSI();
			if (RSSI <= -88)
				Disp_BMP(464, 0, 479, 1, (unsigned char*)WIFI_ICON_1, 0);
			else if (RSSI > -88 || RSSI <= -77)
				Disp_BMP(464, 0, 479, 1, (unsigned char*)WIFI_ICON_2, 0);
			else if (RSSI < -77 || RSSI <= -55)
				Disp_BMP(464, 0, 479, 1, (unsigned char*)WIFI_ICON_3, 0);
			else if (RSSI > -55)
				Disp_BMP(464, 0, 479, 1, (unsigned char*)WIFI_ICON_4, 0);
		}

		Home();

	}
}
