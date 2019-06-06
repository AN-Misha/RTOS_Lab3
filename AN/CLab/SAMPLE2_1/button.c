/************************************************************************************
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: button.с
  Назначение: Управление кнопкой
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/
#include "button.h"
#include "common.h"
void U_BTN_Init (void)
{
	// Структура для инициализации портов	
	PORT_InitTypeDef PortInitStructure;

	// Разрешить тактирование порта B
	RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTE, ENABLE);
  // Сделать входом линию, к которой подключены кнопки
	PORT_StructInit (&PortInitStructure);
	
	PortInitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PortInitStructure.PORT_FUNC  = PORT_FUNC_PORT;
	PortInitStructure.PORT_OE    = PORT_OE_IN;
	PortInitStructure. PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortInitStructure. PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    
    PortInitStructure.PORT_Pin   = U_BTN_PIN_UP;
	PORT_Init (U_BTN_PORT_UP, &PortInitStructure);
    
    PortInitStructure.PORT_Pin   = U_BTN_PIN_DOWN;
	PORT_Init (U_BTN_PORT_DOWN, &PortInitStructure);
    
    PortInitStructure.PORT_Pin   = U_BTN_PIN_LEFT;
	PORT_Init (U_BTN_PORT_LEFT, &PortInitStructure);
    
    PortInitStructure.PORT_Pin   = U_BTN_PIN_RIGHT;
	PORT_Init (U_BTN_PORT_RIGHT, &PortInitStructure);
    
    PortInitStructure.PORT_Pin   = U_BTN_PIN_SELECT;
	PORT_Init (U_BTN_PORT_SELECT, &PortInitStructure);
    
    
}

// Получить состояние кнопки (0 - отпущена, 1 - нажата)
uint8_t U_BTN_Read_Button (MDR_PORT_TypeDef* PORTx, uint8_t PORT_Pin)
{
	// Прочитать состояние входа, к которому подключена кнопка
	if(PORT_ReadInputDataBit (PORTx, PORT_Pin))
	  // Кнопка отпущена
	  return 0;
	else
	  // Кнопка нажата
		return 1;
}
