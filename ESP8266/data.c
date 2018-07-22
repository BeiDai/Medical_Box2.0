#include "data.h"
#include "wifi.h"
#include "oled.h"
#include "rtc.h"
/////////////////////////////////////////////////////

//    ��ESP8266���յ����ݽ��н���
//    
//    ESP8266��Ϊ���տͻ��˷��͵����ݣ�

//    0,CONNECT

//    +IPD,0,7:12302220,CLOSED     

//    ���յ���7λ���� + ������0   

//    USART3_RX_BUF[rlen]=0;  

//////////////////////////////////////////////////////


u8 RecMessage[400];

//    ��':'�ַ���ʼ��������,����ReceiveMessage[]��
//    ������յ�������
u8* ReceiveMessage(u8 *str)
{
	u8 num=0;
	u8 number;
	u8 i=0;
	u8 rlen=0;
	u8 start=0;
	
	  // ��ȡ�����ܳ���
	while(str[rlen++]!= 0 ){}   rlen = rlen - 1;  						   

		// ��ȡ ':' �ַ���ʼλ��
	while(str[start++]!=':'){}  start = start - 1;  i = start;   
		
	  // ��ȡ���ݸ���λ��
	while(str[i--]!=','){ num++; }  num = num -1; 	

	  // ����λ������õ���Ч���ݳ��ȸ��� 0 - 999
  if(num == 1) {number = str[start-1] - '0';}  
	
	else if(num == 2) {number = ( str[start-1] - '0') + ( str[start-2] - '0')*10;}
	
	else if(num == 3) {number = (str[start-1] - '0') + ( str[start-2] - '0')*10 + ( str[start-3] - '0')*100;}
	
	else printf("data error!");
	
	  // ���������Ӧ��ֵ	
//	printf("��ʼ���ţ�%c   ��ʼλ�ã�%d   ���������ܳ���%d    ��������ݳ��ȣ�%d \n",str[start],start,rlen,number);
		
    // �������ݣ��õ������ݼ�¼��RecMessage��
		
	for(i=0;i<number;i++)
	{
		RecMessage[i] = str[start+i+1];
	}
		
	RecMessage[i] = 0;

	// ���ؽ���������
	return (u8*)RecMessage;
}


//  ����յ����ݵ��Ƿ�Ϊ +PID .... ��ʽ��
//  �� ����1
//  �� ����0

u8 ReceiveMessageTrue(u8 *str)
{
	u8 num = 0;
	u8 rlen = 0;
	u8 i=0;
		  // ��ȡ�����ܳ���
	while(str[rlen++]!= 0 ){}   rlen = rlen - 1;  	
		
	for(i=0;i<rlen;i++)
		{
			if(str[i] == '+'){ num = 1; }
		}
		
	return num;
}



void DisplayDate(u8 *RecMessage)
{
	/******************************   �޸�ҩ��ʱ��   *************************************/
				// ʱ��������ݸ�ʽ 6#0018-04-21#18:00:00#7
				// ����У�����ݹ��� ��ȷ���޸�ϵͳʱ��

				// ����ҩ��ʱ��
				u16 Set_Year; u8 Set_Month; u8 Set_Day; u8 Set_Hour; u8 Set_Minute; u8 Set_Second; u8 Set_Week;
			  u8 Start_Number = 0;
				u8 Date_Right = 0;  // У�������Ƿ���ȷ
				u8 Number; 
				u8 Frist_Number_Day = Start_Number + 2;  // ������ʼλ��
				u8 Frist_Number_Time = Start_Number +13; // ʱ����ʼλ��
				u8 Number_Week = Start_Number + 22;			 // ����ʼλ��
				
				// ת������Ϊ���͸�ʽ
				for(Number=Start_Number+2; Number<=Start_Number+22 ; Number++)
				{
					RecMessage[Number] = RecMessage[Number] - '0';
//					printf("   RecMessage[%d] = %d\n",Number,RecMessage[Number]);
				}
				
//				printf("Frist_Number_Day = %d\n",RecMessage[Frist_Number_Day]);
//				
//				printf("Number_Week = %d\n",RecMessage[Number_Week]);
//				
//				
				
				// ת������Ϊ�����գ�ʱ���룬�ܣ���ʽ
				Set_Year = RecMessage[Frist_Number_Day]*1000 + RecMessage[Frist_Number_Day+1]*100 + RecMessage[Frist_Number_Day+2]*10 + RecMessage[Frist_Number_Day+3];
				Set_Month = RecMessage[Frist_Number_Day+5]*10 + RecMessage[Frist_Number_Day+6];
				Set_Day = RecMessage[Frist_Number_Day+8]*10 + RecMessage[Frist_Number_Day+9];
				
				
				Set_Hour = RecMessage[Frist_Number_Time]*10 + RecMessage[Frist_Number_Time+1];
				Set_Minute = RecMessage[Frist_Number_Time+3]*10 + RecMessage[Frist_Number_Time+4];
				Set_Second = RecMessage[Frist_Number_Time+6]*10 + RecMessage[Frist_Number_Time+7];
				
				Set_Week = RecMessage[Number_Week];
				
				// �ж������Ƿ������ʽҪ��
				if(Set_Year>0 && Set_Year<999)			Date_Right = 1;  else Date_Right = 0;
				if(Set_Month>0 && Set_Month<24)			Date_Right = 1;  else Date_Right = 0;
				if(Set_Day>0 && Set_Day<32)					Date_Right = 1;  else Date_Right = 0;
				
				if(Set_Hour>0 && Set_Hour<24)				Date_Right = 1;  else Date_Right = 0;
				if(Set_Minute>0 && Set_Minute<60)		Date_Right = 1;  else Date_Right = 0;
				if(Set_Second>0 && Set_Second<60)		Date_Right = 1;  else Date_Right = 0;
				
				if(Set_Week>0 && Set_Week<=7)				Date_Right = 1;  else Date_Right = 0;
				
//				printf("Set_Year = %d\n",Set_Year);
//				printf("Set_Month = %d\n",Set_Month);
//				printf("Set_Day = %d\n",Set_Day);
//				printf("Set_Hour = %d\n",Set_Hour);
//				printf("Set_Minute = %d\n",Set_Minute);
//				printf("Set_Second = %d\n",Set_Second);
//				printf("Set_Week = %d\n",Set_Week);
//				printf("Date_Right = %d\n",Date_Right);
				
				// ���������ʽҪ�����ϵͳʱ��
				if(Date_Right == 1)
				{
					RTC_TimeAndDate_SetByUser(Set_Year,Set_Month,Set_Day,Set_Hour,Set_Minute,Set_Second,Set_Week);
					printf("RTC_TimeAndDate_SetByUser");
				}

}







