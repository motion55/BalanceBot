/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "Timer.h"
#include "debug_console.h"
#include "MPU6050.h"

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();

	board_init();
	
	/* Initialize interrupts */
	irq_initialize_vectors();
	cpu_irq_enable();

	/* Insert application code here, after the board has been initialized. */
	SetupTimer();
	
	InitTimer();
	
	DebugInit();

//	MPU6050_Setup();
	
	L298N_init();
	
	StartTimer(BLINK_TIMER, 500);
	
	while (true) {
		DebugTask();
		
		if (TimerOut(BLINK_TIMER))
		{
			ResetTimer(BLINK_TIMER);
			StartTimer(BLINK_TIMER, 500);
			ioport_toggle_pin_level(BLINK_LED);
		}
	}
}
