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
#include "asf.h"
#include "Timer.h"

float gain_Ap = 40.0;
float gain_Ai = 2.0;
float gain_Bp = 40.0;
float gain_Bi = 2.0;
float ypr_init[3];

float time_start;
float time_end;
unsigned int  response_time;



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
	
#ifdef	_USE_USB_FOR_DEBUG_
	udc_start();	// Start USB stack
#endif
#ifdef	_USE_DEBUG_CONSOLE_
	DebugInit();
#endif

#ifdef _MPU6050_H_	
	MPU6050_Setup();
#endif
	
	L298N_init();
	
	StartTimer(BLINK_TIMER, 500);
	StartTimer(BALANCE_TIMER, 5);

	#define SPEED_RATE	4
	
	char init_done = 0;
	float speed_Ai_calc = 0.0;
	float speed_Bi_calc = 0.0;
	int speed_A = 0;
	int speed_A_start = 0;
	int speed_A_end = MAX_SPEED;
	int speed_B = 0;
	int speed_B_start = 0;
	int speed_B_end = MAX_SPEED;

	int speed_increment = SPEED_RATE;

	int loops = 0;
	uint32_t lasttime = Get_sys_count();
	uint32_t scantime = 0;
		
	while (true) {
#ifdef	_USE_DEBUG_CONSOLE_
		DebugTask();
#endif

#ifdef _MPU6050_H_
		if (MPU6050_Loop())
		{
			uint32_t nowtime = Get_sys_count();
			scantime += nowtime - lasttime;
			lasttime = nowtime;

			loops++;
			if (loops>=16)
			{
				scantime /= loops;
				#ifdef	_USE_DEBUG_CONSOLE_
				DebugPrint("\r\n scantime = %8li ", scantime);
				#endif
				loops = 0;
				scantime = 0;
			}
		}
#else
		if(TimerOut(BALANCE_TIMER))
		{
			ResetTimer(BALANCE_TIMER);
			StartTimer(BALANCE_TIMER,5);
			speed_A += speed_increment;

			if (speed_A>=MAX_SPEED)
			{
				speed_A = MAX_SPEED;
				speed_increment = -SPEED_RATE;
			}
			else
			if (speed_A<=-MAX_SPEED)
			{
				speed_A = -MAX_SPEED;
				speed_increment = SPEED_RATE;
			}
			speed_B = speed_A;

			L298_set_speed(speed_A, speed_B);
			#ifdef	_USE_DEBUG_CONSOLE_
			DebugPrint("\r\n %8i %8i" , speed_A, speed_B);
			#endif
		}
#endif
	}
}
