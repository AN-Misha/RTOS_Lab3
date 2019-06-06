/************************************************************************************
  Проект: Lab2_2
  Программа: Работа с кнопкой
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: menu.h 
  Назначение: Формирование меню   
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#ifndef __U_MENU
 #define __U_MENU

#include "common.h"
#include "button.h"
#include "MLT/mlt_lcd.h"


// Количество пунктов меню
#define U_MENU_ITEM_COUNT 4
// Время на антидребезг в мс
#define DrebezgTime 50
#define ON 1
#define OFF 0


// Инициализировать меню
void U_MENU_Init (void);

// Задача по работе с меню
void U_MENU_Task_Function (void);
// Задача по выводу бегущей строки на ЖКИ
void U_MENU_Running_String_Task_Function (void);
// Задача - приветствие
void U_Privet(void);
//Задача по выводу на ЖКИ информации полученной с терминала по UART
void U_MENU_Output (void);
// Задача для работы с выводом по UART
void Task_output(void);
#endif 
