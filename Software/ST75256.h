#ifndef _ST75256_H_
#define _ST75256_H_

#include <Arduino.h>
#include <SPI.h>
#include "string.h"
#include "codetab.h"
#include "codetab_zh.h"

#define LCD_WR 2
#define LCD_A0 15
#define LCD_RCK 0
#define LCD_Light 16

#define Data 1
#define Command 0

void Write_LCD(bool operation, unsigned char dat);
void ST75256_Init(void);
void LCD_Set_Coordinate(unsigned int x, unsigned char y);
void LCD_CLS(void);

void LCD_P6x8Ch(unsigned int x, unsigned char y, unsigned char n);
void DispStr0608(unsigned int x, unsigned char y, char* ch);
void DispNum0608(unsigned int x, unsigned char y, unsigned int a);

void LCD_P8x16Ch(unsigned int x, unsigned char y, unsigned char n);
void DispStr0816(unsigned int x, unsigned char y, char* ch);
void DispNum0816(unsigned int x, unsigned char y, unsigned int a);

unsigned char FindChnGBK(char* p, unsigned int* GBKpos);
void display1616(unsigned char page, unsigned char column, unsigned char* p);
void DispZHCN(unsigned int x, unsigned char y, char* p);

void Disp_BMP(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, const unsigned char* BMP, bool inverse);

#endif
