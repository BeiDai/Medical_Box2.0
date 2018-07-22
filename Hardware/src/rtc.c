#include "rtc.h"
#include "sys.h"
#include "oled.h"

/**
  * @brief  ���ó�ʼʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Set(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// ��ʼ��ʱ��
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // ��ʼ������	
	RTC_DateStructure.RTC_WeekDay = WEEKDAY;       
	RTC_DateStructure.RTC_Date = DATE;         
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}


/**
  * @brief  �����û��Զ����ʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_SetByUser(u16 year, u8 month, u8 day, u8 hour, u8 minute, u8 second, u8 weekday)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// ��ʼ��ʱ��
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = hour;        
	RTC_TimeStructure.RTC_Minutes = minute;      
	RTC_TimeStructure.RTC_Seconds = second;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // ��ʼ������	
	RTC_DateStructure.RTC_WeekDay = weekday;       
	RTC_DateStructure.RTC_Date = day;         
	RTC_DateStructure.RTC_Month = month;         
	RTC_DateStructure.RTC_Year = year;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}


/**
  * @brief  ��С��10��������0���
  * @param  ��
  * @retval ��
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
  * @brief  ��ʾʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Show(void)
{
	uint8_t Rtctmp=0;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	
	OLED_Refresh_Gram();
	
	
	// ��ȡ���ں�ʱ��
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	
	//������
//		if(RTC_TimeStructure.RTC_Seconds == 5)
//		{
//			RTC_TimeAndDate_SetByUser(1, 1, 1, 1, 1, 1, 4);
//		}
	
	// ÿ���ӡһ��
	if(Rtctmp != RTC_TimeStructure.RTC_Seconds)
	{
		//x1,x2�ֱ�Ϊ1��2�е��������ֵ��yΪ������ֵ��sizeΪ�ַ���С
		int x1 = 15, x2 = 10, y = 30, size = 12;
		
		//�������
		//����ǰ���,��ֹ�ַ�����
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
		
		//���ʱ��			
		RTC_TimeAndDate_Put(x2 + 20, y + 15, RTC_TimeStructure.RTC_Hours, size);
		OLED_ShowChar(x2 + 34, y + 15,':',12,1);
		RTC_TimeAndDate_Put(x2 + 41, y + 15, RTC_TimeStructure.RTC_Minutes, size);
		OLED_ShowChar(x2 + 55, y + 15,':',12,1);
		RTC_TimeAndDate_Put(x2 + 62, y + 15, RTC_TimeStructure.RTC_Seconds, size);						
		
		//������ڼ�
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
		
		//����δ֪���Լ�ʱ�ƺ�û��Ӱ��
		 (void)RTC->DR;
	}
	
	//�������ֵ���ڱȽ�
	Rtctmp = RTC_TimeStructure.RTC_Seconds;
}	



/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ������RTC_CLK�ķ�Ƶϵ��
  * @param  ��
  * @retval ��
  */
void RTC_CLK_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/*ʹ�� PWR ʱ��*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
  PWR_BackupAccessCmd(ENABLE);

  /* ʹ��LSE */ 
  RCC_LSEConfig(RCC_LSE_ON);
   /* �ȴ�LSE�ȶ� */   
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* ѡ��LSE��ΪRTC��ʱ��Դ */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    



  /* ʹ��RTCʱ�� */
  RCC_RTCCLKCmd(ENABLE);

  /* �ȴ� RTC APB �Ĵ���ͬ�� */
  RTC_WaitForSynchro();
   
/*=====================��ʼ��ͬ��/�첽Ԥ��Ƶ����ֵ======================*/
	/* ����������ʱ��ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* �����첽Ԥ��Ƶ����ֵ */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* ����ͬ��Ԥ��Ƶ����ֵ */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* ��RTC_InitStructure�����ݳ�ʼ��RTC�Ĵ��� */
	RTC_Init(&RTC_InitStructure);
}






