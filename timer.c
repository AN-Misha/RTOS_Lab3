/**
  ******************************************************************************
  * \file    adc.c
   * \brief   Инициализация АЦП
  *          
  ******************************************************************************
*/

/* Подключение заголовочного файла с макроопределениями 
   регистров специальных функций К1986ВЕ92QI(MDR32F9Q2I) */
#include <MDR32Fx.h>
/* Подключение заголовочного файла с конфигурацией 
   Standard Peripheral Library (SPL) для К1986ВЕ92QI(MDR32F9Q2I) */
#include <MDR32F9Qx_config.h>
/* Подключение функций управления портами */
#include <MDR32F9Qx_port.h>
/* Подключение функций управления тактированием */
#include <MDR32F9Qx_rst_clk.h>
/* Подключение функций управления таймерами */
#include <MDR32F9Qx_timer.h>

/* Подключение заголовочного файла, содержащего платформо-зависимые 
   макроопределения */
#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* Структура для инициализации таймера */
TIMER_CntInitTypeDef        TIM_CntInit;
// Функция инициализация таймера 1
void Timer1_init(void)
{
/* Настройка таймера.
       Параметры таймера Timer1: 
       HCLK = 80 MHz, Prescaler = 7 -> Clock = HCLK/(Prescaler + 1) = 10 MHz,
       Period = 999 -> Frequency = Clock/(Period + 1) = 10 kHz */
    /* Разрешение тактирования периферийного модуля таймера 1 */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1,ENABLE);
	  /* Сброс таймера Timer1 */
    TIMER_DeInit(MDR_TIMER1);
    
    /* Разрешение тактирования Timer1 */
    TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv1);
    
    /* Заполнение полей структуры TIM_CntInit значениями по умолчанию */
    TIMER_CntStructInit(&TIM_CntInit);

    /* Заполнение полей структуры TIM_CntInit */
    /* Настройка предделителя */
    TIM_CntInit.TIMER_Prescaler                = 7;
    /* Настройка периода */
    TIM_CntInit.TIMER_Period                   = 999;
    /* Источник тактиования - внутренний генератор */
    TIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;
    /* Счет "вверх" */
    TIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;
    /* Немедленное обновление счетчика по переполнению */
    TIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;
    
    /* Инициализация Timer1 */
    TIMER_CntInit(MDR_TIMER1, &TIM_CntInit);
    
  // Разрешить прерывания по переполнению таймера 
  TIMER_ITConfig (MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);
  // Задать приоритет выше, чем у планировщика задач. 
  NVIC_SetPriority (Timer1_IRQn,3);
	
  // Разрешить прерывания от Timer1
  NVIC_EnableIRQ (Timer1_IRQn); 
     /* Разрешение работы Timer1 */
    TIMER_Cmd(MDR_TIMER1, ENABLE);

	


 
}








