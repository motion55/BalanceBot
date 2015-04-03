/*
 * Timer.h
 *
 * Created: 11/26/2014 10:55:09 AM
 *  Author: Butch
 */ 

#ifndef TIMER_H_
#define TIMER_H_

#include <asf.h>

#define TC_CHANNEL 	0	//use channel 0 of timer/counters 0-2
#define TIMER_FREQ	1000	//1000Hz
#define MAX_TIMER	256

#ifndef F_PBA
#define F_PBA sysclk_get_pba_hz()
#endif

void SetupTimer(void);

void StartTimer(short int timerno, int delayvalue);

void ResetTimer(short int timerno);

char TimerOut(short int timerno);

void InitTimer(void);

#endif /* TIMER_H_ */