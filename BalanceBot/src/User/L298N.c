/*
 * L298N.c
 *
 * Created: 4/3/2015 3:36:44 PM
 *  Author: Butch
 */ 
#include <asf.h>
#include "L298N.h"

/* PWM_FREQUENCY = 24MHz/PWM_PERIOD = 11,718.75Hz*/
#define PWM_PERIOD	2048

void L298N_init(void)
{
	pwm_opt_t pwm_opt;                // PWM option config.
	avr32_pwm_channel_t pwm_channel = {{0}, // cmr
										{0}, // cdty
										{0}, // cdtyupd
										{0}, // cprd
										{0}, // cprdupd
										{0}, // ccnt
										{0}, // dt
										{0}};// dtupd  ;  One channel config.
//	unsigned int channel_id;
	
#if (defined(L298N_ENA_FUNC)&&defined(L298N_ENB_FUNC))
	gpio_enable_module_pin(L298N_ENA_PIN, L298N_ENA_FUNC);
	gpio_enable_module_pin(L298N_ENB_PIN, L298N_ENB_FUNC);
#else	
	gpio_set_gpio_pin(L298N_ENA_PIN);
	gpio_set_gpio_pin(L298N_ENB_PIN);
#endif
	
	gpio_enable_module_pin(L298N_INP1_PIN, L298N_INP1_FUNC);
	gpio_enable_module_pin(L298N_INP2_PIN, L298N_INP2_FUNC);
	gpio_enable_module_pin(L298N_INP3_PIN, L298N_INP3_FUNC);
	gpio_enable_module_pin(L298N_INP4_PIN, L298N_INP4_FUNC);

	pwm_opt.diva = AVR32_PWM_DIVA_CLK_OFF;
	pwm_opt.divb = AVR32_PWM_DIVB_CLK_OFF;
	pwm_opt.prea = AVR32_PWM_PREA_CCK;
	pwm_opt.preb = AVR32_PWM_PREB_CCK;

	pwm_opt.fault_detection_activated = false;
	pwm_opt.sync_channel_activated    = true;
	pwm_opt.sync_update_channel_mode  = PWM_SYNC_UPDATE_MANUAL_WRITE_MANUAL_UPDATE;
	pwm_opt.sync_channel_select[0]    = true;
	pwm_opt.sync_channel_select[1]    = true;
	pwm_opt.sync_channel_select[2]    = true;
	pwm_opt.sync_channel_select[3]    = false;
	pwm_opt.cksel                     = PWM_CKSEL_MCK;
	
	pwm_init(&pwm_opt);

	// Update the period
	pwm_update_period_value(10);

	// Channel configuration
	pwm_channel.CMR.dte   = 1;        // Enable Deadtime for complementary Mode
	pwm_channel.CMR.dthi  = 0;        // Deadtime Not Inverted on PWMH
	pwm_channel.CMR.dtli  = 0;        // Deadtime Not Inverted on PWML
	pwm_channel.CMR.ces   = 0;        // 0/1 Channel Event at the End of PWM Period
	pwm_channel.CMR.calg  = PWM_MODE_CENTER_ALIGNED;		// Channel mode.
	pwm_channel.CMR.cpol  = PWM_POLARITY_LOW;				// Channel polarity.
	pwm_channel.CMR.cpre  = AVR32_PWM_CPRE_CCK;				// Channel prescaler.
	pwm_channel.cdty      = PWM_PERIOD/4;    // Channel duty cycle, should be < CPRD.
	pwm_channel.cprd      = PWM_PERIOD;      // Channel period.
	pwm_channel.DT.dth    = 10;
	pwm_channel.DT.dtl    = PWM_PERIOD;

	// With these settings, the output waveform period will be :
	// (48MHz)/2400 == 20KHz == (MCK/prescaler)/period, with MCK == 48MHz,
	// prescaler == 1, period == 1200x2.

#if (defined(L298N_ENA_FUNC)&&defined(L298N_ENB_FUNC))
	pwm_channel_init(0, &pwm_channel); // Set channel configuration to channel 0
	pwm_channel_init(1, &pwm_channel); // Set channel configuration to channel 1
	pwm_channel_init(2, &pwm_channel); // Set channel configuration to channel 2
	pwm_start_channels((1 << 0)|(1 << 1)|(1 << 2));  // Start channels 0, 1, 2.
#else
	pwm_channel_init(L298N_CHANA, &pwm_channel); // Set channel configuration to channel A
	pwm_channel_init(L298N_CHANB, &pwm_channel); // Set channel configuration to channel B
	pwm_start_channels((1 << L298N_CHANA)|(1 << L298N_CHANB));  // Start channels A & B
#endif	
}