/*
 * MPU6050.c
 *
 * Created: 4/11/2015 5:04:10 PM
 *  Author: Butch
 */ 

#include "asf.h"

#include <string.h>
#include <math.h>
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "I2Cdev.h"

#define TRACE

#define YAW 	0
#define PITCH	1
#define ROLL	2
#define DIM 	3

#define wrap_180(x) (x < -180 ? x+360 : (x > 180 ? x - 360: x))

uint8_t GetGravity(VectorFloat *v, Quaternion *q);
uint8_t GetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity);
float invSqrt(float x);

// MPU control/status vars
uint8_t devAddr = MPU6050_DEFAULT_ADDRESS;
uint8_t devStatus;      // return status after each device operation
uint8_t buffer[14];

int16_t accel_raw[3];			// [x, y, z]            accel vector
int16_t gyro_raw[3];			// [x, y, z]            gyro vector

VectorFloat gyro;
VectorFloat accel;
VectorFloat magnet;

VectorFloat angle;

float ypr[3];

Quaternion q;

uint8_t fifo_rate = 200;

uint32_t timestamp = 0;
float CPU_MHz = 48000000.0f;

/////////////////////////////////////////////////////////////////////////////////////////////////////

int MPU6050_Setup(void)
{
	twi_master_options_t opt;
	
	opt.speed = MPU6050_TWI_SPEED;
	opt.chip = MPU6050_ADDRESS;
	
	int result = twi_master_setup(&MPU6050_TWIM, &opt);
	if (result!=STATUS_OK) return -1;

	// initialize device
	TRACE("Initializing MPU...\r\n");
	result = mpu_init(NULL);
	if (result != 0)
	{
		TRACE("MPU init failed! \r\n");
		return -1;
	}
	
	TRACE("Setting MPU sensors...\r\n");
	if (mpu_set_sensors(INV_XYZ_GYRO|INV_XYZ_ACCEL)!=0) {
		TRACE("Failed to set sensors!\r\n");
		return -1;
	}
	
	TRACE("Setting GYRO sensitivity...\r\n");
	if (mpu_set_gyro_fsr(250)!=0) {
		TRACE("Failed to set gyro sensitivity!\r\n");
		return -1;
	}
	TRACE("Setting ACCEL sensitivity...\r\n");
	if (mpu_set_accel_fsr(2)!=0) {
		TRACE("Failed to set accel sensitivity!\r\n");
		return -1;
	}
	// verify connection
	TRACE("Powering up MPU...\r\n");
	mpu_get_power_state(&devStatus);
	TRACE(devStatus ? "MPU6050 connection successful\r\n" : "MPU6050 connection failed %u\r\n");
	
	StartTimer(BALANCE_TIMER, 5);

	CPU_MHz = sysclk_get_cpu_hz();
	timestamp = Get_sys_count();

	return 0;
}

float GetFilteredAngle(float angle, float gyro, float delta, float accel)
{
	angle = (0.99f * (angle + (gyro * delta))) + (0.01f * accel);

	return angle;
}


bool MPU6050_Loop(void)
{
	if (mpu_get_gyro_reg(gyro_raw))	return false;

	uint32_t delta = timestamp;
	timestamp  = Get_sys_count();
	float dt = ((float)(timestamp - delta))/CPU_MHz;

	gyro.x = gyro_raw[0] / 131.072f;
	gyro.y = gyro_raw[1] / 131.072f;
	gyro.z = gyro_raw[2] / 131.072f;

	if (mpu_get_accel_reg(accel_raw)) return false;

	accel.x = accel_raw[0] / 182.044f;
	accel.y = accel_raw[1] / 182.044f;
	accel.z = accel_raw[2] / 182.044f;

	angle.x = GetFilteredAngle(angle.x,gyro.x,dt,accel.y);
	angle.y = GetFilteredAngle(angle.y,gyro.y,dt,accel.x);
	angle.z = GetFilteredAngle(angle.z,gyro.z,dt,accel.x);

	return true;
}

