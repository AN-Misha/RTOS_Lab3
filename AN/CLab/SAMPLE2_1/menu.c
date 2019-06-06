/************************************************************************************
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: menu.с
  Назначение: Формирование меню
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#include "menu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "uart_io.h"
extern xQueueHandle stdin_queue;
extern xQueueHandle stdout_queue;

// Выбранный пункт меню
uint8_t U_MENU_Item;

// Строка для представления времени
uint8_t U_MENU_Time_String[8];  

// Указатель на сообщение, выводимое в качестве пункта меню
char* U_MENU_Message;

// Подготовить строку для выбранного пункта меню
char* U_MENU_Prepare_Item(uint8_t item); 

// Инициализировать меню
void U_MENU_Init (void)
{
  	
	// Выбрать начальный пункт меню
	U_MENU_Item = 0;
	// Отобразить начальный пункт меню
	 // Подготовить строку для выбранного пункта меню
     U_MENU_Message = U_MENU_Prepare_Item (U_MENU_Item); 			
	// Вывести на ЖКИ строку пункта меню
	if (U_MENU_Message)
	U_MLT_Put_String (U_MENU_Message, 3);
}

// Задача по работе с меню
void U_MENU_Task_Function (void)
{
	/*Переменная, которая будет хранить значение счётчика квантов*/
	portTickType xLastTime;
	uint8_t flag_button;
	//Инициализация переменной, хранящей значение счётчика квантов
	xLastTime=xTaskGetTickCount();
	flag_button=OFF;
	while(1)
	{
		// Пауза 20мс, чтобы по-напрасну не грузить процессор
    vTaskDelay (20);  
    // Проверка на нажатие кнопки
	if ((U_BTN_Read_Button()==ON) && (flag_button==OFF)&& ((xTaskGetTickCount()-xLastTime)>=DrebezgTime))
	{
	  
	  // Если нажата кнопка установить флаг нажатой кнопки
	  flag_button=ON;
	  xLastTime=xTaskGetTickCount(); // Получение текущего значения счётчика квантов	
	}	
	// Проверка на отпускание кнопки с учетом времени антидребезга
	if ((flag_button==ON)&&(U_BTN_Read_Button()==OFF)&& ((xTaskGetTickCount()-xLastTime)>=DrebezgTime))
	{
		// Перейти к следующему пункту меню, а если прошли все, то к начальному
	  if (++ U_MENU_Item == U_MENU_ITEM_COUNT)
		 U_MENU_Item = 0;
	  // Подготовить строку для выбранного пункта меню
	  U_MENU_Message = U_MENU_Prepare_Item (U_MENU_Item); 			
	  // Вывести на ЖКИ строку пункта меню
	  if (U_MENU_Message)
	  U_MLT_Put_String (U_MENU_Message, 3); 
		// Если кнопка отжата установить флаг отжатой кнопки
		flag_button=OFF;
		xLastTime=xTaskGetTickCount(); // Получение текущего значения счётчика квантов	
	}
    
		
	}
}

// Подготовить строку для выбранного пункта меню
char* U_MENU_Prepare_Item (uint8_t item) 
{
	switch (item)
	{
		// Пункт 0;
		case 0:
			
			return "\xCF\xF3\xED\xEA\xF2 0";  // Пункт 0
		
		// Пункт 1;
		case 1:
			
			return "\xCF\xF3\xED\xEA\xF2 1";  // Пункт 1

		// Пункт 2;
		case 2:
			
			return "\xCF\xF3\xED\xEA\xF2 2";  // Пункт 2

		// Пункт 3;
		case 3:
		
			return "\xCF\xF3\xED\xEA\xF2 3";  // Пункт 3
		
		default:
			
			return 0;
	}
}

// Задача по выводу бегущей строки на ЖКИ
void U_MENU_Running_String_Task_Function (void)
{
	uint32_t k = 0;
	
	// Строка, выводимая на индикатор 
//	const char s[] =  "\xD0\xE0\xE1\xEE\xF2\xE0 \xF1 \xEA\xED\xEE\xEF\xEA\xE0\xEC\xE8   ";  // "Работа с кнопками"
	const char s[]="Work with buttons";
	while(1)
	{
		// Пауза в тиках системного таймера. Здесь 1 тик = 1 мс
    vTaskDelay(1000);
		
		// Вывести строку c прокруткой
		// s - выводимая строка
		// 6 - номер строки на дисплее
		// k++ - смещение строки на количество символов
		U_MLT_Scroll_String (s, 6, k++);
	}
}
//Задача по выводу на ЖКИ информации полученной с терминала по UART
void U_MENU_Output (void)
{ 
	 /* Буфер для полученного сообщения по UART */
    char Message_UART[16 + 1]; 
	while(1)
	{	
	 /* Ожидание ввода сообщения по UART */
   scanf("%16s", Message_UART);
	 // Передача полученного ссобщения на дисплей ЖКИ в 4-ю строку
	  U_MLT_Put_String (Message_UART, 4); 
	 // Передача в стандартный поток вывода, что сообщение успешно передано на ЖКИ
	 printf("Transmit Message To LCD - Success\r\n");
	}
}
// Задача - приветствие
void U_Privet(void)
{
	// Отправка сообщения приветствия
	printf("Work with Queues\r\n");
	// Удаление задачи
	vTaskDelete( NULL );
}
// Задача для работы с выводом по UART
void Task_output(void)
{
	  static uint8_t buffer;
  while(1)
	{
		if (xQueueReceive(stdout_queue, &buffer, portMAX_DELAY) == pdPASS)
    {
				  /* Ожидание флага освобождения буфера передачи (TXFE) */
					while (UART_GetFlagStatus(UART_IO, UART_FLAG_TXFE) != SET);
					// Отправка одного символа по UART
					UART_SendData(UART_IO, buffer);
		}
				
  }
}
