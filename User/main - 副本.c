
/****************           云药盒1.0版本       ************************/

#include "sys.h"
#include "delay.h"
#include "usart.h"

/**************             用户驱动头文件        ***********************/

#include "led.h"
#include "usart3.h"
#include "wifi.h"
#include "motor.h"
#include "reedswitch.h"
#include "oled.h"

/**************           uCOSIII 系统头文件      ***********************/

#include "includes.h"
#include "os_app_hooks.h"

/****************    UCOSIII中以下优先级用户程序不能使用   ***************/

/*	将这些优先级分配给了UCOSIII的5个系统内部任务
		优先级0：中断服务服务管理任务 OS_IntQTask()
		优先级1：时钟节拍任务 OS_TickTask()
		优先级2：定时任务 OS_TmrTask()
		优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
		优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
*/

/*********************           任务定义            *********************/


/*****************************   开始任务  *******************************/

//任务优先级 
#define START_TASK_PRIO		10
//任务堆栈大小	
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

/**********************    LED指示灯    *****************************/

//任务优先级
#define LED_TASK_PRIO		4
//任务堆栈大小	
#define LED_STK_SIZE 		128
//任务控制块
OS_TCB LedTaskTCB;
//任务堆栈	
CPU_STK LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *p_arg);

/************************  药盒位置初始化  ***************************/

//任务优先级
#define POS_TASK_PRIO		5
//任务堆栈大小	
#define POS_STK_SIZE 		128
//任务控制块
OS_TCB PosTaskTCB;
//任务堆栈	
CPU_STK POS_TASK_STK[POS_STK_SIZE];
//任务函数
void pos_task(void *p_arg);

/******************************  主任务  *****************************/

//任务优先级
#define MAIN_TASK_PRIO		6
//任务堆栈大小
#define MAIN_STK_SIZE		1200
//任务控制块
OS_TCB	MainTaskTCB;
//任务堆栈
__align(8) CPU_STK	MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数
void main_task(void *p_arg);


/*****************************  变量  ******************************/

u16 SendTime; //发送数据时间 100ms*SendTime = 单片机发送数据间隔时间
// SendTime = 60 ；6s钟一次


/*****************************  主函数  ******************************/

int main(void)
{
  	OS_ERR err;
  	CPU_SR_ALLOC();
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
		delay_init(168);  
		ReedSwitchInit();
		LED_Init();
		uart_init(115200);//初始化串口波特率为115200
		usart3_init(115200); 
		Motor_Init();
		EN = 1; //关闭电机电源
		
  	SendTime = 600;
	
//	USART3_RX_STA=0; //允许接收数据
//	positionNow = 1; //药盒初始位置为1
	
		OLED_Init();	
		OLED_ShowChinese(0+30,0,0,12,1); //智
		OLED_ShowChinese(14+30,0,1,12,1); //能
		OLED_ShowChinese(28+30,0,2,12,1);//云
		OLED_ShowChinese(42+30,0,3,12,1);//药
		OLED_ShowChinese(56+30,0,4,12,1);//盒	
		OLED_ShowChinese(0+30-12,0+16,6,12,1); //请
		OLED_ShowChinese(14+30-12,0+16,7,12,1); //取
		OLED_ShowChinese(28+30-12,0+16,8,12,1);//药
		OLED_ShowChinese(42+30-12,0+16,9,12,1);//：
		OLED_ShowChinese(56+30-12+12,0+16,10,12,1);//盒
		OLED_ShowChinese(70+30-12+12,0+16,11,12,1);//号
		OLED_Refresh_Gram();
		
		
	  atk_8266_config();
		USART3_RX_STA=0; //允许接收数据

		
		OSInit(&err);		//初始化UCOSIII
		OS_CRITICAL_ENTER();//进入临界区
		//创建开始任务
		OSTaskCreate(  (OS_TCB 	* )&StartTaskTCB,		//任务控制块
									 (CPU_CHAR	* )"start task", 		//任务名字
									 (OS_TASK_PTR )start_task, 			//任务函数
									 (void		* )0,					//传递给任务函数的参数
									 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
									 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
									 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
									 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
									 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
									 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
									 (void   	* )0,					//用户补充的存储区
									 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
									 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
		OS_CRITICAL_EXIT();	//退出临界区	 
		OSStart(&err);  //开启UCOSIII
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
	
	//创建wifi初始化函数
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
				 
	//创建药盒位置初始化函数
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
				 
	//创建主任务
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
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务		
								 
//  OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
								 
	OS_CRITICAL_EXIT();	//进入临界区
								 
								 
							
}
/*************************   LED工作指示灯   ******************************/

void led_task(void *p_arg)
{
	OS_ERR err;
//	CPU_SR_ALLOC();
//	p_arg = p_arg;
	
	while(1)
	{
//		OS_CRITICAL_ENTER();	//进入临界区
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
		
//		OS_CRITICAL_EXIT();	//退出临界区
		OS_TaskSuspend((OS_TCB*)&LedTaskTCB,&err);		//挂起wifi初始化任务
	}
}

/************************   药盒位置初始化   *****************************/
void pos_task(void *p_arg)
{
	OS_ERR err;
//	CPU_SR_ALLOC();
//	p_arg = p_arg;

	while(1)
	{
//		OS_CRITICAL_ENTER();	//进入临界区
		boxPosition_Init(1,650,6400 *1000);
		positionNow = 1;
		positionReceive = 1;
//		OS_CRITICAL_EXIT();	//退出临界区
		OS_TaskSuspend((OS_TCB*)&PosTaskTCB,&err);		//挂起位置初始化任务
	}		
}


/************************   主任务函数   **********************************/
void main_task(void *p_arg)
{ 
	  
	u16 rlen=0;
	OS_ERR err;
	while(1)
	{
		if(USART3_RX_STA&0X8000)	
		{
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;		//添加结束符 
			positionReceive = USART3_RX_BUF[rlen-1] - '0'; //把字符转为整形
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
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
		
	}
}


