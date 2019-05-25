/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "init.h"
#include "menu.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* Подключение функций стандартного ввода/вывода (printf и scanf) */
#include <stdio.h>

/* На отладочной плате модуль UART микроконтроллера подключен 
   к преобразователю UART - RS-232.
   Персональный компьютер подключается к разъему RS-232 отладочной
   платы через преобразователь RS-232 - USB.
   На персональном компьютере в программе терминале выбирается 
   COM порт с параметрами: 115200 8N1 (скорость передачи 115200 бод, 
   8 бит данных, без контроля четности и 1 стоп бит).
   В данном проекте ввод и вывод символов производится
   с помощью стандартной библиотеки ввода/вывода (stdio.h).
   Для использования стандартной библиотеки ввода/вывода
   переопределены функции stdin_getchar и stdout_putchar 
   для приема и передачи символов по UART (uart_io.h, uart_io.c).
*/
#include "uart_io.h"

/* Объявить переменную-дескриптор очереди. Эта переменная
* будет использоваться для ссылки на очередь после ее создания. */
 xQueueHandle stdin_queue,stdout_queue;
 /*Объявить переменную - семафор */
xSemaphoreHandle SemaphoreDMA;
 //2 Буфера для хранения результатов АЦП
uint16_t ADC_Buffer_ping [ADC_BUFFER_SIZE];
uint16_t ADC_Buffer_pong [ADC_BUFFER_SIZE];
//Переменная флаг пинг-понга
uint32_t ping_pong=PING;
/* Структура для инициализации DMA */
extern DMA_CtrlDataInitTypeDef DMA_InitStructure;
	/* Структура для инициализации конфигурации канала DMA */
extern	DMA_ChannelInitTypeDef DMA_Channel_InitStructure;
// Функция обработчика прерывания по UART2
void UART2_IRQHandler(void)
{
	static uint8_t ReceiveByte=0x00; // данные для приема
	static BaseType_t xHigherPriorityTaskWoken;
	if (UART_GetITStatusMasked(MDR_UART2, UART_IT_RX) == SET)
	//проверка установки флага прерывания по окончании приема данных
	{
	UART_ClearITPendingBit(MDR_UART2, UART_IT_RX);//очистка флага прерывания по приему данных
	}
	ReceiveByte=UART_ReceiveData(UART_IO); // записать принятый данный в статическую переменную
  //Отправить принятый байт в очередь приема данных
	xQueueSendToBackFromISR(stdin_queue, 
        &ReceiveByte, &xHigherPriorityTaskWoken);
	// Принудительно переключить контекст, вызов планировщика для передачи управления другой задаче
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/* Функция обработчика прерывания по Timer1 */
void Timer1_IRQHandler(void)
{
	  // Запуск АЦП1 для однократного преобразования 
	  ADC1_Start();
    /* Сброс флага прерывания по переполнению таймера */
    TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
}
/*Функция обработчика прерывания по DMA*/
void DMA_IRQHandler(void)
{

	static BaseType_t xHigherPriorityTaskWoken;
    /* Перенастройка канала DMA */
	/* Смена адреса массива приемника DMA с пинг на понг и наоборот */
  if(ping_pong==PING)
	{		
	 DMA_InitStructure.DMA_DestBaseAddr  =(uint32_t) &ADC_Buffer_pong;
		ping_pong=PONG;
  }  
  else
	{		
	 DMA_InitStructure.DMA_DestBaseAddr  =(uint32_t) &ADC_Buffer_ping;
		ping_pong=PING;
  } 
    // Инициализировать канал	DMA 
		DMA_Init (DMA_Channel_ADC1, &DMA_Channel_InitStructure);
	// Отдать семафор на запуск задачи-обработчик буфера оцифрованных значений 
  	xSemaphoreGiveFromISR(SemaphoreDMA, &xHigherPriorityTaskWoken);
	// Принудительно переключить контекст, вызов планировщика для передачи управления другой задаче
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
// Задача бездействие
void vApplicationIdleHook (void)
{
}


int main(void)
{
	// Инициализация тактирования МК на частоте 80МГц
	init ();
	
	// Инициализация кнопки
	U_BTN_Init ();	
	/* Вызов функции инициализации UART для стандартного потока ввода/вывода */
  uart_io_init();
		/*Инициализация таймера TIMER1 */
  // Timer1 работает с периодом 100мкс(частота 10кГц)
	// По переполнению таймера Timer1 формируется запрос на прерывние
	// Приоритет прерывания выше, чем у планировщика задач
	// Функция инициализации таймера находится в модуле timer.c
	Timer1_init();
	/*Инициализация АЦП1*/
	// АЦП 12-разрядный
  // Вывод МК PD7 настраивается как аналоговый
  // Тактирование АЦП1 с частотой 5МГц 
  // По документации макс. частота для АЦП1 14МГц
  // Установлен режим одиночного преобразования
  // Выбран 7-й канал для оцифровки АЦП1 
  // Функция инициализации АЦП1 находится в модуле adc.c

	ADC_init();
	/*Инициализация DMA*/
	// Источник данных DMA - регистр данных АЦП1 
	// Приемник данных DMA - массив ADC_Buffer_ping 
	// Количество циклов запуска DMA – размер массива-буфера 
	// измерений ADC_BUFFER_SIZE
	// Не увеличиваем адрес источника данных после каждой пересылки
	// Увеличение адреса приемника на 2 байта (полслова) каждую пересылку 
	// Базовый режим работы DMA
	// Инициализировать канал DMA для работы с АЦП ADC1 DMA_Channel_ADC1
	// Разрешить работу DMA с каналом АЦП1
  // Функция инициализации DMA находится в модуле dma.c
	DMA_init();

	// Инициализация ЖКИ
	U_MLT_Init ();	
	// Инициализация меню
	U_MENU_Init ();

  // Создание семафора для синхронизации задачи обработчика буфера оцифрованных значений
	vSemaphoreCreateBinary(SemaphoreDMA);
	// Создание очередей для потока ввода/вывода
	stdin_queue = xQueueCreate(STDIN_QUEUE_SIZE, sizeof(char));
  stdout_queue = xQueueCreate(STDOUT_QUEUE_SIZE, sizeof(char));
	   
	  //Создание задачи по работе с меню 
    xTaskCreate(U_MENU_Task_Function,(char *) "Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	  //Создание задачи по выводу бегущей строки на ЖКИ 
    xTaskCreate(U_MENU_Running_String_Task_Function, (char *) "Task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    //Создание задачи по выводу на ЖКИ информации полученной с терминала по UART
     xTaskCreate(U_MENU_Output, (char *) "Task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
		 //Создание задачи приветствия
     xTaskCreate(U_Privet, (char *) "Task4", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);
		 //Создание задачи для работы с выводом по UART
     xTaskCreate(Task_output, (char *) "Task5", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
		 //Создание задачи обработчика буфера оцифрованных значений Task_DSP
		// Функция задачи Task_DSP находится в модуле menu.c
     xTaskCreate(Task_DSP, (char *) "Task6", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
	// Запуск планировщика задач 	
    vTaskStartScheduler();
}

