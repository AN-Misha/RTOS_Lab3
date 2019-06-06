/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "MLT/mlt_lcd.h"
#include "init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Подключение функций стандартного ввода/вывода (printf и scanf) */
#include <stdio.h>
//#include <heap_2.h>
#include "ParseCommands.h"


#define MESSAGE_MAX_SIZE 100
#define PAGE_NUM 10
#define PAGE_SIZE 20;



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
// Функция обработчика прерывания по UART2

uint8_t page_counter = 0;


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
// Задача бездействие
void vApplicationIdleHook (void)
{
}


void U_MENU_Output (void * pvParameters)
{ 
	 /* Буфер для полученного сообщения по UART */
  char Message_UART[MESSAGE_MAX_SIZE + 1];\
	PARSEresulte parseStruct;
	while(1)
	{
		
	 /* Ожидание ввода сообщения по UART */
    scanf("%50s", Message_UART);
	 // Передача полученного ссобщения на дисплей ЖКИ в 4-ю строку
	  //U_MLT_Put_String (Message_UART, 4); 
	 // Передача в стандартный поток вывода, что сообщение успешно передано на ЖКИ
	  getDataFromUART(&parseStruct, Message_UART);
		switch(parseStruct.command_name)
		{
			case PUTC:
				printf("PUTC!!");
			break;
		}
		
	}
}



int main(void)
{
	// Инициализация тактирования МК на частоте 80МГц
	init ();
	
	// Инициализация кнопки
	U_BTN_Init ();
	// Инициализация ЖКИ
	U_MLT_Init ();	

	/* Вызов функции инициализации UART для стандартного потока ввода/вывода */
  uart_io_init();

	// Создание очередей для потока ввода/вывода
	stdin_queue = xQueueCreate(STDIN_QUEUE_SIZE, sizeof(char));
  stdout_queue = xQueueCreate(STDOUT_QUEUE_SIZE, sizeof(char));
	   
	//Создание задачи по работе с меню 
    //Создание задачи по выводу на ЖКИ информации полученной с терминала по UART
  xTaskCreate(U_MENU_Output, (char *) "Task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
		 //Создание задачи приветствия
	// Запуск планировщика задач 	
  vTaskStartScheduler();
}

