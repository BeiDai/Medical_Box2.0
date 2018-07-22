#include "data.h"
#include "wifi.h"
#include "oled.h"
#include "rtc.h"
/////////////////////////////////////////////////////

//    对ESP8266接收的数据进行解码
//    
//    ESP8266作为接收客户端发送的数据：

//    0,CONNECT

//    +IPD,0,7:12302220,CLOSED     

//    接收到的7位数据 + 结束符0   

//    USART3_RX_BUF[rlen]=0;  

//////////////////////////////////////////////////////


u8 RecMessage[400];

//    从':'字符开始接收数据,存入ReceiveMessage[]中
//    解码接收到的数据
u8* ReceiveMessage(u8 *str)
{
	u8 num=0;
	u8 number;
	u8 i=0;
	u8 rlen=0;
	u8 start=0;
	
	  // 获取数据总长度
	while(str[rlen++]!= 0 ){}   rlen = rlen - 1;  						   

		// 获取 ':' 字符开始位置
	while(str[start++]!=':'){}  start = start - 1;  i = start;   
		
	  // 获取数据个数位数
	while(str[i--]!=','){ num++; }  num = num -1; 	

	  // 根据位数计算得到有效数据长度个数 0 - 999
  if(num == 1) {number = str[start-1] - '0';}  
	
	else if(num == 2) {number = ( str[start-1] - '0') + ( str[start-2] - '0')*10;}
	
	else if(num == 3) {number = (str[start-1] - '0') + ( str[start-2] - '0')*10 + ( str[start-3] - '0')*100;}
	
	else printf("data error!");
	
	  // 调试输出相应数值	
//	printf("开始符号：%c   开始位置：%d   接收数据总长：%d    解码后数据长度：%d \n",str[start],start,rlen,number);
		
    // 解码数据，得到的数据记录到RecMessage中
		
	for(i=0;i<number;i++)
	{
		RecMessage[i] = str[start+i+1];
	}
		
	RecMessage[i] = 0;

	// 返回解码后的数据
	return (u8*)RecMessage;
}


//  检测收到数据的是否为 +PID .... 格式的
//  是 返回1
//  否 返回0

u8 ReceiveMessageTrue(u8 *str)
{
	u8 num = 0;
	u8 rlen = 0;
	u8 i=0;
		  // 获取数据总长度
	while(str[rlen++]!= 0 ){}   rlen = rlen - 1;  	
		
	for(i=0;i<rlen;i++)
		{
			if(str[i] == '+'){ num = 1; }
		}
		
	return num;
}



void DisplayDate(u8 *RecMessage)
{
	/******************************   修改药盒时间   *************************************/
				// 时间接收数据格式 6#0018-04-21#18:00:00#7
				// 具有校验数据功能 正确则修改系统时间

				// 定义药盒时间
				u16 Set_Year; u8 Set_Month; u8 Set_Day; u8 Set_Hour; u8 Set_Minute; u8 Set_Second; u8 Set_Week;
			  u8 Start_Number = 0;
				u8 Date_Right = 0;  // 校验数据是否正确
				u8 Number; 
				u8 Frist_Number_Day = Start_Number + 2;  // 日期起始位置
				u8 Frist_Number_Time = Start_Number +13; // 时间起始位置
				u8 Number_Week = Start_Number + 22;			 // 周起始位置
				
				// 转化数据为整型格式
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
				
				// 转化数据为年月日，时分秒，周，格式
				Set_Year = RecMessage[Frist_Number_Day]*1000 + RecMessage[Frist_Number_Day+1]*100 + RecMessage[Frist_Number_Day+2]*10 + RecMessage[Frist_Number_Day+3];
				Set_Month = RecMessage[Frist_Number_Day+5]*10 + RecMessage[Frist_Number_Day+6];
				Set_Day = RecMessage[Frist_Number_Day+8]*10 + RecMessage[Frist_Number_Day+9];
				
				
				Set_Hour = RecMessage[Frist_Number_Time]*10 + RecMessage[Frist_Number_Time+1];
				Set_Minute = RecMessage[Frist_Number_Time+3]*10 + RecMessage[Frist_Number_Time+4];
				Set_Second = RecMessage[Frist_Number_Time+6]*10 + RecMessage[Frist_Number_Time+7];
				
				Set_Week = RecMessage[Number_Week];
				
				// 判断数据是否满足格式要求
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
				
				// 数据满足格式要求更改系统时间
				if(Date_Right == 1)
				{
					RTC_TimeAndDate_SetByUser(Set_Year,Set_Month,Set_Day,Set_Hour,Set_Minute,Set_Second,Set_Week);
					printf("RTC_TimeAndDate_SetByUser");
				}

}







