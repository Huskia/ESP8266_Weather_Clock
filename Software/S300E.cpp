#include "S300E.h"

unsigned int Read_CO2(void) {
	unsigned char i = 0;
	unsigned char S300E_Buff[7];
	Wire.beginTransmission(S300E_SLA);
	Wire.write(0x52);
	Wire.endTransmission(0);
	Wire.requestFrom(S300E_SLA, 7);
	while (Wire.available()) {
		S300E_Buff[i] = Wire.read();
		i++;
	}
	return (S300E_Buff[1] << 8) | S300E_Buff[2];
}
