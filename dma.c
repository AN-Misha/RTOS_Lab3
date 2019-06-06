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
/* Подключение функций управления контроллером ПДП */
#include <MDR32F9Qx_dma.h>

/* Подключение заголовочного файла, содержащего платформо-зависимые 
   макроопределения */
#include "dma.h"
#include "adc.h"
//2 Буфера для хранения результатов АЦП
extern uint16_t ADC_Buffer_ping [];
extern uint16_t ADC_Buffer_pong [];
	/* Структура для инициализации DMA */
	DMA_CtrlDataInitTypeDef DMA_InitStructure;
	/* Структура для инициализации конфигурации канала DMA */
	DMA_ChannelInitTypeDef DMA_Channel_InitStructure;

void DMA_init(void)
{

	// Разрешить тактирование DMA и обязательно интерфейсов SSP1 и SSP2
	RST_CLK_PCLKcmd (RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 |
	RST_CLK_PCLK_SSP2, ENABLE);
	// Запретить все прерывания, в том числе от SSP1 и SSP2
//	NVIC->ICPR[0] = 0xFFFFFFFF;
//	NVIC->ICER[0] = 0xFFFFFFFF;
	// Сбросить все настройки DMA
	DMA_DeInit();
	DMA_StructInit (&DMA_Channel_InitStructure);
	/* Настройка первичной конфигурации канала DMA */
  /* Источник данных DMA - регистр данных АЦП1 */
	DMA_InitStructure.DMA_SourceBaseAddr = (uint32_t)(&(MDR_ADC->ADC1_RESULT));
	/* Приемник данных DMA - массив ADC_Buffer_ping */
	DMA_InitStructure.DMA_DestBaseAddr = (uint32_t) &ADC_Buffer_ping;
	/* Задание размер буффера (массива) результата измерений ADC_BUFFER_SIZE
	Количество циклов запуска DMA*/
	DMA_InitStructure.DMA_CycleSize = ADC_BUFFER_SIZE;
	/* Не увеличиваем адрес источника данных */
	DMA_InitStructure.DMA_SourceIncSize = DMA_SourceIncNo;
	/* Увеличение адреса приемника на 2 байта (полслова) каждую пересылку */
	DMA_InitStructure.DMA_DestIncSize = DMA_DestIncHalfword;
	/* Выравнивание данных по 2 байта */
	DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_HalfWord;
	/* Однократная передача полного цикла */
	DMA_InitStructure.DMA_NumContinuous = DMA_Transfers_1;
	DMA_InitStructure.DMA_SourceProtCtrl = DMA_SourcePrivileged;
	DMA_InitStructure.DMA_DestProtCtrl = DMA_DestPrivileged;
	// Базовый режим работы DMA
	DMA_InitStructure.DMA_Mode = DMA_Mode_Basic;
	// Настройка канала DMA
	// Задать структуру канала DMA
	DMA_Channel_InitStructure.DMA_PriCtrlData = &DMA_InitStructure;
	DMA_Channel_InitStructure.DMA_Priority = DMA_Priority_Default;
	// Снятие запретов на одиночные запросы к DMA
	DMA_Channel_InitStructure.DMA_UseBurst = DMA_BurstClear;
	DMA_Channel_InitStructure.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
	// Инициализировать канал	DMA 
	DMA_Init (DMA_Channel_ADC1, &DMA_Channel_InitStructure);
	//Снятие запретов на запросы к DMA со стороны АЦП1 (заплатка к МК Миландр)
	MDR_DMA->CHNL_REQ_MASK_CLR = 1 << DMA_Channel_ADC1;
	MDR_DMA->CHNL_USEBURST_CLR = 1 << DMA_Channel_ADC1;
	// Задать приоритет аппаратного прерывания от DMA
  NVIC_SetPriority (DMA_IRQn, 3);
	// Разрешить прерывания от DMA
	NVIC_EnableIRQ (DMA_IRQn);
	// Разрешить работу DMA с каналом АЦП1
	DMA_Cmd (DMA_Channel_ADC1, ENABLE);
    

}








