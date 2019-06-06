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

// Порты кнопки
#define U_BTN_PORT_UP MDR_PORTB
#define U_BTN_PIN_UP PORT_Pin_5

#define U_BTN_PORT_DOWN MDR_PORTE
#define U_BTN_PIN_DOWN PORT_Pin_1

#define U_BTN_PORT_LEFT MDR_PORTE
#define U_BTN_PIN_LEFT PORT_Pin_3

#define U_BTN_PORT_RIGHT MDR_PORTB
#define U_BTN_PIN_RIGHT PORT_Pin_6

#define U_BTN_PORT_SELECT MDR_PORTC
#define U_BTN_PIN_SELECT PORT_Pin_2

// Инициализировать кнопку
void U_BTN_Init (void);

// Получить состояние кнопки
uint8_t U_BTN_Read_Button (MDR_PORT_TypeDef* PORTx, uint8_t PORT_Pin);

#endif 
