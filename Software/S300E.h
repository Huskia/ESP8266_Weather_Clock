#pragma once

#include <Arduino.h>
#include <Wire.h>

#define S300E_SLA 0x31

unsigned int Read_CO2(void);
