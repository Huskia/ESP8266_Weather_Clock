#pragma once
/*------------------------------------------------------------------------------
*                                 Instruction
*   This is the driver of R8025T RTC IC for Arduino.
*   It is worth mentioning that this driver can't work on R8025AC though they
* have samiliar part name.
*   Data format is BCD except weekday.
*   Original time is set to 2000-01-01 SAT 06：00：00.
--------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>

#define R8025_SLA 0x32

int R8025_Init(void);
void Read_R8025(unsigned char* RTC_Buf);
void Write_R8025(unsigned char* RTC_Buf);
