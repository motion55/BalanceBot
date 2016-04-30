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

typedef struct {
	float w;
	float x;
	float y;
	float z;
} Quaternion;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} VectorInt16;

typedef struct {
	float x;
	float y;
	float z;
} VectorFloat;

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
//(0 = success, !0 = error)
uint8_t buffer[14];
//uint8_t fifoCount;     // count of all bytes currently in FIFO
//uint8_t fifoBuffer[64]; // FIFO storage buffer

int16_t accel_raw[3];			// [x, y, z]            accel vector
int16_t gyro_raw[3];			// [x, y, z]            gyro vector
int32_t _quat[4];
//int32_t t;
//int16_t c[3];

VectorFloat gravity;    // [x, y, z]            gravity vector

float lastval[3];
int16_t sensors;

float ypr[3];
Quaternion q;
float temperature;
float gyro[3];
float accel[3];
float compass[3];

uint8_t fifo_rate = 200;

uint32_t timestamp = 0;
float CPU_MHz = 48000000.0f;

/** Get raw 6-axis motion sensor readings (accel/gyro).
 * Retrieves all currently available motion sensor values.
 * @param ax 16-bit signed integer container for accelerometer X-axis value
 * @param ay 16-bit signed integer container for accelerometer Y-axis value
 * @param az 16-bit signed integer container for accelerometer Z-axis value
 * @param gx 16-bit signed integer container for gyroscope X-axis value
 * @param gy 16-bit signed integer container for gyroscope Y-axis value
 * @param gz 16-bit signed integer container for gyroscope Z-axis value
 * @see getAcceleration()
 * @see getRotation()
 * @see MPU6050_RA_ACCEL_XOUT_H
 */
void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    readBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
    *ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    *ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    *az = (((int16_t)buffer[4]) << 8) | buffer[5];
    *gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    *gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    *gz = (((int16_t)buffer[12]) << 8) | buffer[13];
}

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

	gyro[0] = gyro_raw[0] / 131.072f;
	gyro[1] = gyro_raw[1] / 131.072f;
	gyro[2] = gyro_raw[2] / 131.072f;

	if (mpu_get_accel_reg(accel_raw)) return false;

	accel[0] = accel_raw[0] / 182.044f;
	accel[1] = accel_raw[1] / 182.044f;
	accel[2] = accel_raw[2] / 182.044f;

	return true;
}

