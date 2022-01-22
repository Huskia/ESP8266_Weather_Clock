#include "SHT20.h"

void SHT20_Setup(void) {
	Wire.beginTransmission(SHT20_SLA); //
	Wire.write(0x01);
	Wire.endTransmission(1);
}

void SHT20_Reset(void) {
	Wire.beginTransmission(SHT20_SLA); //
	Wire.write(0xFE);
	Wire.endTransmission(1);
	delay(15);
}

void Trigger_T(void) {
	Wire.beginTransmission(SHT20_SLA); //
	Wire.write(0xF3);
	Wire.endTransmission(1);
}

void Trigger_RH(void) {
	Wire.beginTransmission(SHT20_SLA); //
	Wire.write(0xF5);
	Wire.endTransmission(1);
}

unsigned int Get_Raw_Result(void) {
	unsigned char i = 0;
	unsigned char Read_Buff[3];
	unsigned int Raw;
	//Wire.beginTransmission(SHT20_SLA); //
	//Wire.endTransmission(0);
	if (Wire.requestFrom(SHT20_SLA, 3) != 0) {
		while (Wire.available()) {
			Read_Buff[i] = Wire.read();
			i++;
		}
	}
	else
		return 0;
	Raw = ((unsigned int)Read_Buff[0] << 8) | Read_Buff[1];
	return (Raw & 0xFFFC);
}

int Get_T_Result(void) {
	float T;
	unsigned Raw_Data;
	Raw_Data = Get_Raw_Result();
	//Serial.println(Raw_Data);
	if (Raw_Data > 0)
		T = 175.72 * (Raw_Data / 65536.0) - 46.85;
	else
		T = -50;
	if (T < 0)
		T -= 0.05;
	else
		T += 0.05;
	return (T * 10);
}

int Get_RH_Result(void) {
	float RH;
	unsigned Raw_Data;
	Raw_Data = Get_Raw_Result();
	if (Raw_Data > 0)
		RH = 125.0 * (Raw_Data / 65536.0) - 6.0;
	else
		RH = 0;
	return (RH * 10 + 0.5);
}
