/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	ioport_init();	//This must be called before any other ioport function.
	ioport_set_pin_dir(BLINK_LED, IOPORT_DIR_OUTPUT);	//make LED pin an output
	
	#if 1
	init_dbg_rs232(sysclk_get_peripheral_bus_hz(DBG_USART));
	#else
	static const gpio_map_t DBG_USART_GPIO_MAP =
	{
		{DBG_USART_RX_PIN, DBG_USART_RX_FUNCTION},
		{DBG_USART_TX_PIN, DBG_USART_TX_FUNCTION}
	};

	gpio_enable_module(DBG_USART_GPIO_MAP,
	sizeof(DBG_USART_GPIO_MAP) / sizeof(DBG_USART_GPIO_MAP[0]));

	usart_serial_options_t usart_opt = {
		.baudrate = DBG_USART_BAUDRATE,
		.charlength = 8,
		.paritytype = USART_NO_PARITY,
		.stopbits = 1
	};
	usart_serial_init(DBG_USART, &usart_opt);
	#endif

	static const gpio_map_t TWI_GPIO_MAP =
		{
			{MPU6050_TWIM_SDA_PIN, MPU6050_TWIM_SDA_FUNCTION},
			{MPU6050_TWIM_SCL_PIN, MPU6050_TWIM_SCL_FUNCTION}
		};

	// TWI gpio pins configuration
	gpio_enable_module(TWI_GPIO_MAP, sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));
}
