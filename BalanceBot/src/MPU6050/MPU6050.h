/*
 * MPU6050Lib.h
 *
 * Created: 4/11/2015 5:53:43 PM
 *  Author: Butch
 */ 

#ifndef MPU6050_H_
#define MPU6050_H_

typedef enum {MPU6050_OK=0, MPU6050_NOT_DETECTED=100} MPU6050_STATUS_CODE;

int MPU6050_Setup(void);

void MPU6050_Loop(void);
	
#endif /* MPU6050LIB_H_ */