/*
 * L298N.c
 *
 * Created: 4/3/2015 3:36:44 PM
 *  Author: Butch
 */ 
#include <asf.h>
#include "L298N.h"


int L298_set_channel_speed(unsigned int channel_id, int channel_speed);

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
	
#ifdef L298N_ENA_FUNC
	gpio_enable_module_pin(L298N_ENA_PIN, L298N_ENA_FUNC);
	gpio_clr_gpio_pin(L298N_INP1_PIN);
	gpio_clr_gpio_pin(L298N_INP2_PIN);
#else
	gpio_set_gpio_pin(L298N_ENA_PIN);
	gpio_enable_module_pin(L298N_INP1_PIN, L298N_INP1_FUNC);
	gpio_enable_module_pin(L298N_INP2_PIN, L298N_INP2_FUNC);
#endif

#ifdef L298N_ENB_FUNC
	gpio_enable_module_pin(L298N_ENB_PIN, L298N_ENB_FUNC);
	gpio_clr_gpio_pin(L298N_INP3_PIN);
	gpio_clr_gpio_pin(L298N_INP4_PIN);
#else	
	gpio_set_gpio_pin(L298N_ENB_PIN);
	gpio_enable_module_pin(L298N_INP3_PIN, L298N_INP3_FUNC);
	gpio_enable_module_pin(L298N_INP4_PIN, L298N_INP4_FUNC);
#endif

	pwm_opt.diva = AVR32_PWM_DIVA_CLK_OFF;
	pwm_opt.divb = AVR32_PWM_DIVB_CLK_OFF;
	pwm_opt.prea = AVR32_PWM_PREA_CCK;
	pwm_opt.preb = AVR32_PWM_PREB_CCK;

	pwm_opt.fault_detection_activated = false;
	pwm_opt.sync_channel_activated    = true;
	pwm_opt.sync_update_channel_mode  = PWM_SYNC_UPDATE_MANUAL_WRITE_MANUAL_UPDATE;
	pwm_opt.sync_channel_select[L298N_CHANA]    = 1;
	pwm_opt.sync_channel_select[L298N_CHANB]    = 1;
	pwm_opt.sync_channel_select[L298N_CHANC]    = 1;
	pwm_opt.sync_channel_select[3]    = 0;
	pwm_opt.cksel                     = PWM_CKSEL_MCK;
	
	pwm_init(&pwm_opt);

	// Update the period
	pwm_update_period_value(1);

	// With these settings, the output waveform period will be :
	// (48MHz)/2400 == 20KHz == (MCK/prescaler)/period, with MCK == 48MHz,
	// prescaler == 1, period == 1200x2.

#if (defined(L298N_ENA_FUNC)||defined(L298N_ENB_FUNC))
	// Channel configuration
	pwm_channel.CMR.dte   = 1;        // Enable Deadtime for complementary Mode
	pwm_channel.CMR.dthi  = 0;        // Deadtime Inverted on PWMH
	pwm_channel.CMR.dtli  = 0;        // Deadtime Inverted on PWML
	pwm_channel.CMR.ces   = 0;        // 0/1 Channel Event at the End of PWM Period
	pwm_channel.CMR.calg  = PWM_MODE_CENTER_ALIGNED;		// Channel mode.
	pwm_channel.CMR.cpol  = PWM_POLARITY_HIGH;				// Channel polarity.
	pwm_channel.CMR.cpre  = AVR32_PWM_CPRE_CCK;				// Channel prescaler.
	pwm_channel.cprd      = PWM_PERIOD;      // Channel period.
	pwm_channel.cdty      = PWM_PERIOD/2;    // Channel duty cycle, should be < CPRD.
	pwm_channel.DT.dth    = 0;
	pwm_channel.DT.dtl    = 0;

	pwm_channel_init(0, &pwm_channel); 
	pwm_channel_init(L298N_CHANC, &pwm_channel); // Set channel configuration to channel 1
	pwm_start_channels((1 << L298N_CHANC)|1);    // Start channel C &  channel 0.
#else
	// Channel configuration
	pwm_channel.CMR.dte   = 1;        // Enable Deadtime for complementary Mode
	pwm_channel.CMR.dthi  = 0;        // Deadtime Not Inverted on PWMH
	pwm_channel.CMR.dtli  = 0;        // Deadtime Not Inverted on PWML
	pwm_channel.CMR.ces   = 0;        // 0/1 Channel Event at the End of PWM Period
	pwm_channel.CMR.calg  = PWM_MODE_CENTER_ALIGNED;		// Channel mode.
	pwm_channel.CMR.cpol  = PWM_POLARITY_LOW;				// Channel polarity.
	pwm_channel.CMR.cpre  = AVR32_PWM_CPRE_CCK;				// Channel prescaler.
	pwm_channel.cprd      = PWM_PERIOD;      // Channel period.
	pwm_channel.cdty      = PWM_PERIOD/2;    // Channel duty cycle, should be < CPRD.
	pwm_channel.DT.dth    = 0;
	pwm_channel.DT.dtl    = 0;	//PWM_PERIOD;

	pwm_channel_init(L298N_CHANA, &pwm_channel); // Set channel configuration to channel A
	pwm_channel_init(L298N_CHANB, &pwm_channel); // Set channel configuration to channel B
	pwm_start_channels((1 << L298N_CHANA)|(1 << L298N_CHANB));  // Start channels A & B
