#pragma once

#include <Arduino.h>
#include <Wire.h>

#define SHT20_SLA 0x40

void SHT20_Setup(void);
void SHT20_Reset(void);
void Trigger_T(void);
void Trigger_RH(void);
int Get_T_Result(void);
int Get_RH_Result(void);
