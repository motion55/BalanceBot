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

#define TRACE	print_dbg

#define YAW 	0
#define PITCH	1
#define ROLL	2
#define DIM 	3

#define wrap_180(x) (x < -180 ? x+360 : (x > 180 ? x - 360: x))

uint8_t GetGravity(VectorFloat *v, Quaternion *q);
uint8_t GetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity);
float invSqrt(float x);

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

bool dmpInterrupt = false;
bool dmpReady = false;
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
uint32_t deltatime = 0;

void MPU6050_SetupInterrupt(void);

__attribute__((__interrupt__))
static void MPU6050_DMP_int_handler(void)
{
	gpio_clear_pin_interrupt_flag(MPU6050_DMP_INT_PIN);
	dmpInterrupt = true;
}

int MPU6050_Setup(void)
{
	twi_master_options_t opt;
	
	opt.speed = MPU6050_TWI_SPEED;
	opt.chip = MPU6050_ADDRESS;
	
	dmpReady = false;
	
	int result = twi_master_setup(&MPU6050_TWIM, &opt);
	if (result!=STATUS_OK)
		return -1;

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
	if (mpu_set_gyro_fsr(2000)!=0) {
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

	//fifo config
	TRACE("Setting MPU fifo...\r\n");
	if (mpu_configure_fifo(INV_XYZ_GYRO|INV_XYZ_ACCEL)!=0) {
		TRACE("Failed to initialize MPU fifo!\r\n");
		return -1;
	}

	// load and configure the DMP
	TRACE("Loading DMP firmware...\r\n");
	if (dmp_load_motion_driver_firmware()!=0) {
		TRACE("Failed to enable DMP!\r\n");
		return -1;
	}

	TRACE("Activating DMP...\r\n");
	if (mpu_set_dmp_state(1)!=0) {
		TRACE("Failed to enable DMP!\r\n");
		return -1;
	}

	//dmp_set_orientation()
	//if (dmp_enable_feature(DMP_FEATURE_LP_QUAT|DMP_FEATURE_SEND_RAW_GYRO)!=0) {
	TRACE("Configuring DMP...\r\n");
	if (dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT|DMP_FEATURE_SEND_RAW_ACCEL|DMP_FEATURE_SEND_CAL_GYRO|DMP_FEATURE_GYRO_CAL)!=0) {
		TRACE("Failed to enable DMP features!\r\n");
		return -1;
	}

	TRACE("Setting DMP fifo rate...\r\n");
	if (dmp_set_fifo_rate(fifo_rate)!=0) {
		TRACE("Failed to set dmp fifo rate!\r\n");
		return -1;
	}
	
	TRACE("Resetting fifo queue...\r\n");
	if (mpu_reset_fifo()!=0) {
		TRACE("Failed to reset fifo!\r\n");
		return -1;
	}

	TRACE("Checking... ");
	int r;
	do {
		delay_ms(1000/fifo_rate);  //dmp will have 4 (5-1) packets based on the fifo_rate
		r=dmp_read_fifo(g,a,_q,&sensors,&fifoCount);
	} while (r!=0 || fifoCount<5); //packets!!!
	TRACE("Done.\r\n");

	MPU6050_SetupInterrupt();
	
	dmpReady = true;
	
	return 0;
}

void MPU6050_SetupInterrupt(void)
{
	TRACE("Setup DMP interrupt.\r\n");
	dmpInterrupt = false;
	gpio_configure_pin(MPU6050_DMP_INT_PIN, GPIO_DIR_INPUT|GPIO_PULL_UP);
	INTC_register_interrupt(&MPU6050_DMP_int_handler, MPU6050_DMP_INT_IRQ, MPU6050_DMP_INTC_LEVEL);
	gpio_clear_pin_interrupt_flag(MPU6050_DMP_INT_PIN);
	gpio_enable_pin_interrupt(MPU6050_DMP_INT_PIN, GPIO_FALLING_EDGE);
	Enable_global_interrupt();
}

bool MPU6050_Loop(void)
{
	if (!dmpReady) 
	{
		TRACE("Error: DMP not ready!!\r\n");
		return false;
	}
	
	if (!dmpInterrupt)
		return false;
	dmpInterrupt = false;
	
	timestamp = Get_sys_count();

	while (dmp_read_fifo(g,a,_q,&sensors,&fifoCount)!=0); //gyro and accel can be null because of being disabled in the efeatures
	
	q.w = (float)_q[0] / 16384.0f;
	q.x = (float)_q[1] / 16384.0f;
	q.y = (float)_q[2] / 16384.0f;
	q.z = (float)_q[3] / 16384.0f;
	
	GetGravity(&gravity, &q);
	GetYawPitchRoll(ypr, &q, &gravity);

	//scaling for degrees output
	for (int i=0;i<DIM;i++){
		ypr[i]*=180/M_PI;
	}

	//unwrap yaw when it reaches 180
	ypr[0] = wrap_180(ypr[0]);

	//change sign of Pitch, MPU is attached upside down
	ypr[1]*=-1.0;
	#if 0
	mpu_get_temperature(&t,&timestamp);
	temperature=(float)t/65536L;

	mpu_get_compass_reg(c,&timestamp);

	//0=gyroX, 1=gyroY, 2=gyroZ
	//swapped to match Yaw,Pitch,Roll
	//Scaled from deg/s to get tr/s
	for (int i=0;i<DIM;i++){
		gyro[i]   = (float)(g[DIM-i-1])/131.0/360.0;
		accel[i]   = (float)(a[DIM-i-1]);
		compass[i] = (float)(c[DIM-i-1]);
	}
	#endif	
	
	deltatime = Get_sys_count()-timestamp;
	
	return true;
}

uint8_t GetGravity(VectorFloat *v, Quaternion *q) 
{
	v -> x = 2 * (q -> x*q -> z - q -> w*q -> y);
	v -> y = 2 * (q -> w*q -> x + q -> y*q -> z);
	v -> z = q -> w*q -> w - q -> x*q -> x - q -> y*q -> y + q -> z*q -> z;
	return 0;
}

uint8_t GetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity) 
{
	// yaw: (about Z axis)
	data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);
	// pitch: (nose up/down, about Y axis)
	data[1] = atan(gravity->x * invSqrt(gravity->y*gravity->y + gravity->z*gravity->z));
	// roll: (tilt left/right, about X axis)
	data[2] = atan(gravity->y * invSqrt(gravity->x*gravity->x + gravity->z*gravity->z));
	return 0;
}

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
 __attribute__((weak))
 float invSqrt(float x) 
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}