#endif	
}

int L298_set_speed(int Chan_A_Speed, int Chan_B_Speed)
{
#if (defined(L298N_ENA_FUNC)||defined(L298N_ENB_FUNC))
	avr32_pwm_channel_t pwm_channel;
#endif

#ifdef L298N_ENA_FUNC
	if (Chan_A_Speed==0)
	{
		gpio_clr_gpio_pin(L298N_INP1_PIN);
		gpio_clr_gpio_pin(L298N_INP2_PIN);
	}
	else
	if (Chan_A_Speed<0)
	{
		gpio_clr_gpio_pin(L298N_INP1_PIN);
		gpio_set_gpio_pin(L298N_INP2_PIN);
		Chan_A_Speed = -Chan_A_Speed;
	}
	else
	{
		gpio_set_gpio_pin(L298N_INP1_PIN);
		gpio_clr_gpio_pin(L298N_INP2_PIN);
	}
	if (Chan_A_Speed>MAX_SPEED) Chan_A_Speed = MAX_SPEED;
	pwm_channel.cdtyupd = PWM_PERIOD/2;
	pwm_channel.DTUPD.dthupd = Chan_A_Speed+DEAD_ZONE;
#else
	if (Chan_A_Speed==0)
		gpio_clr_gpio_pin(L298N_ENA_PIN);
	else	
		gpio_set_gpio_pin(L298N_ENA_PIN);
	L298_set_channel_speed(L298N_CHANA, Chan_A_Speed);
#endif	
		
#ifdef L298N_ENB_FUNC
	if (Chan_B_Speed==0)
	{
		gpio_clr_gpio_pin(L298N_INP3_PIN);
		gpio_clr_gpio_pin(L298N_INP4_PIN);
	}
	else
	if (Chan_B_Speed<0)
	{
		gpio_set_gpio_pin(L298N_INP3_PIN);
		gpio_clr_gpio_pin(L298N_INP4_PIN);
		Chan_B_Speed = -Chan_B_Speed;
	}
	else
	{
		gpio_clr_gpio_pin(L298N_INP3_PIN);
		gpio_set_gpio_pin(L298N_INP4_PIN);
	}
	if (Chan_B_Speed>MAX_SPEED) Chan_B_Speed = MAX_SPEED;
	pwm_channel.cdtyupd = PWM_PERIOD/2;
	pwm_channel.DTUPD.dtlupd = Chan_B_Speed+DEAD_ZONE;
#else
	if (Chan_B_Speed==0)
		gpio_clr_gpio_pin(L298N_ENB_PIN);
	else	
		gpio_set_gpio_pin(L298N_ENB_PIN);
	L298_set_channel_speed(L298N_CHANB, Chan_B_Speed);
#endif
	
#if (defined(L298N_ENA_FUNC)||defined(L298N_ENB_FUNC))
	pwm_interrupt_update_channel(L298N_CHANC, &pwm_channel);
#endif
	return pwm_update_manual_write_manual_update();
}

int L298_set_channel_speed(unsigned int channel_id, int channel_speed)
{
	avr32_pwm_channel_t pwm_channel;
	
	pwm_channel.cprdupd = PWM_PERIOD;
	
	if (channel_speed>0)
	{
		channel_speed += DEAD_ZONE;
		if (channel_speed>(PWM_PERIOD-1)) channel_speed = PWM_PERIOD-1;
		channel_speed = (PWM_PERIOD-1)-channel_speed;
		pwm_channel.cdtyupd = channel_speed;
		pwm_channel.DTUPD.dtlupd = PWM_PERIOD*2;
		pwm_channel.DTUPD.dthupd = 0;
	}
	else if (channel_speed<0)
	{
		channel_speed = -channel_speed;
		channel_speed += DEAD_ZONE;
		if (channel_speed>(PWM_PERIOD-1)) channel_speed = PWM_PERIOD-1;
		pwm_channel.cdtyupd = channel_speed;
		pwm_channel.DTUPD.dtlupd = 0;
		pwm_channel.DTUPD.dthupd = PWM_PERIOD*2;
	}
	else
	{
		pwm_channel.cdtyupd = 0;
		pwm_channel.DTUPD.dtlupd = PWM_PERIOD*2;
		pwm_channel.DTUPD.dthupd = PWM_PERIOD*2;
	}
	
	return pwm_interrupt_update_channel(channel_id, &pwm_channel);
}

