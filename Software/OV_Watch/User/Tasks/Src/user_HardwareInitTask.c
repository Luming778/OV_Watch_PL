/* Private includes -----------------------------------------------------------*/

// includes
// sys
#include "usart.h"
#include "tim.h"
#include "stm32f4xx_it.h"
#include "delay.h"

// user
#include "user_TasksInit.h"
#include "HWDataAccess.h"
#include "version.h"

// bsp
#include "key.h"
#include "lcd.h"
#include "CST816.h"      // 触摸屏，暂不开
// #include "DataSave.h"    // EEPROM，暂不开

// ui
#include "lvgl.h"            // LVGL，暂不开
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "ui.h"

// APP SYS setting
// #include "ui_DateTimeSetPage.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern uint8_t HardInt_receive_str[25];
/* Private function prototypes -----------------------------------------------*/
void LED_Port_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  hardwares init task
  * @param  argument: Not used
  * @retval None
  */
void HardwareInitTask(void *argument)
{
	while(1)
	{
    vTaskSuspendAll();


    LED_Port_Init(); // LED - 测试用，暂不开
    // RTC Wake - 暂不开RTC
    // if(HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 2000, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
    // {
    //   Error_Handler();
    // }

    // usart start
    // HAL_UART_Receive_DMA(&huart1,(uint8_t*)HardInt_receive_str,25);
    // __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);

    // PWM Start - 背光需要
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

    // sys delay
    delay_init();

    // power - 暂不开
    HWInterface.Power.Init();

    // key
    //Key_Port_Init();

    // sensors - 全部暂不开（HW_USE_* 已置0）
    // uint8_t num = 3;
    // while(num && HWInterface.AHT21.ConnectionError)
    // {
    //   num--;
    //   HWInterface.AHT21.ConnectionError = HWInterface.AHT21.Init();
    // }

    // num = 3;
    // while(num && HWInterface.Ecompass.ConnectionError)
    // {
    //   num--;
    //   HWInterface.Ecompass.ConnectionError = HWInterface.Ecompass.Init();
    // }
    // if(!HWInterface.Ecompass.ConnectionError)
    //   HWInterface.Ecompass.Sleep();

    // num = 3;
    // while(num && HWInterface.Barometer.ConnectionError)
    // {
    //   num--;
    //   HWInterface.Barometer.ConnectionError = HWInterface.Barometer.Init();
    // }

    // num = 3;
    // while(num && HWInterface.IMU.ConnectionError)
    // {
    //   num--;
    //   HWInterface.IMU.ConnectionError = HWInterface.IMU.Init();
    // }

    // num = 3;
    // while(num && HWInterface.HR_meter.ConnectionError)
    // {
    //   num--;
    //   HWInterface.HR_meter.ConnectionError = HWInterface.HR_meter.Init();
    // }
    // if(!HWInterface.HR_meter.ConnectionError)
    //   HWInterface.HR_meter.Sleep();


    // EEPROM - 暂不开
    // EEPROM_Init();
    // ...

    // BLE - 暂不开
    // HWInterface.BLE.Init();
    // HWInterface.BLE.Disable();

    // touch - 
    CST816_GPIO_Init();
    CST816_RESET();

    // ========== LCD 初始化和测试 ==========
    LCD_Init();
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    delay_ms(10);
    LCD_Open_Light();
    LCD_Set_Light(80);  // 亮度80%
    LCD_ShowString(60, 120, (uint8_t*)"LCD OK!", WHITE, BLACK, 24, 0);
    LCD_ShowString(34, 160, (uint8_t*)"OV-Watch V2.4.4", WHITE, BLACK, 16, 0);
    delay_ms(1000);
    LCD_Fill(0, LCD_H/2-24, LCD_W, LCD_H/2+49, BLACK);
    // ======================================

    // LVGL - 暂不开
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    ui_init();

    xTaskResumeAll();
		vTaskDelete(NULL);
	}
}
