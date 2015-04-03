/*
 * L298N.c
 *
 * Created: 4/3/2015 3:36:44 PM
 *  Author: Butch
 */ 
#include <asf.h>
#include "L298N.h"

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
	unsigned int channel_id;
	
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
	pwm_opt.sync_channel_select[2]    = false;
	pwm_opt.sync_channel_select[3]    = false;
	pwm_opt.cksel                     = PWM_CKSEL_GCLK;
	
	pwm_init(&pwm_opt);

	// Update the period
	pwm_update_period_value(10);

	// Channel configuration
	pwm_channel.CMR.dte   = 1;        // Enable Deadtime for complementary Mode
	pwm_channel.CMR.dthi  = 1;        // Deadtime Inverted on PWMH
	pwm_channel.CMR.dtli  = 0;        // Deadtime Not Inverted on PWML
	pwm_channel.CMR.ces   = 0;        // 0/1 Channel Event at the End of PWM Period
	pwm_channel.CMR.calg  = PWM_MODE_CENTER_ALIGNED;		// Channel mode.
	pwm_channel.CMR.cpol  = PWM_POLARITY_LOW;				// Channel polarity.
	pwm_channel.CMR.cpre  = AVR32_PWM_CPRE_CCK;				// Channel prescaler.
	pwm_channel.cdty      = 10;       // Channel duty cycle, should be < CPRD.
	pwm_channel.cprd      = 20;       // Channel period.

	// With these settings, the output waveform period will be :
	// (56MHz)/20 == 2.8MHz == (MCK/prescaler)/period, with MCK == 56MHz,
	// prescaler == 1, period == 20.

	pwm_channel_init(channel_id, &pwm_channel); // Set channel configuration to channel 0
	pwm_start_channels((1 << channel_id));  // Start channel 0 & 1.
}