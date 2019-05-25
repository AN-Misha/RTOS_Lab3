/************************************************************************************

  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: adc.h 
  Назначение: Управление АЦП 
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#include "common.h"
 // Канал для измерения напряжения

 #define U_ADC_U_PIN     PORT_Pin_7
// Размер буффера результатов измерений
 #define ADC_BUFFER_SIZE 256
 #define ADC_FREQ 10000
 #define ADC_MEAN 20
 #define PING 0
 #define PONG 1
 // Калибровка вольтметра путем указания полученного значения
// АЦП U_ADC_D для известного напряжения U_ADC_U на входе АЦП
#define U_ADC_U 3.3F
#define U_ADC_D 0x1000
// Прототип функции инициализации АЦП
void ADC_init (void);

