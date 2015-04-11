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

#define TWI_SDA_PIN			AVR32_TWIMS0_TWD_PIN
#define TWI_SDA_FUNCTION	AVR32_TWIMS0_TWD_FUNCTION
#define TWI_SCL_PIN 		AVR32_TWIMS0_TWCK_PIN
#define TWI_SCL_FUNCTION	AVR32_TWIMS0_TWCK_FUNCTION

#define TWI_SPEED             50000   		// Speed of TWI
#define MPU6050_ADDRESS       0b1110100		// MPU6050's TWI 7-bit address

#endif // USER_BOARD_H
