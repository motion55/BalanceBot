/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
//#define BOARD_XOSC_HZ          8000000

// External oscillator type.
//!< External clock signal
//#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
//#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
//#define BOARD_XOSC_STARTUP_US  500000

#define BOARD_OSC0_HZ          12000000
#define BOARD_OSC0_STARTUP_US  17000
#define BOARD_OSC0_IS_XTAL     true

#define BLINK_TIMER     	1
#define BLINK_LED   		AVR32_PIN_PD30
#define DEBUG_TIMER  		2
#define BALANCE_TIMER		3
#define BALANCE_TIMER_2		4


#define DBG_USART   			(&AVR32_USART2)
#define DBG_USART_RX_PIN        AVR32_USART2_RXD_0_1_PIN
#define DBG_USART_RX_FUNCTION   AVR32_USART2_RXD_0_1_FUNCTION
#define DBG_USART_TX_PIN        AVR32_USART2_TXD_0_1_PIN
#define DBG_USART_TX_FUNCTION   AVR32_USART2_TXD_0_1_FUNCTION
#define DBG_USART_BAUDRATE      38400

//#define _USE_USB_FOR_DEBUG_

#ifdef	_USE_USB_FOR_DEBUG_
#define debug_char_rdy()	udi_cdc_is_rx_ready()
#define get_debug_char()	udi_cdc_getc()
#define put_debug_char(ch)	udi_cdc_putc(ch)
#else
#define debug_char_rdy()	usart_test_hit(DBG_USART)
#define get_debug_char()	usart_getchar(DBG_USART)
#define put_debug_char(ch)	(usart_write_char(DBG_USART,ch)==USART_SUCCESS)
#endif	//_USE_USB_FOR_DEBUG_

#define	MPU6050_TWIM             	AVR32_TWIM0
#define MPU6050_TWIM_SDA_PIN     	AVR32_TWIMS0_TWD_PIN
#define MPU6050_TWIM_SDA_FUNCTION	AVR32_TWIMS0_TWD_FUNCTION
#define MPU6050_TWIM_SCL_PIN 		AVR32_TWIMS0_TWCK_PIN
#define MPU6050_TWIM_SCL_FUNCTION	AVR32_TWIMS0_TWCK_FUNCTION

#define MPU6050_TWI_SPEED     400000   		// Speed of TWI
#define MPU6050_ADDRESS       0x68  		// MPU6050's TWI 7-bit address

#define MPU6050_DMP_INT_PIN 	AVR32_PIN_PC22
#define MPU6050_DMP_INT_IRQ		(AVR32_GPIO_IRQ_0+(MPU6050_DMP_INT_PIN/8))
#define MPU6050_DMP_INTC_LEVEL	AVR32_INTC_INT2

//#define MPU9150
#define MPU6050

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "string.h"
#include "L298N.h"
#include "MPU6050.h"
#include "MadgwickAHRS.h"
#include "debug_console.h"
#include "Timer.h"

#define _USE_DEBUG_CONSOLE_

extern void Do_Debug_Idle(void);

extern void debug_main_Z(char *cmd_line);


extern float gain_Ap;
extern float gain_Ai;
extern float gain_Ad;

#endif // USER_BOARD_H
