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
/* Подключение функций управления АЦП */
#include <MDR32F9Qx_adc.h>

/* Подключение заголовочного файла, содержащего платформо-зависимые 
   макроопределения */
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

void ADC_init(void)
{
	ADC_InitTypeDef ADC_InitStructure; //Общая инициализацинная структура подсистемы АЦП
	ADCx_InitTypeDef ADC1_InitStructure; //Инициализацинная структура для АЦП1
	// Структура для инициализации портов	
	PORT_InitTypeDef PortInitStructure;
    /* Инициализация выводов АЦП  */
    /* Разрешение тактирования портов с выводами АЦП (PORTD)*/
   RST_CLK_PCLKcmd (RST_CLK_PCLK_ADC | RST_CLK_PCLK_PORTD, ENABLE);
   // Cделать вывод U_ADC_U_PIN=PD7 аналоговым:
	PORT_StructInit (&PortInitStructure);
	PortInitStructure.PORT_Pin = U_ADC_U_PIN;
	PortInitStructure.PORT_MODE = PORT_MODE_ANALOG;
	PORT_Init (MDR_PORTD, &PortInitStructure);
	
    /* Заполнение общих полей структуры ADC_InitStructure */
	/*Режим независимого запуска двух АЦП*/
	ADC_InitStructure.ADC_SynchronousMode = ADC_SyncMode_Independent;
	/*Задержка между запусками двух АЦП*/
	ADC_InitStructure.ADC_StartDelay = 0;
	// Отключение встроенного температурного датчика
	ADC_InitStructure.ADC_TempSensor = ADC_TEMP_SENSOR_Disable;
	ADC_InitStructure.ADC_TempSensorAmplifier = ADC_TEMP_SENSOR_AMPLIFIER_Disable;
	ADC_InitStructure.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Disable;
	//Отключение внутреннего датчика опорного напряжения
	ADC_InitStructure.ADC_IntVRefConversion = ADC_VREF_CONVERSION_Disable;
	ADC_InitStructure.ADC_IntVRefTrimming = 0;
	ADC_Init (&ADC_InitStructure);
     
		 /* Заполнение полей структуры ADC1_InitStructure */
		 // Источник тактирования для АЦП от частоты ЦП
	ADC1_InitStructure.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU;
	// Делитель частоты 16, fadc=fcpu/16=80МГц/16=5 МГц, максимально возможная частота 14МГц
	ADC1_InitStructure.ADC_Prescaler = ADC_CLK_div_8;
	// Режим одиночного преобразования
	ADC1_InitStructure.ADC_SamplingMode = ADC_SAMPLING_MODE_SINGLE_CONV;
	// Автоматическое переключение каналов выключено
	ADC1_InitStructure.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
	// Канал для оцифровки=7
	ADC1_InitStructure.ADC_ChannelNumber = ADC_CH_ADC7;
	// В автоматичиском режиме количество используемых каналов
	ADC1_InitStructure.ADC_Channels = 0;
	// В автоматическом режиме задержка между переключением каналов
	ADC1_InitStructure.ADC_DelayGo = 7;
	// Контроль за уровнем напряжения выключен
	ADC1_InitStructure.ADC_LevelControl = ADC_LEVEL_CONTROL_Disable;
	ADC1_InitStructure.ADC_LowLevel = 0;
	ADC1_InitStructure.ADC_HighLevel = 0;
	// Источник опорного напряжения - внутренний
	ADC1_InitStructure.ADC_VRefSource = ADC_VREF_SOURCE_INTERNAL;
	// Источник опорного напряжения без температурной компенсации
	ADC1_InitStructure.ADC_IntVRefSource = ADC_INT_VREF_SOURCE_INEXACT;
	ADC1_Init (&ADC1_InitStructure);

    /* Разрешение работы АЦП1 */
    ADC1_Cmd (ENABLE);
}








