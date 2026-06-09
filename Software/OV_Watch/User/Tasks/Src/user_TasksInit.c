/* Private includes -----------------------------------------------------------*/
//includes
#include "user_TasksInit.h"
//sys
#include "stdio.h"
#include "lcd.h"
//gui
#include "ui.h"
//tasks
#include "user_HardwareInitTask.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Timers --------------------------------------------------------------------*/
osTimerId_t IdleTimerHandle;


/* Tasks ---------------------------------------------------------------------*/
// Hardwares initialization
osThreadId_t HardwareInitTaskHandle;
const osThreadAttr_t HardwareInitTask_attributes = {
  .name = "HardwareInitTask",
  .stack_size = 128 * 40,
  .priority = (osPriority_t) osPriorityNormal+1,
};

//LVGL Handler task
osThreadId_t LvHandlerTaskHandle;
const osThreadAttr_t LvHandlerTask_attributes = {
  .name = "LvHandlerTask",
  .stack_size = 128 * 30,
  .priority = (osPriority_t) osPriorityNormal,
};
//WDOG Feed task

//Idle Enter Task

//Stop Enter Task

//Key task
/*
osThreadId_t KeyTaskHandle;
const osThreadAttr_t KeyTask_attributes = {
  .name = "KeyTask",
  .stack_size = 128 * 1,
  .priority = (osPriority_t) osPriorityNormal,
};
*/

//ScrRenew task
/*
osThreadId_t ScrRenewTaskHandle;
const osThreadAttr_t ScrRenewTask_attributes = {
  .name = "ScrRenewTask",
  .stack_size = 128 * 10,
  .priority = (osPriority_t) osPriorityLow1,
};
*/
//SensorDataRenew task

//HRDataRenew task

//ChargPageEnterTask

//messagesendtask

//MPUCheckTask

//DataSaveTask


/* Message queues ------------------------------------------------------------*/
//Key message


/* Private function prototypes -----------------------------------------------*/
static void LvHandlerTask(void *argument);
//void WDOGFeedTask(void *argument);

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void User_Tasks_Init(void)
{
  /* add mutexes, ... */

  /* add semaphores, ... */

  /* start timers, add new ones, ... */

	//IdleTimerHandle = osTimerNew(IdleTimerCallback, osTimerPeriodic, NULL, NULL);
	//osTimerStart(IdleTimerHandle,100);//100ms

  /* add queues, ... */


	/* add threads, ... */
  HardwareInitTaskHandle  = osThreadNew(HardwareInitTask, NULL, &HardwareInitTask_attributes);
  LvHandlerTaskHandle  = osThreadNew(LvHandlerTask, NULL, &LvHandlerTask_attributes);
	//KeyTaskHandle 			 = osThreadNew(KeyTask, NULL, &KeyTask_attributes);
	//ScrRenewTaskHandle   = osThreadNew(ScrRenewTask, NULL, &ScrRenewTask_attributes);
	

  /* add events, ... */


	/* add  others ... */


}


/**
  * @brief  FreeRTOS Tick Hook, to increase the LVGL tick
  * @param  None
  * @retval None
  */
//void TaskTickHook(void);


/**
  * @brief  LVGL Handler task, to run the lvgl
  * @param  argument: Not used
  * @retval None
  */
static void LvHandlerTask(void *argument)
{
  while(1)
  {
    lv_task_handler();
    osDelay(5);
  }
}





