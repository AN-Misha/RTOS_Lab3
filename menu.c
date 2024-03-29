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
#include "adc.h"
#include "timer.h"
extern xQueueHandle stdin_queue;
extern xQueueHandle stdout_queue;
extern xSemaphoreHandle SemaphoreDMA;

typedef enum State
{
    freq_meas,
    fft
}State;

typedef enum Signal
{
    s_sin,
    s_ramp,
    s_square
}Signal;


Signal signal;
#define FFT_SIZE 32
#define FFT_UPSCALE 4
#define ARM_MATH_CM3
#include <math.h>
#include "arm_math.h"

q15_t fft_res[FFT_SIZE*2];
arm_rfft_instance_q15 S;
uint16_t fft_amp[FFT_SIZE];
float fft_amp_norm[FFT_SIZE/2];

static const uint8_t image_ramp[] = 
{
	0xFF, 0x40, 0x20, 0x10, 0x08, 0x04,  0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00,  0x00, 0x00 ,      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00,
	0xFF, 0x80,0x80, 0x80, 0x80, 0x80, 0x80,  0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x82, 0x84,  0x88, 0x90,       0xA0, 0xA0, 0x90, 0x88, 0x84, 0x82,  0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 
};

static const uint8_t image_square[] = 
{
	0xFF, 0x00, 0x00, 0x00, 0x00, 0xF8,  0x08, 0x08, 0x08, 0x08, 0xF8, 0x00, 0x00, 0x00,  0x00, 0x00 ,      0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,  0x08, 0x08, 0x08, 0x08, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xA0, 0xA0, 0xA0, 0xA0, 0xBF,  0x80, 0x80, 0x80, 0x80, 0xBF, 0xA0, 0xA0, 0xA0,  0xA0, 0xA0,       0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xBF,  0x80, 0x80, 0x80, 0x80, 0xBF, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0
};

static const uint8_t image_sin[] = 
{
	0xFF, 0x20, 0x20, 0x10, 0x10, 0x08,  0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x80,  0x00, 0x00 ,      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0x80,0x80, 0x80, 0x80, 0x80, 0x80,  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81,  0x82, 0x84,       0x84, 0x88, 0x88, 0x88, 0x84, 0x84,  0x82, 0x82, 0x81, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
};

//2 Буфера для хранения результатов АЦП
extern uint16_t ADC_Buffer_ping [];
extern uint16_t ADC_Buffer_pong [];
extern uint32_t ping_pong;
//extern uint32_t timer_period;
extern DMA_CtrlDataInitTypeDef DMA_InitStructure;
	/* Структура для инициализации конфигурации канала DMA */
extern	DMA_ChannelInitTypeDef DMA_Channel_InitStructure;
/* Структура для инициализации таймера */
extern TIMER_CntInitTypeDef        TIM_CntInit;
// Функция обработчика прерывания по UART2
// Выбранный пункт меню
uint8_t U_MENU_Item;

// Строка для представления среднего напряжения
char message[64];  

// Указатель на сообщение, выводимое в качестве пункта меню
char* U_MENU_Message;

// Подготовить строку для выбранного пункта меню
char* U_MENU_Prepare_Item(uint8_t item); 

uint16_t period_global;

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

float thd;

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
		 //code
					
					switch(signal)
					{
							case s_sin:
									printf("Sin, THD: %f, Time (ms): %u\n", thd, xTaskGetTickCount());
							break;
							
							case s_ramp:
									printf("Triangle, THD: %f, Time (ms): %u\n", thd, xTaskGetTickCount());
							break;
							
							case s_square:
									printf("Square, THD: %f, Time (ms): %u\n", thd, xTaskGetTickCount());
							break;
					}
					
					
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
	printf("Work with Semaphors\r\n");
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


