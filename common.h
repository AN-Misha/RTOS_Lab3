/************************************************************************************

  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: common.h
  Назначение: Подключение необходимых библиотек
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_it.h"
/* Подключение функций управления UART */
#include <MDR32F9Qx_uart.h>
/* Подключение функций управления АЦП */
#include <MDR32F9Qx_adc.h>
/* Подключение функций управления таймерами */
#include <MDR32F9Qx_timer.h>
/* Подключение функций управления контроллером ПДП */
#include <MDR32F9Qx_dma.h>
