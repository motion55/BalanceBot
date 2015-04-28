/*
 * MPU6050Lib.c
 *
 * Created: 4/11/2015 5:04:10 PM
 *  Author: Butch
 */ 

#include "asf.h"
#include "debug_console.h"

#include <string.h>
#include <math.h>
#include "helper_3dmath.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "MPU6050.h"


// MPU control/status vars
uint8_t devStatus;      // return status after each device operation
//(0 = success, !0 = error)
uint8_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

int16_t a[3];              // [x, y, z]            accel vector
int16_t g[3];              // [x, y, z]            gyro vector
int32_t _q[4];
int32_t t;
int16_t c[3];

VectorFloat gravity;    // [x, y, z]            gravity vector

int r;
int initialized = 0;
int dmpReady = 0;
float lastval[3];
int16_t sensors;

float ypr[3];
Quaternion q;
float temp;
float gyro[3];
float accel[3];
float compass[3];

uint8_t rate = 40;

int MPU6050_Setup(void)
{
	twi_master_options_t opt;
	
	opt.speed = MPU6050_TWI_SPEED;
	opt.chip = MPU6050_ADDRESS;
	
	int result = twi_master_setup(&MPU6050_TWIM, &opt);
	if (result!=STATUS_OK)
		return -1;

	// initialize device
	DebugPrint("Initializing MPU...\n");
	if (mpu_init(NULL) != 0) {
		DebugPrint("MPU init failed!\n");
		return -1;
	}
	DebugPrint("Setting MPU sensors...\n");
	if (mpu_set_sensors(INV_XYZ_GYRO|INV_XYZ_ACCEL)!=0) {
		DebugPrint("Failed to set sensors!\n");
		return -1;
	}
	DebugPrint("Setting GYRO sensitivity...\n");
	if (mpu_set_gyro_fsr(2000)!=0) {
		DebugPrint("Failed to set gyro sensitivity!\n");
		return -1;
	}
	DebugPrint("Setting ACCEL sensitivity...\n");
	if (mpu_set_accel_fsr(2)!=0) {
		DebugPrint("Failed to set accel sensitivity!\n");
		return -1;
	}
	// verify connection
	DebugPrint("Powering up MPU...\n");
	mpu_get_power_state(&devStatus);
	DebugPrint(devStatus ? "MPU6050 connection successful\n" : "MPU6050 connection failed %u\n",devStatus);

	//fifo config
	DebugPrint("Setting MPU fifo...\n");
	if (mpu_configure_fifo(INV_XYZ_GYRO|INV_XYZ_ACCEL)!=0) {
		DebugPrint("Failed to initialize MPU fifo!\n");
		return -1;
	}

	// load and configure the DMP
	DebugPrint("Loading DMP firmware...\n");
	if (dmp_load_motion_driver_firmware()!=0) {
		DebugPrint("Failed to enable DMP!\n");
		return -1;
	}

	DebugPrint("Activating DMP...\n");
	if (mpu_set_dmp_state(1)!=0) {
		DebugPrint("Failed to enable DMP!\n");
		return -1;
	}

	//dmp_set_orientation()
	//if (dmp_enable_feature(DMP_FEATURE_LP_QUAT|DMP_FEATURE_SEND_RAW_GYRO)!=0) {
	DebugPrint("Configuring DMP...\n");
	if (dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT|DMP_FEATURE_SEND_RAW_ACCEL|DMP_FEATURE_SEND_CAL_GYRO|DMP_FEATURE_GYRO_CAL)!=0) {
		DebugPrint("Failed to enable DMP features!\n");
		return -1;
	}

	DebugPrint("Setting DMP fifo rate...\n");
	if (dmp_set_fifo_rate(rate)!=0) {
		DebugPrint("Failed to set dmp fifo rate!\n");
		return -1;
	}
	
	DebugPrint("Resetting fifo queue...\n");
	if (mpu_reset_fifo()!=0) {
		DebugPrint("Failed to reset fifo!\n");
		return -1;
	}

	DebugPrint("Checking... ");
	do {
		delay_ms(1000/rate);  //dmp will habve 4 (5-1) packets based on the fifo_rate
		r=dmp_read_fifo(g,a,_q,&sensors,&fifoCount);
	} while (r!=0 || fifoCount<5); //packets!!!
	DebugPrint("Done.\n");

	initialized = 1;
	
	return 0;
}

void MPU6050_Loop(void)
{
}
