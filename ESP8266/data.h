#ifndef __DATA_H
#define __DATA_H
#include "sys.h"
/////////////////////////////////////////////////////

//    对ESP8266接收的数据进行解码
//     


extern u8 RecMessage[400];   // 接收到的数据

u8* ReceiveMessage(u8 *str);  // 解码接收到的数据，返回解码后的数据

u8 ReceiveMessageTrue(u8 *str);  // 判断接收到的数据是否为 +PID ...类型数据

void DisplayDate(u8 *RecMessage);  // 显示时间
 
#endif
