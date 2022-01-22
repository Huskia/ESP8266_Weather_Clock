#include "ST75256.h"


/*********************LCD写数据/命令**************************/
void Write_LCD(bool operation, unsigned char dat) {
	digitalWrite(LCD_A0, operation ? 1 : 0);
	digitalWrite(LCD_WR, 0);
	digitalWrite(LCD_RCK, 0);
	SPI.transfer(dat);
	digitalWrite(LCD_RCK, 1);
	digitalWrite(LCD_WR, 1);
}

/********************双ST75256初始化************************/
void ST75256_Init(void) {
	SPI.begin();
	SPI.setFrequency(3000000);
	pinMode(LCD_WR, OUTPUT);
	pinMode(LCD_A0, OUTPUT);
	pinMode(LCD_RCK, OUTPUT);
	pinMode(LCD_Light, OUTPUT);
	delay(50);
	/*--------------------------------------------------Slave Part Start---------------------------------------------------*/
	Write_LCD(Command, 0x30); // Extension Command 1 (0x30 for EC1, 0x31 for EC2, 0x38 for EC3, 0x39 for EC4)
	Write_LCD(Command, 0x6F); // Enable Slave

	Write_LCD(Command, 0x31); // Extension Command 2
	Write_LCD(Command, 0xD7); // Disable Auto Read
	Write_LCD(Data, 0x9F);
	Write_LCD(Command, 0xE0); // Enable OTP Read
	Write_LCD(Data, 0x00);
	delay(10);
	Write_LCD(Command, 0xE3); // OTP Up-Load
	delay(20);
	Write_LCD(Command, 0xE1); // OTP Control Out

	Write_LCD(Command, 0x30); // Extension Command 1
	Write_LCD(Command, 0x94); // Sleep Out
	delay(50);
	Write_LCD(Command, 0x20); // Power Control
	Write_LCD(Data, 0x01); // VB OFF ; VR,ON ; VF OFF
	Write_LCD(Command, 0xF0); // Display Mode
	Write_LCD(Data, 0x10); // Monochrome Mode
	Write_LCD(Command, 0xCA); // Display Control
	Write_LCD(Data, 0x00); // CL Dividing Ratio? Not Divide
	Write_LCD(Data, 0x9F); // Duty=160
	Write_LCD(Data, 0x00); // Frame inversion
	Write_LCD(Command, 0xBC); // Data Scan Direction
	Write_LCD(Data, 0x03);

	Write_LCD(Command, 0x31); // Extension Command 2
	Write_LCD(Command, 0x32); // Analog Circuit Set
	Write_LCD(Data, 0x00);
	Write_LCD(Data, 0x01); // Booster Efficiency =Level 1
	Write_LCD(Data, 0x02); //Bias=1/12

	Write_LCD(Command, 0x30); // Extension Command 1
	Write_LCD(Command, 0x75); // Page Address Setting
	Write_LCD(Data, 0x09); // COM64 -> COM160
	Write_LCD(Data, 0x14);
	Write_LCD(Command, 0x15); // Column Address Setting
	Write_LCD(Data, 0x10); // SEG15 -> SEG255
	Write_LCD(Data, 0xFF);
	Write_LCD(Command, 0xA6); //Inverse Display
	Write_LCD(Command, 0xAF); // Display On
/*--------------------------------------------------Slave Part End---------------------------------------------------*/
/*------------------------------------------------Master Part Start--------------------------------------------------*/
	Write_LCD(Command, 0x30); // Extension Command 1
	Write_LCD(Command, 0x6E); // Enable Master

	Write_LCD(Command, 0x31); // Extension Command 2
	Write_LCD(Command, 0xD7); // Disable Auto Read
	Write_LCD(Data, 0x9F);
	Write_LCD(Command, 0xE0); // Enable OTP Read
	Write_LCD(Data, 0x00);
	delay(10);
	Write_LCD(Command, 0xE3); // OTP Up-Load
	delay(20);
	Write_LCD(Command, 0xE1); // OTP Control Out

	Write_LCD(Command, 0x30); // Extension Command 1
	Write_LCD(Command, 0x94); // Sleep Out
	delay(50);
	Write_LCD(Command, 0xD1); //OSC ON
	Write_LCD(Command, 0x20); // Power Control
	Write_LCD(Data, 0x0B); // VB ON ; VR,VF ON
	Write_LCD(Command, 0x81); // Set Vop = 16.80V
	Write_LCD(Data, 0x0C);
	Write_LCD(Data, 0x05);
	Write_LCD(Command, 0xF0); // Display Mode
	Write_LCD(Data, 0x10); // Monochrome Mode
	Write_LCD(Command, 0xCA); // Display Control
	Write_LCD(Data, 0x00); // CL Dividing Ratio? Not Divide
	Write_LCD(Data, 0x9F); // Duty=160
	Write_LCD(Data, 0x00); // Frame inversion
	Write_LCD(Command, 0xBC); // Data Scan Direction
	Write_LCD(Data, 0x03);

	Write_LCD(Command, 0x31); // Extension Command 2
	Write_LCD(Command, 0x32); // Analog Circuit Set
	Write_LCD(Data, 0x00);
	Write_LCD(Data, 0x01); // Booster Efficiency =Level 1
	Write_LCD(Data, 0x02); //Bias=1/12
	Write_LCD(Command, 0x40); // Internal Power Supply

	Write_LCD(Command, 0x30); // Extension Command 1
	Write_LCD(Command, 0x75); // Page Address Setting
	Write_LCD(Data, 0x09); // COM64 -> COM160
	Write_LCD(Data, 0x14);
	Write_LCD(Command, 0x15); // Column Address Setting
	Write_LCD(Data, 0x10); // SEG15 -> SEG255
	Write_LCD(Data, 0xFF);
	Write_LCD(Command, 0xA6); //Inverse Display
	Write_LCD(Command, 0xAF); //Display On
/*--------------------------------------------------Master Part End-------------------------------------------------*/
}

