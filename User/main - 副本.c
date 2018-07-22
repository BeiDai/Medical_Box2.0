
/****************           ��ҩ��1.0�汾       ************************/

#include "sys.h"
#include "delay.h"
#include "usart.h"

/**************             �û�����ͷ�ļ�        ***********************/

#include "led.h"
#include "usart3.h"
#include "wifi.h"
#include "motor.h"
#include "reedswitch.h"
#include "oled.h"

/**************           uCOSIII ϵͳͷ�ļ�      ***********************/

#include "includes.h"
#include "os_app_hooks.h"

/****************    UCOSIII���������ȼ��û�������ʹ��   ***************/

/*	����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
		���ȼ�0���жϷ������������� OS_IntQTask()
		���ȼ�1��ʱ�ӽ������� OS_TickTask()
		���ȼ�2����ʱ���� OS_TmrTask()
		���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
		���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
*/

/*********************           ������            *********************/


/*****************************   ��ʼ����  *******************************/

//�������ȼ� 
#define START_TASK_PRIO		10
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

/**********************    LEDָʾ��    *****************************/

//�������ȼ�
#define LED_TASK_PRIO		4
//�����ջ��С	
#define LED_STK_SIZE 		128
//������ƿ�
OS_TCB LedTaskTCB;
//�����ջ	
CPU_STK LED_TASK_STK[LED_STK_SIZE];
//������
void led_task(void *p_arg);

/************************  ҩ��λ�ó�ʼ��  ***************************/

//�������ȼ�
#define POS_TASK_PRIO		5
//�����ջ��С	
#define POS_STK_SIZE 		128
//������ƿ�
OS_TCB PosTaskTCB;
//�����ջ	
CPU_STK POS_TASK_STK[POS_STK_SIZE];
//������
void pos_task(void *p_arg);

/******************************  ������  *****************************/

//�������ȼ�
#define MAIN_TASK_PRIO		6
//�����ջ��С
#define MAIN_STK_SIZE		1200
//������ƿ�
OS_TCB	MainTaskTCB;
//�����ջ
__align(8) CPU_STK	MAIN_TASK_STK[MAIN_STK_SIZE];
//������
void main_task(void *p_arg);


/*****************************  ����  ******************************/

u16 SendTime; //��������ʱ�� 100ms*SendTime = ��Ƭ���������ݼ��ʱ��
// SendTime = 60 ��6s��һ��


/*****************************  ������  ******************************/

