/*
 * Timer.c
 *
 * Created: 11/26/2014 10:53:44 AM
 *  Author: Butch
 */ 
#include "Timer.h"

#ifndef	true
#define true	1
#endif
#ifndef	false
#define false	0
#endif

#define APPLI_CPU_SPEED	sysclk_get_cpu_hz()

static struct timer_register_type {
	uint32_t endcount;
	char start;
	char ended;
} timerarray[MAX_TIMER];

volatile uint32_t Time_Of_Day; 
volatile uint32_t CPU_counts;

void Calc_Time_of_Day(void);

void StartTimer(short int timerno, int delayvalue)
{
	if	(!timerarray[timerno].start)
	{
		Calc_Time_of_Day();
		timerarray[timerno].endcount = Time_Of_Day + delayvalue;
		timerarray[timerno].ended = false;
		timerarray[timerno].start = true;
	}
}

void ResetTimer(short int timerno)
{
	timerarray[timerno].ended	=	false;
	timerarray[timerno].start	=	false;
}

char TimerOut(short int timerno)
{
	if (timerarray[timerno].ended)
		return true;
	else
	if (timerarray[timerno].start)
	{
		Calc_Time_of_Day();
		int32_t temp = Time_Of_Day - timerarray[timerno].endcount;
		if (temp>=0)	// detect if delay has elapsed
		{
			timerarray[timerno].ended = true;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void InitTimer(void)
{
	int temp;

	for (temp=0; temp<MAX_TIMER; temp++) ResetTimer(temp);
}

void SetupTimer(void)
{
	CPU_counts = Get_sys_count();
}

void Calc_Time_of_Day(void)
{
	uint32_t delta_time = Get_sys_count() - CPU_counts;
	delta_time = (delta_time*TIMER_FREQ)/APPLI_CPU_SPEED;
	CPU_counts += (delta_time*APPLI_CPU_SPEED)/TIMER_FREQ;
	Time_Of_Day += delta_time;
}