/*************************************设置坐标********************************************/
void LCD_Set_Coordinate(unsigned int x, unsigned char y) {
	if (x < 240) {
		Write_LCD(Command, 0x6E); // Enable Master
		Write_LCD(Command, 0x75); // Page Address Setting
		Write_LCD(Data, y + 9);
		Write_LCD(Data, 0x14);
		Write_LCD(Command, 0x15); // Column Address Setting
		Write_LCD(Data, x + 16);
		Write_LCD(Data, 0xFF);
	}
	else {
		Write_LCD(Command, 0x6F); // Enable Slave
		Write_LCD(Command, 0x75); // Page Address Setting
		Write_LCD(Data, y + 9); // COM0 -> COM96
		Write_LCD(Data, 0x14);
		Write_LCD(Command, 0x15); // Column Address Setting
		Write_LCD(Data, x - 224); // SEG0 -> SEG239
		Write_LCD(Data, 0xFF);
	}
	Write_LCD(Command, 0x5C); //开始写入显示数据
}

/*************************************清屏********************************************/
void LCD_CLS(void) {
	unsigned int i;
	LCD_Set_Coordinate(0, 0);
	for (i = 0; i < 2880; i++)
		Write_LCD(Data, 0x00);
	LCD_Set_Coordinate(240, 0);
	for (i = 0; i < 2880; i++)
		Write_LCD(Data, 0x00);
}


/************************************以下为实用函数*************************************/

/***************功能描述：显示6*8的一个ASCII字符	显示的坐标（x,y），y为页范围0～7****************/
void LCD_P6x8Ch(unsigned int x, unsigned char y, unsigned char n) {
	unsigned char i = 0;
	LCD_Set_Coordinate(x, y);
	for (i = 0; i < 6; i++)
		Write_LCD(Data, F6x8[n][i]);
}

/***************功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7****************/
void DispStr0608(unsigned int x, unsigned char y, char* ch) {
	unsigned char c = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if (x > 478) { x = 0; y++; }
		LCD_P6x8Ch(x, y, c);
		x += 6;
		j++;
	}
}

