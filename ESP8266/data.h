#ifndef __DATA_H
#define __DATA_H
#include "sys.h"
/////////////////////////////////////////////////////

//    ��ESP8266���յ����ݽ��н���
//     


extern u8 RecMessage[400];   // ���յ�������

u8* ReceiveMessage(u8 *str);  // ������յ������ݣ����ؽ���������

u8 ReceiveMessageTrue(u8 *str);  // �жϽ��յ��������Ƿ�Ϊ +PID ...��������

void DisplayDate(u8 *RecMessage);  // ��ʾʱ��
 
#endif
