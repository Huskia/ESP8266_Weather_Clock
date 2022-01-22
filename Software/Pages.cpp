#include "Pages.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);//东八区

String UserKey = "00000000000000000000000000000000";   // 私钥
String Location = "000000000"; // 城市代码
String Unit = "m";             // 公制-m/英制-i
String Lang = "zh";            // 语言 英文-en/中文-zh

WeatherNow weatherNow;
WeatherForecast weatherforecast;
AirQuality airquality;

void NTP_Start(void) {
	timeClient.begin(); //Start NTP Client
}

unsigned char Leap_Year_Judge(unsigned short year) {

	if ((year % 400) == 0) {
		return 1;
	}
	else if ((year % 100) == 0) {
		return 0;
	}
	else if ((year % 4) == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

unsigned char last_day_of_mon(unsigned char month, unsigned short year) {
	const unsigned char day_per_mon[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if ((month == 0) || (month > 12)) {
		return day_per_mon[1] + Leap_Year_Judge(year);
	}

	if (month != 2) {
		return day_per_mon[month - 1];
	}
	else {
		return day_per_mon[1] + Leap_Year_Judge(year);
	}
}

void RTC_Sync(void) {
	unsigned long Unix_Time;
	unsigned int year;
	unsigned char month, day, week, hour, min, sec;
	int year_tmp, month_tmp, day_tmp;
	unsigned char RTC_Buff[7];

	Unix_Time = timeClient.getEpochTime();

	int days = Unix_Time / 86400L;
	for (year_tmp = 1970; days > 0; year_tmp++) {
		day_tmp = (365 + Leap_Year_Judge(year_tmp)); //这一年有多少天？
		if (days >= day_tmp) {//条件成立，则 yearTmp 即是这个时间戳值所代表的年数。
			days -= day_tmp;
		}
		else {
			break;
		}
	}
	year = year_tmp - 2000;

	for (month_tmp = 1; month_tmp < 12; month_tmp++) {
		day_tmp = last_day_of_mon(month_tmp, year);
		if (days >= day_tmp) {
			days -= day_tmp;
		}
		else {
			break;
		}
	}
	month = month_tmp;

	day = days + 1;

	week = ((Unix_Time / 86400L) + 4) % 7; //0 is Sunday

	hour = (Unix_Time % 86400L) / 3600;

	min = (Unix_Time % 3600) / 60;

	Unix_Time = timeClient.getEpochTime(); //Reflash time vaule

	sec = Unix_Time % 60;

	RTC_Buff[0] = ((sec / 10) << 4) | sec % 10; //Change data format to BCD
	RTC_Buff[1] = ((min / 10) << 4) | min % 10;
	RTC_Buff[2] = ((hour / 10) << 4) | hour % 10;
	if (week == 0)
		RTC_Buff[3] = 0x40;
	else
		RTC_Buff[3] = 0x01 << (week - 1);
	RTC_Buff[4] = ((day / 10) << 4) | day % 10;
	RTC_Buff[5] = ((month / 10) << 4) | month % 10;
	RTC_Buff[6] = ((year / 10) << 4) | year % 10;
	Write_R8025(RTC_Buff); //Write time data to RTC

}

bool RTC_Update(void) {
	if (timeClient.update()) {
		RTC_Sync();
		Disp_BMP(424, 0, 439, 1, (unsigned char*)NTP_OK, 0);
		return 1;
	}
	else {
		Disp_BMP(424, 0, 439, 1, (unsigned char*)NTP_Fail, 0);
		return 0;
	}

}

void Home(void) {
	static bool refresh_start = 1, refresh_stop = 0, sync_fail = 0;
	unsigned char Time_Buff[7];
	unsigned int sync_delay = 100;
	Read_R8025(Time_Buff);

	static bool update_start = 0;
	if (Time_Buff[2] == 0x12 || Time_Buff[2] == 0x00) {  //Calibrate RTC twice everyday
		if (update_start) {
			update_start = 0;
			sync_delay = 100;
			Disp_BMP(424, 0, 439, 1, NTP_Wait, 0);
			if (RTC_Update()) {
				sync_fail = 0;
			}
			else {
				sync_fail = 1;
			}
		}
	}
	else {
		update_start = 1;
		sync_fail = 0;
	}

	if (sync_fail) { //NTP同步失败则10秒后自动尝试重新同步
		sync_delay--;
		if (sync_delay == 0) {
			update_start = 1;
		}
	}


	if (Time_Buff[0] == 0)  //Only refresh at 00s
		refresh_start = 1;
	if (Time_Buff[0] == 0x59)
		refresh_stop = 0;
	if (refresh_start && !refresh_stop) {
		refresh_start = 0;
		refresh_stop = 1;
		DispStr0816(0, 0, "20");
		LCD_P8x16Ch(16, 0, (Time_Buff[6] / 16) + 16);
		LCD_P8x16Ch(24, 0, (Time_Buff[6] & 0x0F) + 16);
		DispZHCN(32, 0, "年");
		LCD_P8x16Ch(48, 0, (Time_Buff[5] / 16) + 16);
		LCD_P8x16Ch(56, 0, (Time_Buff[5] & 0x0F) + 16);
		DispZHCN(64, 0, "月");
		LCD_P8x16Ch(80, 0, (Time_Buff[4] / 16) + 16);
		LCD_P8x16Ch(88, 0, (Time_Buff[4] & 0x0F) + 16);
		DispZHCN(96, 0, "日");
		switch (Time_Buff[3]) {
			// case 0x00: DispZHCN(120, 0, "星期日"); break;
		case 0x01: DispZHCN(120, 0, "星期一"); break;
		case 0x02: DispZHCN(120, 0, "星期二"); break;
		case 0x04: DispZHCN(120, 0, "星期三"); break;
		case 0x08: DispZHCN(120, 0, "星期四"); break;
		case 0x10: DispZHCN(120, 0, "星期五"); break;
		case 0x20: DispZHCN(120, 0, "星期六"); break;
		case 0x40: DispZHCN(120, 0, "星期日"); break;
		default: break;
		}
		LCD_P8x16Ch(184, 0, (Time_Buff[2] / 16) + 16);
		LCD_P8x16Ch(192, 0, (Time_Buff[2] & 0x0F) + 16);
		LCD_P8x16Ch(200, 0, 26);
		LCD_P8x16Ch(208, 0, (Time_Buff[1] / 16) + 16);
		LCD_P8x16Ch(216, 0, (Time_Buff[1] & 0x0F) + 16);
		LCD_P8x16Ch(224, 0, 26);
	}
	LCD_P8x16Ch(232, 0, (Time_Buff[0] / 16) + 16);
	LCD_P8x16Ch(240, 0, (Time_Buff[0] & 0x0F) + 16);


	static unsigned char SHT20_Order = 0;
	static int T = 0, RH = 0;
	unsigned int light_adc = 0, hour_dec = (Time_Buff[2] >> 4) * 10 + Time_Buff[2] & 0x0F;
	static unsigned int pwm_value = 0, pwm_value_old = 0;
	SHT20_Order++;
	switch (SHT20_Order) {  //refresh T & RH every 1s
	case 1: Trigger_T(); break;
	case 2: T = Get_T_Result(); break;
	case 3: Trigger_RH(); break;
	case 4: RH = Get_RH_Result(); break;
	case 5:
		LCD_P8x16Ch(268, 0, 97);//Display temperature.
		if (T < 0) {
			T = -T;
			LCD_P8x16Ch(276, 0, 13);
		}
		else {
			LCD_P8x16Ch(276, 0, 0);
		}
		LCD_P8x16Ch(284, 0, T % 1000 / 100 + 16);
		LCD_P8x16Ch(292, 0, T % 100 / 10 + 16);
		LCD_P8x16Ch(300, 0, 14);
		LCD_P8x16Ch(308, 0, T % 10 + 16);
		LCD_P8x16Ch(316, 0, 95);
		LCD_P8x16Ch(332, 0, 98);  //Display RH.
		LCD_P8x16Ch(348, 0, RH % 1000 / 100 + 16);
		LCD_P8x16Ch(356, 0, RH % 100 / 10 + 16);
		LCD_P8x16Ch(364, 0, 14);
		LCD_P8x16Ch(372, 0, RH % 10 + 16);
		LCD_P8x16Ch(380, 0, 5);

		light_adc = analogRead(A0);
		if (light_adc < 40 && (hour_dec > 6 && hour_dec < 22))
			pwm_value = 100;
		else if (light_adc < 40)
			pwm_value = 0;
		else if (light_adc > 400)
			pwm_value = 1023;
		else
			pwm_value = 2.6 * light_adc - 2.2;
		/*Serial.print("ADC:");
		Serial.print(light_adc);
		Serial.print("\n");*/

		break;
	case 10: SHT20_Order = 0; break;
	default: break;
	}

	if (pwm_value > pwm_value_old) {
		pwm_value_old++;
		analogWrite(LCD_Light, pwm_value_old);
	}
	else if (pwm_value < pwm_value_old) {
		pwm_value_old--;
		analogWrite(LCD_Light, pwm_value_old);
	}

	static bool reflash_weather;
	if (Time_Buff[1] == 0x00 || Time_Buff[1] == 0x30) {
		if (reflash_weather) {
			reflash_weather = 0;
			Disp_weather();
		}
	}
	else {
		reflash_weather = 1;
	}

}

void Weather_Init(void) {
	weatherNow.config(UserKey, Location, Unit, Lang); // 配置请求信息
	weatherforecast.config(UserKey, Location, Unit, Lang); // 配置请求信息
	airquality.config(UserKey, Location, Unit, Lang); // 配置请求信息
}

void Disp_weather_icon(unsigned int x, unsigned char y, unsigned int icon_num) {
	unsigned char i;
	unsigned int j = 0;
	unsigned int xx, yy;
	int num_buff;
	unsigned char dat_buff;
	for (i = 0; i < 100; i++) {
		memcpy_P(&num_buff, &ALL_ICON[i].ICON_NUM, sizeof(int));
		if (num_buff == icon_num) {
			for (yy = y; yy <= y + 7; yy++) {
				LCD_Set_Coordinate(x, yy);
				for (xx = x; xx <= x + 63; xx++) {
					if (xx == 240)
						LCD_Set_Coordinate(xx, yy);
					else {
						memcpy_P(&dat_buff, &ALL_ICON[i].ICON_DATA[j++], 1);
						Write_LCD(Data, dat_buff);
					}
				}
			}
			return;
		}
	}

}

void Disp_weather(void) {

	if (weatherNow.get()) { // 获取天气更新

		String last_update = weatherNow.getLastUpdate();
		DispStr0608(340, 11, (char*)last_update.c_str());
		Serial.print(F("Last Update: "));
		Serial.println(last_update);  // 获取服务器更新天气信息时间

		int temp = weatherNow.getTemp();
		if (temp < 0) {
			temp = -temp;
			LCD_P8x16Ch(64, 2, 13);
		}
		else {
			LCD_P8x16Ch(64, 2, 0);
		}
		LCD_P8x16Ch(72, 2, temp / 10 + 16);
		LCD_P8x16Ch(80, 2, temp % 10 + 16);
		LCD_P8x16Ch(88, 2, 95);
		Serial.print(F("Temperature: "));
		Serial.println(temp);                        // 获取实况温度

		int tempf = weatherNow.getFeelLike();
		LCD_P8x16Ch(104, 2, 99);
		if (tempf < 0) {
			tempf = -tempf;
			LCD_P8x16Ch(112, 2, 13);
		}
		else {
			LCD_P8x16Ch(112, 2, 0);
		}
		LCD_P8x16Ch(120, 2, tempf / 10 + 16);
		LCD_P8x16Ch(128, 2, tempf % 10 + 16);
		LCD_P8x16Ch(136, 2, 95);
		Serial.print(F("FeelsLike: "));
		Serial.println(tempf);    // 获取实况体感温度

		int code;
		code = weatherNow.getIcon();                 // 获取当前天气图标代码
		Disp_weather_icon(0, 2, code);
		Serial.print(F("Icon: "));
		Serial.println(code);

		String weather_txt = weatherNow.getWeatherText();
		DispZHCN(64, 4, "　　　　");
		DispZHCN(64, 4, (char*)weather_txt.c_str());
		Serial.print(F("Weather Now: "));
		Serial.println(weather_txt); // 获取实况天气状况的文字描述

		String winddir_txt = weatherNow.getWindDir();
		DispZHCN(64, 6, "　　　　");
		DispZHCN(64, 6, (char*)winddir_txt.c_str());
		Serial.print(F("windDir: "));
		Serial.println(winddir_txt);     // 获取实况风向

		int winds_cale = weatherNow.getWindScale();
		LCD_P8x16Ch(64, 8, (winds_cale / 10) ? (winds_cale / 10 + 16) : 0);
		LCD_P8x16Ch(72, 8, winds_cale % 10 + 16);
		DispZHCN(80, 8, "级");
		Serial.print(F("WindScale: "));
		Serial.println(winds_cale);   // 获取实况风力等级
		//Serial.print(F("Humidity: "));
		//Serial.println(weatherNow.getHumidity());    // 获取实况相对湿度百分比数值
		//Serial.print(F("Precip: "));
		//Serial.println(weatherNow.getPrecip());      // 获取实况降水量,毫米
	}
	else {    // 更新失败
		Serial.println("Update Failed...");
		Serial.print("Server Response: ");
		Serial.println(weatherNow.getServerCode()); // 参考 https://dev.heweather.com/docs/start/status-code
		Disp_weather_icon(0, 2, 999);
		DispZHCN(64, 2, "　　　　");
	}

	if (weatherforecast.get()) { // 获取天气预报更新
		DispZHCN(64 + 160, 2, "明天");
		DispZHCN(64 + 160 * 2, 2, "后天");
		for (int i = 1; i < 3; i++) {
			//Serial.print("Server Response: ");
			//Serial.println(weatherforecast.getServerCode());    // 获取API状态码
			//Serial.print(F("Last Update: "));
			//Serial.println(weatherforecast.getLastUpdate());    // 获取服务器更新天气信息时间

			int temph = weatherforecast.getTempMax(i);
			if (temph < 0) {
				temph = -temph;
				LCD_P8x16Ch(104 + 160 * i, 6, 13);
			}
			else {
				LCD_P8x16Ch(104 + 160 * i, 6, 0);
			}
			LCD_P8x16Ch(112 + 160 * i, 6, temph / 10 + 16);
			LCD_P8x16Ch(120 + 160 * i, 6, temph % 10 + 16);
			LCD_P8x16Ch(128 + 160 * i, 6, 95);
			Serial.print(F("TempMax: "));
			Serial.println(weatherforecast.getTempMax(i));      // 获取最高温度

			LCD_P8x16Ch(96 + 160 * i, 6, 94);

			int templ = weatherNow.getTemp();
			if (templ < 0) {
				templ = -templ;
				LCD_P8x16Ch(64 + 160 * i, 6, 13);
			}
			else {
				LCD_P8x16Ch(64 + 160 * i, 6, 0);
			}
			LCD_P8x16Ch(72 + 160 * i, 6, templ / 10 + 16);
			LCD_P8x16Ch(80 + 160 * i, 6, templ % 10 + 16);
			LCD_P8x16Ch(88 + 160 * i, 6, 95);
			Serial.print(F("TempMin: "));
			Serial.println(weatherforecast.getTempMin(i));      // 获取最低温度

			int code;
			code = weatherforecast.getIconDay(i);
			Disp_weather_icon(160 * i, 2, code);
			Serial.print(F("Icon: "));
			Serial.println(code);                              // 获取天气图标代码

			String weather_txt = weatherforecast.getTextDay(i);
			DispZHCN(64 + 160 * i, 4, "　　　　");
			DispZHCN(64 + 160 * i, 4, (char*)weather_txt.c_str());
			Serial.print(F("Weather Direction: "));
			Serial.println(weather_txt);      // 获取天气状况的文字描述

			String winddir_txt = weatherforecast.getWindDirDay(i);
			DispZHCN(64 + 160 * i, 8, "　　　　");
			DispZHCN(64 + 160 * i, 8, (char*)winddir_txt.c_str());
			Serial.print(F("WindDir: "));
			Serial.println(winddir_txt);   // 获取风向


			Serial.print(F("WindScale: "));
			Serial.println(weatherforecast.getwindScaleDay(i)); // 获取风力等级

			//Serial.print(F("Humidity: "));
			//Serial.println(weatherforecast.getHumidity(i));     // 获取相对湿度百分比数值
			//Serial.print(F("Precip: "));
			//Serial.println(weatherforecast.getPrecip(i));       // 获取降水量,毫米
			//Serial.print(F("UvIndex: "));
			//Serial.println(weatherforecast.getUvIndex(i));      // 获取紫外线强度指数
		}
	}
	else {    // 更新失败
		Serial.println("Update Failed...");
		Serial.print("Server Response: ");
		Serial.println(weatherforecast.getServerCode()); // 参考 https://dev.heweather.com/docs/start/status-code
	}

	if (airquality.get()) { // 获取空气质量更新
		//Serial.print(F("Last Update: "));
		//Serial.println(airquality.getLastUpdate()); // 获取服务器更新天气信息时间
		int AQI = airquality.getAqi();
		DispStr0816(0, 10, "AQI:");
		LCD_P8x16Ch(32, 10, AQI / 100 + 16);
		LCD_P8x16Ch(40, 10, AQI % 100 / 10 + 16);
		LCD_P8x16Ch(48, 10, AQI % 10 + 16);
		Serial.print(F("AirQuality Aqi: "));
		Serial.println(AQI);        // 实时空气质量指数

		String category = airquality.getCategory();
		DispZHCN(64, 10, "　　　　");
		DispZHCN(64, 10, (char*)category.c_str());
		Serial.print(F("Category: "));
		Serial.println(category);   // 实时空气质量指数级别
		//Serial.print(F("Primary: "));
		//Serial.println(airquality.getPrimary());    // 实时空气质量的主要污染物，优时返回值为 NA
	}
	else {  // 更新失败
		Serial.println("Update Failed...");
		Serial.print("Server Response: ");
		Serial.println(airquality.getServerCode()); // 参考 https://dev.heweather.com/docs/start/status-code
	}
}
