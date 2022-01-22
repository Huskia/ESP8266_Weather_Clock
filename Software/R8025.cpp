#include "R8025.h"

/*--------------------Initialize RTC--------------------*/
int R8025_Init(void) {
	unsigned char RTC_Buf[7]; //缓冲区
	unsigned char i = 0;
	bool pon;
	Wire.beginTransmission(R8025_SLA); //读出2字节设置寄存器，用于测试芯片是否首次上电
	Wire.write(0x0E);
	Wire.endTransmission(0);
	Wire.requestFrom(R8025_SLA, 2);
	while (Wire.available()) {
		RTC_Buf[i] = Wire.read();
		i++;
	}

	pon = RTC_Buf[0] & 0x02;
	if (pon) { //如果断电标志位PON等于1则初始化芯片
		Wire.beginTransmission(R8025_SLA); //将初始设置写入
		Wire.write(0x0E);
		Wire.write(0x00);//
		Wire.write(0x40);//
		Wire.endTransmission(1);
		RTC_Buf[0] = 0x00; //写入秒
		RTC_Buf[1] = 0x00; //写入分
		RTC_Buf[2] = 0x06; //写入时
		RTC_Buf[3] = 0x20; //写入周
		RTC_Buf[4] = 0x01; //写入日
		RTC_Buf[5] = 0x01; //写入月
		RTC_Buf[6] = 0x00; //写入年
		Wire.beginTransmission(R8025_SLA); //将初始设置写入
		Wire.write(0x00);
		Wire.write(RTC_Buf, 7);
		Wire.endTransmission(1);          //写入初始化时间2000-01-01 SAT 06：00：00
	}
	return pon;
}

/*-----------------------Read data from RTC---------------------------*/
void Read_R8025(unsigned char* RTC_Buf) {
	unsigned char i = 0;
	Wire.beginTransmission(R8025_SLA); //Read time reg
	Wire.write(0x00);
	Wire.endTransmission(0);
	Wire.requestFrom(R8025_SLA, 7);
	while (Wire.available()) {
		RTC_Buf[i] = Wire.read();
		i++;
	}
}

/*-----------------------Send data from RTC---------------------------*/
void Write_R8025(unsigned char* RTC_Buf) {
	unsigned char i;
	Wire.beginTransmission(R8025_SLA); //Read time reg
	Wire.write(0x00);
	Wire.write(RTC_Buf, 7);
	Wire.endTransmission(1);
}
