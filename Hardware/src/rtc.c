#include "rtc.h"
#include "sys.h"
#include "oled.h"

/**
  * @brief  设置初始时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Set(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// 初始化时间
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // 初始化日期	
	RTC_DateStructure.RTC_WeekDay = WEEKDAY;       
	RTC_DateStructure.RTC_Date = DATE;         
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}


/**
  * @brief  设置用户自定义的时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_SetByUser(u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second, u8 weekday)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// 初始化时间
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = hour;        
	RTC_TimeStructure.RTC_Minutes = minute;      
	RTC_TimeStructure.RTC_Seconds = second;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // 初始化日期	
	RTC_DateStructure.RTC_WeekDay = weekday;       
	RTC_DateStructure.RTC_Date = day;         
	RTC_DateStructure.RTC_Month = month;         
	RTC_DateStructure.RTC_Year = year;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}


/**
  * @brief  对小于10的数，补0输出
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Put(int x, int y, int out, int size)
{
	if(out < 10)
	{
		OLED_ShowNum(x, y, 0, 1, size);
		OLED_ShowNum(x + 7, y, out, 1, size);
	}
	else
	{
		OLED_ShowNum(x, y, out, 2, size);
	}
}

/**
  * @brief  显示时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Show(void)
{
	uint8_t Rtctmp=0;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	
	OLED_Refresh_Gram();
	
	
	// 获取日期和时间
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	
	//调试用
//		if(RTC_TimeStructure.RTC_Seconds == 5)
//		{
//			RTC_TimeAndDate_SetByUser(1, 1, 1, 1, 1, 1, 4);
//		}
	
	// 每秒打印一次
	if(Rtctmp != RTC_TimeStructure.RTC_Seconds)
	{
		//x1,x2分别为1、2行的列坐标基值，y为行坐标值，size为字符大小
		int x1 = 15, x2 = 10, y = 30, size = 12;
		
		//输出日期
		//输入前清空,防止字符残留
//		OLED_ShowString(x1 + 0, y, "                       ", size);
//		OLED_ShowString(x2 + 0, y, "                                    ", size);
		OLED_ShowString(0, y, "                                    ", size);
		OLED_ShowString(0, y, "                                    ", size);
		
		
		OLED_ShowNum(x1 + 0, y, 2, 1, size);
		OLED_ShowNum(x1 + 7, y,0, 1, size);
		RTC_TimeAndDate_Put(x1 + 14, y, RTC_DateStructure.RTC_Year, size);
		OLED_ShowChar(x1 + 28, y,'-',12,1);
		RTC_TimeAndDate_Put(x1 + 35, y, RTC_DateStructure.RTC_Month, size);
		OLED_ShowChar(x1 + 49, y,'-',12,1);
		RTC_TimeAndDate_Put(x1 + 56, y, RTC_DateStructure.RTC_Date, size);
		
		//输出时间			
		RTC_TimeAndDate_Put(x2 + 20, y + 15, RTC_TimeStructure.RTC_Hours, size);
		OLED_ShowChar(x2 + 34, y + 15,':',12,1);
		RTC_TimeAndDate_Put(x2 + 41, y + 15, RTC_TimeStructure.RTC_Minutes, size);
		OLED_ShowChar(x2 + 55, y + 15,':',12,1);
		RTC_TimeAndDate_Put(x2 + 62, y + 15, RTC_TimeStructure.RTC_Seconds, size);						
		
		//输出星期几
		switch(RTC_DateStructure.RTC_WeekDay)
		{
			case 1:OLED_ShowString(x1 + 80, y,"Mon", size);break;
			case 2:OLED_ShowString(x1 + 80, y,"Tue", size);break;
			case 3:OLED_ShowString(x1 + 80, y,"Wed", size);break;
			case 4:OLED_ShowString(x1 + 80, y,"Thur", size);break;
			case 5:OLED_ShowString(x1 + 80, y,"Fri", size);break;
			case 6:OLED_ShowString(x1 + 80, y,"Sat", size);break;
			case 7:OLED_ShowString(x1 + 80, y,"Sun", size);break;
			default:break;
		}
		
		//作用未知，对计时似乎没有影响
		 (void)RTC->DR;
	}
	
	//返回秒的值用于比较
	Rtctmp = RTC_TimeStructure.RTC_Seconds;
}	



/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
void RTC_CLK_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/*使能 PWR 时钟*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
  PWR_BackupAccessCmd(ENABLE);

  /* 使能LSE */ 
  RCC_LSEConfig(RCC_LSE_ON);
   /* 等待LSE稳定 */   
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* 选择LSE做为RTC的时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    



  /* 使能RTC时钟 */
  RCC_RTCCLKCmd(ENABLE);

  /* 等待 RTC APB 寄存器同步 */
  RTC_WaitForSynchro();
   
/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* 设置异步预分频器的值 */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* 设置同步预分频器的值 */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	RTC_Init(&RTC_InitStructure);
}






