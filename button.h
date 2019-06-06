/************************************************************************************
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: button.h 
  Назначение: Управление кнопкой   
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#ifndef __U_BUTTON
 #define __U_BUTTON

#include "common.h"

// Порт кнопки
#define U_BTN_PORT MDR_PORTC
// Линия для кнопки
#define U_BTN_PIN PORT_Pin_2

// Инициализировать кнопку
void U_BTN_Init (void);

// Получить состояние кнопки
uint8_t U_BTN_Read_Button (void);

#endif 
