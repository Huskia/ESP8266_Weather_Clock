#ifndef _CODETAB_ZH_H_
#define _CODETAB_ZH_H_

#include <Arduino.h>

typedef struct tag {                  //汉字字模数据结构
	char ChineseGBK[4];
	char ChineseMAP[32];
}tagChinese;

extern tagChinese Chinese1616[];
extern unsigned int Chinese_Num;

#endif