/***************功能描述：显示6*8的ASCII数字	显示的坐标（x,y），y为页范围0～7****************/
void DispNum0608(unsigned int x, unsigned char y, unsigned int a) {
	char i = 0, j;
	unsigned int k;
	bool zero = 1;
	for (i = 0; i < 5; i++)//判断数字是否结束
	{
		switch (i) {         //从高位分离数字
		case 0: k = a / 10000; break;
		case 1: k = a / 1000;  break;
		case 2: k = a / 100;   break;
		case 3: k = a / 10;    break;
		case 4: k = a;       break;
		default: break;
		}
		j = k % 10 + 16;//获取当数字加16后的值，这个值就是这个数字在精简版ASCII表中的序号
		if (j == 16 && i < 4 && zero == 1) //判断有效数字前面是否有0，隐去无效的0，最后一位是0则保留
			j = 0;
		else
			zero = 0;
		LCD_P6x8Ch(x + i * 6, y, j);
	}
}

/*******************功能描述：显示8*16一个标准ASCII字符	 显示的坐标（x,y），y为页范围0～7****************/
void LCD_P8x16Ch(unsigned int x, unsigned char y, unsigned char n) {
	unsigned char i = 0;
	LCD_Set_Coordinate(x, y);
	for (i = 0; i < 8; i++)
		Write_LCD(Data, F8X16[n * 16 + i]);
	LCD_Set_Coordinate(x, y + 1);
	for (i = 0; i < 8; i++)
		Write_LCD(Data, F8X16[n * 16 + i + 8]);
}

/*******************功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～7****************/
void DispStr0816(unsigned int x, unsigned char y, char* ch) {
	unsigned char c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if (x > 472) { x = 0; y += 2; }
		LCD_P8x16Ch(x, y, c);
		x += 8;
		j++;
	}
}

/***************功能描述：显示8*16的ASCII有效数字	显示的坐标（x,y），y为页范围0～7****************/
void DispNum0816(unsigned int x, unsigned char y, unsigned int a) {
	char i = 0, j;
	unsigned int k;
	bool zero = 1;
	for (i = 0; i < 5; i++)//判断数字是否结束
	{
		switch (i) {         //从高位分离数字
		case 0: k = a / 10000; break;
		case 1: k = a / 1000;  break;
		case 2: k = a / 100;   break;
		case 3: k = a / 10;    break;
		case 4: k = a;       break;
		default: break;
		}
		j = k % 10 + 16;//获取当数字加16后的值，这个值就是这个数字在精简版ASCII表中的序号
		if (j == 16 && i < 4 && zero == 1) //判断有效数字前面是否有0，隐去无效的0，最后一位是0则保留
			j = 0;
		else
			zero = 0;
		LCD_P8x16Ch(x + i * 8, y, j);
	}
}

/**********************************显示16×16的中文字符*************************************/
unsigned char FindChnGBK(char* p, unsigned int* GBKpos) {
	unsigned int i, j;
	j = Chinese_Num;
	for (i = 0; i < j; i++)
	{
		if (!memcmp(Chinese1616[i].ChineseGBK, p, 3)) {
			(*GBKpos) = i;
			return 1;
		}
	}
	return 0;
}

void display1616(unsigned char page, unsigned int column, unsigned char* p) {
	unsigned char i, j;
	for (i = 0; i < 2; i++)
	{
		LCD_Set_Coordinate(column, page + i);
		for (j = 0; j < 16; j++)
		{
			Write_LCD(Data, *p);
			p++;
		}

	}
}

void DispZHCN(unsigned int x, unsigned char y, char* p) {
	unsigned int GBKPos;
	while (*p != '\0') {
		if (FindChnGBK(p, &GBKPos)) {
			display1616(y, x, (unsigned char*)&Chinese1616[GBKPos].ChineseMAP);
			x += 16;
		}
		p += 3;
	}
}

/***********功能描述：显示显示BMP图片,起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void Disp_BMP(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, const unsigned char* BMP, bool inverse) {
	unsigned int j = 0;
	unsigned int x, y;

	for (y = y0; y <= y1; y++)
	{
		LCD_Set_Coordinate(x0, y);
		for (x = x0; x <= x1; x++) {
			if (x == 240)
				LCD_Set_Coordinate(x, y);
			if (inverse)
				Write_LCD(Data, ~BMP[j++]);
			else
				Write_LCD(Data, BMP[j++]);
		}
	}
}
/************************************************************************************************/

/*********************************************************************************/
