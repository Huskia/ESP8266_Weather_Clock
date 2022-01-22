#ifndef _CODETAB_H_
#define _CODETAB_H_

#include <Arduino.h>

extern const unsigned char F6x8[][6];
extern const unsigned char F8X16[];
extern const unsigned char LOGO[];

extern const unsigned char WIFI_ICON_0[];
extern const unsigned char WIFI_ICON_1[];
extern const unsigned char WIFI_ICON_2[];
extern const unsigned char WIFI_ICON_3[];
extern const unsigned char WIFI_ICON_4[];

extern const unsigned char Server_NC[];
extern const unsigned char Server_SC[];
extern const unsigned char NTP_OK[];
extern const unsigned char NTP_Wait[];
extern const unsigned char NTP_Fail[];

extern const unsigned char Num24x48[];

typedef struct ICON {                  //汉字字模数据结构
	int ICON_NUM;
	const unsigned char ICON_DATA[512];
}ICON_TAB;

extern ICON_TAB ALL_ICON[];

#endif