int main(void)
{
  	OS_ERR err;
  	CPU_SR_ALLOC();
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
		delay_init(168);  
		ReedSwitchInit();
		LED_Init();
		uart_init(115200);//��ʼ�����ڲ�����Ϊ115200
		usart3_init(115200); 
		Motor_Init();
		EN = 1; //�رյ����Դ
		
  	SendTime = 600;
	
//	USART3_RX_STA=0; //�����������
//	positionNow = 1; //ҩ�г�ʼλ��Ϊ1
	
		OLED_Init();	
		OLED_ShowChinese(0+30,0,0,12,1); //��
		OLED_ShowChinese(14+30,0,1,12,1); //��
		OLED_ShowChinese(28+30,0,2,12,1);//��
		OLED_ShowChinese(42+30,0,3,12,1);//ҩ
		OLED_ShowChinese(56+30,0,4,12,1);//��	
		OLED_ShowChinese(0+30-12,0+16,6,12,1); //��
		OLED_ShowChinese(14+30-12,0+16,7,12,1); //ȡ
		OLED_ShowChinese(28+30-12,0+16,8,12,1);//ҩ
		OLED_ShowChinese(42+30-12,0+16,9,12,1);//��
		OLED_ShowChinese(56+30-12+12,0+16,10,12,1);//��
		OLED_ShowChinese(70+30-12+12,0+16,11,12,1);//��
		OLED_Refresh_Gram();
		
		
	  atk_8266_config();
		USART3_RX_STA=0; //�����������

		
		OSInit(&err);		//��ʼ��UCOSIII
		OS_CRITICAL_ENTER();//�����ٽ���
		//������ʼ����
		OSTaskCreate(  (OS_TCB 	* )&StartTaskTCB,		//������ƿ�
									 (CPU_CHAR	* )"start task", 		//��������
									 (OS_TASK_PTR )start_task, 			//������
									 (void		* )0,					//���ݸ��������Ĳ���
									 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
									 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
									 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
									 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
									 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
									 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
									 (void   	* )0,					//�û�����Ĵ洢��
									 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
									 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
		OS_CRITICAL_EXIT();	//�˳��ٽ���	 
		OSStart(&err);  //����UCOSIII
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	
	//����wifi��ʼ������
	OSTaskCreate((OS_TCB 	* )&LedTaskTCB,		
								 (CPU_CHAR	* )"Led task", 		
                 (OS_TASK_PTR )led_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED_TASK_PRIO,     
                 (CPU_STK   * )&LED_TASK_STK[0],	
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//����ҩ��λ�ó�ʼ������
	OSTaskCreate((OS_TCB 	* )&PosTaskTCB,		
								 (CPU_CHAR	* )"position task", 		
                 (OS_TASK_PTR )pos_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )POS_TASK_PRIO,     	
                 (CPU_STK   * )&POS_TASK_STK[0],	
                 (CPU_STK_SIZE)POS_STK_SIZE/10,	
                 (CPU_STK_SIZE)POS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
				 
	//����������
	OSTaskCreate((OS_TCB 	* )&MainTaskTCB,		
								 (CPU_CHAR	* )"main task", 		
                 (OS_TASK_PTR )main_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MAIN_TASK_PRIO,     	
                 (CPU_STK   * )&MAIN_TASK_STK[0],	
                 (CPU_STK_SIZE)MAIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)MAIN_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);		
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����		
								 
//  OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
								 
	OS_CRITICAL_EXIT();	//�����ٽ���
								 
								 
							
}
/*************************   LED����ָʾ��   ******************************/

void led_task(void *p_arg)
{
	OS_ERR err;
//	CPU_SR_ALLOC();
//	p_arg = p_arg;
	
	while(1)
	{
//		OS_CRITICAL_ENTER();	//�����ٽ���
		LED2 = !LED2;
		LED3 = !LED3;

		if(SendTime == 0)
		{
			atk_8266_send_cmd("AT+CIPSEND=0,5","OK",50);
    	atk_8266_send_data("1min\n","OK",50);
			SendTime = 600;
		}
		else
		{
			SendTime--;
		}
		
		
//		atk_8266_send_cmd("AT+CIPSEND=0,6","OK",50);
//		atk_8266_send_data("ackACK","OK",100);
		
//		OS_CRITICAL_EXIT();	//�˳��ٽ���
		OS_TaskSuspend((OS_TCB*)&LedTaskTCB,&err);		//����wifi��ʼ������
	}
}

/************************   ҩ��λ�ó�ʼ��   *****************************/
void pos_task(void *p_arg)
{
	OS_ERR err;
//	CPU_SR_ALLOC();
//	p_arg = p_arg;

	while(1)
	{
//		OS_CRITICAL_ENTER();	//�����ٽ���
		boxPosition_Init(1,650,6400 *1000);
		positionNow = 1;
		positionReceive = 1;
//		OS_CRITICAL_EXIT();	//�˳��ٽ���
		OS_TaskSuspend((OS_TCB*)&PosTaskTCB,&err);		//����λ�ó�ʼ������
	}		
}


/************************   ��������   **********************************/
void main_task(void *p_arg)
{ 
	  
	u16 rlen=0;
	OS_ERR err;
	while(1)
	{
		if(USART3_RX_STA&0X8000)	
		{
			rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
			USART3_RX_BUF[rlen]=0;		//��ӽ����� 
			positionReceive = USART3_RX_BUF[rlen-1] - '0'; //���ַ�תΪ����
			position1 = USART3_RX_BUF[rlen-2] - '0';
			switch(positionReceive)
			{
				case 1: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=1; break;
				
				case 2: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=2; break;
				
				case 3: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=3; break;
				
				case 4: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=4; break;
				
				case 5: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=5;	break;
				
				case 6: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=6; break;
				
				case 7: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=7; break;
				
				case 8: OLED_ShowNum(74,17,positionReceive,1,12),OLED_Refresh_Gram(),
								boxPosition(positionReceive);positionReceive=0,positionNow=8; break;
			}

			USART3_RX_STA=0;
		}
		OSTaskResume((OS_TCB*)&LedTaskTCB,&err);	
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
		
	}
}


