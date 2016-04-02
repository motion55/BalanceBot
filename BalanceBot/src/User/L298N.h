/*
 * L298N.h
 *
 * Created: 4/3/2015 3:37:09 PM
 *  Author: Butch
 */ 

#ifndef L298N_H_
#define L298N_H_

//#define _NEW_PINS_

#ifdef	_NEW_PINS_

#define L298N_ENA_PIN	AVR32_PWM_PWML_2_1_PIN  	//AVR32_PIN_PC19
//#define L298N_ENA_FUNC	AVR32_PWM_PWML_2_1_FUNCTION
#define L298N_ENB_PIN	AVR32_PWM_PWMH_2_1_PIN  	//AVR32_PIN_PC20	
//#define L298N_ENB_FUNC	AVR32_PWM_PWMH_2_1_FUNCTION
#define	L298N_INP1_PIN	AVR32_PWM_PWMH_1_PIN    	//AVR32_PIN_PC15
#define	L298N_INP1_FUNC	AVR32_PWM_PWMH_1_FUNCTION
#define	L298N_INP2_PIN	AVR32_PWM_PWML_1_PIN    	//AVR32_PIN_PC16
#define	L298N_INP2_FUNC	AVR32_PWM_PWML_1_FUNCTION
#define	L298N_INP3_PIN	AVR32_PWM_PWMH_0_PIN    	//AVR32_PIN_PC17
#define	L298N_INP3_FUNC	AVR32_PWM_PWMH_0_FUNCTION
#define	L298N_INP4_PIN	AVR32_PWM_PWML_0_PIN    	//AVR32_PIN_PC18
#define	L298N_INP4_FUNC	AVR32_PWM_PWML_0_FUNCTION

#define L298N_CHANA	1
#define L298N_CHANB	0
#define L298N_CHANC	2

#else

#define L298N_ENA_PIN	AVR32_PWM_PWMH_1_PIN  	//AVR32_PIN_PC15
//#define L298N_ENA_FUNC	AVR32_PWM_PWMH_1_FUNCTION
#define L298N_ENB_PIN	AVR32_PWM_PWML_1_PIN  	//AVR32_PIN_PC16
//#define L298N_ENB_FUNC	AVR32_PWM_PWML_1_FUNCTION
#define	L298N_INP1_PIN	AVR32_PWM_PWMH_0_PIN   	//AVR32_PIN_PC17
#define	L298N_INP1_FUNC	AVR32_PWM_PWMH_0_FUNCTION
#define	L298N_INP2_PIN	AVR32_PWM_PWML_0_PIN   	//AVR32_PIN_PC18
#define	L298N_INP2_FUNC	AVR32_PWM_PWML_0_FUNCTION
#define	L298N_INP3_PIN	AVR32_PWM_PWML_2_1_PIN 	//AVR32_PIN_PC19
#define	L298N_INP3_FUNC	AVR32_PWM_PWML_2_1_FUNCTION
#define	L298N_INP4_PIN	AVR32_PWM_PWMH_2_1_PIN 	//AVR32_PIN_PC20
#define	L298N_INP4_FUNC	AVR32_PWM_PWMH_2_1_FUNCTION

#define L298N_CHANA	0
#define L298N_CHANB	2
#define L298N_CHANC	1

#endif

/* PWM_FREQUENCY = 24MHz/PWM_PERIOD = 11,718.75Hz*/
#define PWM_PERIOD	2048
#define	DEAD_ZONE	200

#define MAX_SPEED	(PWM_PERIOD-DEAD_ZONE-1)
#define MAX_SPEED_F	(float)(MAX_SPEED/1.5f)

extern void L298N_init(void);

extern int L298_set_speed(int Chan_A_Speed, int Chan_B_Speed);

#endif /* L298N_H_ */