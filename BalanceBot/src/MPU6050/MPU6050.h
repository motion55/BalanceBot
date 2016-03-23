/*
 * MPU6050.h
 *
 * Created: 4/11/2015 5:53:43 PM
 *  Author: Butch
 */ 

#ifndef _MPU6050_H_
#define _MPU6050_H_

typedef enum {MPU6050_OK=0, MPU6050_NOT_DETECTED=100} MPU6050_STATUS_CODE;

extern float ypr[3];
extern float gyro[3];
extern float accel[3];
extern float compass[3];

extern uint32_t timestamp;
extern uint32_t deltatime;

int MPU6050_Setup(void);

bool MPU6050_Loop(void);
	
#endif /* _MPU6050_H_ */