// Задача обработчик буфера оцифрованных значений
void Task_DSP(void)
{
    arm_status fft_status;
    uint16_t *buffer_ADC,front1, front2, front3, counter = 0, tim_period;
    uint32_t i,average,summa,period = 0;
    float summ_U = 0;
    State state = freq_meas;
	while(1)
	{
		// Объявление указателя на буфер обработки данных 
		
        
        
		float U;
		/* Реализовано ожидание события с помощью двоичного
		семафора. Семафор после создания становится
		доступен (так, как будто его кто-то отдал).
		Поэтому сразу после запуска планировщика задача
		захватит его. Второй раз сделать это ей не удастся,
		и она будет ожидать, находясь в блокированном
		состоянии, пока семафор не отдаст обработчик
		прерывания. Время ожидания задано равным
		бесконечности, поэтому нет необходимости проверять
		возвращаемое функцией xSemaphoreTake() значение. */
		xSemaphoreTake(SemaphoreDMA, portMAX_DELAY);
		TIMER_Cmd(MDR_TIMER2, DISABLE);
		//Задача разблокирована, семафор взят
		if(ping_pong==PING)
		{
			//Заполняется DMA буфер пинг
				// В указатель buffer_ADC записывается начальный адрес массива буфера понг
			buffer_ADC=ADC_Buffer_pong;
		}
		if(ping_pong==PONG)
		{
			//Заполняется DMA буфер понг
			// В указатель buffer_ADC записывается начальный адрес массива буфера пинг
			buffer_ADC=ADC_Buffer_ping;
		}
		// Обработка буфера оцифрованных значений
        
		if(state == freq_meas)
        {
            counter++;
            if(counter == 5)
            {
                summa=0;
                for(i=0;i<ADC_BUFFER_SIZE;i++)
                {
                    summa=buffer_ADC[i]+summa;
                }
                // Получение среднего значения
                average=summa/ADC_BUFFER_SIZE;
                
                if(buffer_ADC[0] > average)
                {
                   for(i = 0; i < ADC_BUFFER_SIZE; i++)
                   {
                      if(buffer_ADC[i] < average)
                      {
                          front1 = i;
                          break;
                      }
                   }
                   for(i = front1; i < ADC_BUFFER_SIZE; i++)
                   {
                      if(buffer_ADC[i] > average)
                      {
                          front2 = i;
                          break;
                      }
                   }
                             for(i = front2; i < ADC_BUFFER_SIZE; i++)
                   {
                      if(buffer_ADC[i] < average)
                      {
                          front3 = i;
                          break;
                      }
                   } 
                }
                
                    
                    
                    //обработка буфера иная
                else
                {
                   for(i = 0; i < ADC_BUFFER_SIZE; i++)
                   {
                      if(buffer_ADC[i] > average)
                      {
                          front1 = i;
                          break;
                      }
                   }
                   for(i = front1; i < ADC_BUFFER_SIZE; i++)
                   {   
                      if(buffer_ADC[i] < average)
                      {
                          front2 = i;
                          break;
                      }
                   }
                   for(i = front2; i < ADC_BUFFER_SIZE; i++)
                   {
                      if(buffer_ADC[i] > average)
                      {
                          front3 = i;
                          break;
                      }
                   } 
                }
								
                period = (front3 - front1);
                tim_period = ((((float)period/ADC_FREQ) * 10000000) / 32) - 1;//(20000 / 32)/(ADC_FREQ/period);  
				//tim_period = (312 /(ADC_FREQ/period))*999;				
                U = ADC_FREQ/period;
				
                sprintf(message , "F = %5.3fHz", U);
                U_MLT_Put_String (message, 4);
								
                DMA_Cmd (DMA_Channel_ADC1, DISABLE);
                TIMER_Cmd(MDR_TIMER2, DISABLE);
				//MDR_TIMER2->STATUS = 0;
				//tim_period=MDR_TIMER2->ARR;
                
				//MDR_TIMER2->ARR=tim_period;
				//tim_period=MDR_TIMER2->ARR;
								
                TIM_CntInit.TIMER_Period = tim_period;
                period = 0;
                //ping_pong = PING;
                //DMA_InitStructure.DMA_DestBaseAddr  =(uint32_t) &ADC_Buffer_ping;
                //DMA_Init (DMA_Channel_ADC1, &DMA_Channel_InitStructure);
                TIMER_CntInit(MDR_TIMER2, &TIM_CntInit);
                
                TIMER_Cmd(MDR_TIMER2, ENABLE);
								//Timer1_init(tim_period);
				DMA_Cmd (DMA_Channel_ADC1, ENABLE);
                
                state = fft;
				counter = 0;
            }
						 
        
        }
        else if(state == fft)
        {
            counter++;       //very govnocod
            if(counter == 5)
            {
                counter = 0;
                fft_status = arm_rfft_init_q15(&S, FFT_SIZE,0, 1);
                if(fft_status == ARM_MATH_SUCCESS)
                {
                    summ_U = 0;
                    arm_rfft_q15(&S, (q15_t *)&buffer_ADC[0], &fft_res[0]);
                    for(i = 0; i < FFT_SIZE*2; i++)
                    {
                        fft_res[i]<<=FFT_UPSCALE;
                    }
                    arm_cmplx_mag_q15(&fft_res[0], (q15_t *)&fft_amp[0], FFT_SIZE);
										
										for(i = 1; i < FFT_SIZE/2; i++)
                    {
                        fft_amp_norm[i] = (float)fft_amp[i] / (float)fft_amp[1];
                    }
										
                    for(i = 2; i < FFT_SIZE/2; i++)
                    {
                        summ_U += fft_amp_norm[i] * fft_amp_norm[i];
                    }
                    thd = 100*(sqrt(summ_U));
                    sprintf(message , "THD = %3.3f%%", thd);
                    U_MLT_Put_String (message, 5);
                    
                    
                    DMA_Cmd (DMA_Channel_ADC1, DISABLE);
  //                  TIMER_Cmd(MDR_TIMER1, DISABLE);
					//MDR_TIMER2->STATUS = 0;
                    //tim_period=MDR_TIMER2->ARR;
					tim_period = 999;
                    TIM_CntInit.TIMER_Period = tim_period;
					//MDR_TIMER2->ARR=tim_period;
                    //tim_period=MDR_TIMER2->ARR;
                    period = 0;
                    //ping_pong = PING;
                    //DMA_InitStructure.DMA_DestBaseAddr  =(uint32_t) &ADC_Buffer_ping;
                    //DMA_Init (DMA_Channel_ADC1, &DMA_Channel_InitStructure);
                    TIMER_CntInit(MDR_TIMER2, &TIM_CntInit);
                    DMA_Cmd (DMA_Channel_ADC1, ENABLE);
                    TIMER_Cmd(MDR_TIMER2, ENABLE);
                    
										
										//Timer1_init(999);
										//DMA_Cmd (DMA_Channel_ADC1, ENABLE);
										
										
                    if(thd < 6)
                    {
                        signal = s_sin;
                        U_MLT_Put_Image (image_sin, 0, 0, 1, 3);
                    }
                    else if(thd < 30)
                    {
                        signal = s_ramp;
                        U_MLT_Put_Image (image_ramp, 0, 0, 1, 3);
                    }
                    else
                    {
                        signal = s_square;
                        U_MLT_Put_Image (image_square, 0, 0, 1, 3);
                    }
																				
                }
				state = freq_meas;
            }
        }
            
            

        
		// Преобразование среднего значения АЦП в измеренное напряжение
		// U_ADC_U равно 3.3, напряжение питания
		// U_ADC_D равно 0x1000 (212=4096) – максимальное значение 12-разрядного АЦП
		// U_ADC_U и U_ADC_D определены в файле adc.h
		//U = ADC_FREQ/period; // получается результат вещественный в вольтах
		// Вывести результат измерения напряжения на ЖКИ
		// формирует строку символов с вставкой 5-символьного поля - вещественное значение с 3 цифрами после запятой 
		TIMER_Cmd(MDR_TIMER2, ENABLE);
	}
	
}	
