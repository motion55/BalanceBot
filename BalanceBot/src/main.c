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

float time_start;
float time_end;
unsigned int  response_time;

float gain_Ap = 150.0f;
float gain_Ai = 20.0f;
float gain_Ad = 800.0f;

float decay = 0.99;

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
	float speed_I_calc = 0.0;

	int speed_A = 0;
	int speed_B = 0;

	int speed_increment = SPEED_RATE;

	int loops = 0;
	uint32_t lasttime = Get_sys_count();
	uint32_t scantime = 0;

	float old_angle = 0;

	//float gain_Bp = 40.0;
	//float gain_Bi = 2.0;
	float ypr_zero[3];
	float ypr_total[3] = {0.0,0.0,0.0};
	float ypr_zero_long[3];
	
#define AVERAGE_SIZE	256

	while (!init_done) 
	{
		if (MPU6050_Loop())
		{
			ypr_total[0] +=  ypr[0];
			ypr_total[1] +=  ypr[1];
			ypr_total[2] +=  ypr[2];

			loops++;
			if (loops>=256)
			{
				init_done = 1;
				ypr_zero[0] =  ypr_total[0]/loops;
				ypr_zero[1] =  ypr_total[1]/loops;
				ypr_zero[2] =  ypr_total[2]/loops;
				loops = 0;
				ypr_total[0] = 0;
				ypr_total[1] = 0;
				ypr_total[2] = 0;

				ypr_zero_long[1] = ypr_zero[1]*AVERAGE_SIZE;
				old_angle = ypr_zero[1];


				#ifdef	_USE_DEBUG_CONSOLE_
				DebugPrint("\r\n     Init %8.3f - %8.3f %8.3f %8.3f", ypr_zero[1], ypr[0], ypr[1], ypr[2]);
				#endif
			}
		}
	}
		
	for (;;) {
#ifdef	_USE_DEBUG_CONSOLE_
		DebugTask();
#endif

#ifdef _MPU6050_H_
		if (MPU6050_Loop())
		{
	#if 1
			ypr_total[0] += ypr[0];
			ypr_total[1] += ypr[1];
			ypr_total[2] += ypr[2];

			loops++;
			if (loops>=4)
			{
				loops = 0;

				ypr[0] = ypr_total[0]/4;
				ypr[1] = ypr_total[1]/4;
				ypr[2] = ypr_total[2]/4;

				ypr_total[0] = 0;
				ypr_total[1] = 0;
				ypr_total[2] = 0;

				#define BALANCE_PORT	2

				float tilt_angle = -(ypr[BALANCE_PORT] - ypr_zero[BALANCE_PORT]);
				float speed_P_calc = tilt_angle * gain_Ap;
				speed_I_calc += tilt_angle * gain_Ai;
				float speed_D_calc = (tilt_angle-old_angle) * gain_Ad;
				old_angle = tilt_angle;

//				if (tilt_angle>=0 && old_angle<0) speed_I_calc = 0;
//				else
//				if (tilt_angle<=0 && old_angle>0) speed_I_calc = 0;
//				speed_I_calc *= decay;

				if (speed_I_calc>MAX_SPEED_F) speed_I_calc = MAX_SPEED_F;
				else
				if (speed_I_calc<-MAX_SPEED_F) speed_I_calc = -MAX_SPEED_F;
				
				speed_A = (int)(speed_P_calc + speed_I_calc + speed_D_calc);

				if (speed_A>MAX_SPEED) speed_A = MAX_SPEED;
				else
				if (speed_A<-MAX_SPEED) speed_A = -MAX_SPEED;

				speed_B = speed_A;
				
				L298_set_speed(speed_A, speed_B);

				//ypr_zero_long[BALANCE_PORT] -= ypr_zero[BALANCE_PORT];
				//ypr_zero_long[BALANCE_PORT] += ypr[BALANCE_PORT];
				//ypr_zero[BALANCE_PORT] = ypr_zero_long[BALANCE_PORT]/AVERAGE_SIZE;

				#ifdef	_USE_DEBUG_CONSOLE_
				//DebugPrint("\r\n %8.3f %8.3f - %8.3f %8.3f %8.3f", tilt_angle, ypr_zero[BALANCE_PORT], ypr[0], ypr[1], ypr[2]);
				DebugPrint("\r\n %8.3f %5i", tilt_angle, speed_A);
				#endif
			}
	#else
			ypr_total[0] += ypr[0];
			ypr_total[1] += ypr[1];
			ypr_total[2] += ypr[2];

			loops++;
			if (loops>=8)
			{
				ypr[0] = ypr_total[0]/8;
				ypr[1] = ypr_total[1]/8;
				ypr[2] = ypr_total[2]/8;

				ypr_total[0] = 0;
				ypr_total[1] = 0;
				ypr_total[2] = 0;

				uint32_t nowtime = Get_sys_count();
				scantime = nowtime - lasttime;
				lasttime = nowtime;

				scantime /= loops;
				loops = 0;

				#ifdef	_USE_DEBUG_CONSOLE_
				DebugPrint("\r\n scantime = %8li - %8.3f %8.3f %8.3f", scantime, ypr[0], ypr[1], ypr[2]);
				#endif
			}
	#endif
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
