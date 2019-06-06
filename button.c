/************************************************************************************
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: button.с
  Назначение: Управление кнопкой
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/
#include "button.h"
#include "common.h"
// Инициализировать кнопки
void U_BTN_Init (void)
{
	// Структура для инициализации портов	
	PORT_InitTypeDef PortInitStructure;

	// Разрешить тактирование порта B
	RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTC, ENABLE);

  // Сделать входом линию, к которой подключены кнопки
	PORT_StructInit (&PortInitStructure);
	PortInitStructure.PORT_Pin   = U_BTN_PIN;
	PortInitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PortInitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PortInitStructure.PORT_OE    = PORT_OE_IN;
	PortInitStructure. PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortInitStructure. PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PORT_Init (U_BTN_PORT, &PortInitStructure);
}

// Получить состояние кнопки (0 - отпущена, 1 - нажата)
uint8_t U_BTN_Read_Button (void)
{
	// Прочитать состояние входа, к которому подключена кнопка
	if(PORT_ReadInputDataBit (U_BTN_PORT, U_BTN_PIN))
	  // Кнопка отпущена
	  return 0;
	else
	  // Кнопка нажата
		return 1;
}
