/* ###################################################################
 **     Filename    : main.c
 **     Project     : Lab_03
 **     Processor   : MK22FN512VDC12
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2021-08-13, 13:30, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Pins1.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "MCUC1.h"
#include "PC.h"
#include "Inhr2.h"
#include "ASerialLdd2.h"
#include "Inhr1.h"
#include "ASerialLdd1.h"
#include "FC321.h"
#include "RealTimeLdd1.h"
#include "TU1.h"
#include "WAIT1.h"
#include "GPSTimer.h"
#include "RealTimeLdd2.h"
#include "TU2.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
#include "BNO085.h"
#include "Sensors.h"
#include <stdbool.h>
#include "printf.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "GPS.h"
#include "utm.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#define BNO080_DEFAULT_ADDRESS 0x4A
#define BMPXXX_DEFAULT_ADDRESS 0x00

float h;
float pitch, roll, yaw = 0;
float p, q, r = 0;
;
float ax, ay, az = 0;
float lx, ly, lz = 0;
uint8_t accelAccuracy, gyroAccuracy, magAccuracy, linAccuracy;
float quatRadianAccuracy;
char level[32];

unsigned int time;

float lat, lon, startLat, startLon, distance = 0;
float imudx, imudy, imudz = 0;
float dt;
unsigned long age;
float mps;
volatile GPS gps;
volatile uint8_t index;
volatile char buffer[256];
volatile bool complete_command;
bool debug = false;
bool haveGps;
float totalTime;

struct GPSStats {
	float dx;
	float dy;
	float x;
	float y;
};

//void sendBT() {
////	snprintf(str, 128,
////			"xAcc: %f, yAcc: %f, xMps: %f, yMps: %f, xOff: %f, yOff: %f, dT: %f\r\n",
////			lx, ly, xMps, yMps, xOff, yOff, dt);
////	PC_SendStr(str);
//	snprintf(str, 128, "roll: %f, pitch: %f, yaw: %f, gYaw: %f\r\n", roll,
//			pitch, yaw, gYaw);
//	PC_SendStr(str);
//}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

//	/* Write your code here */
//	/* For example: for(;;) { } */
	// Initialize setpoints
	NEW_GPS(&gps);
	bool firstTime = true;
	haveGps = false;
	BNO085 IMU;
	New_BNO085(&IMU, BNO080_DEFAULT_ADDRESS);
	if (IMU.begin(&IMU) == false) {
		while (1)

			;

	}

	IMU.enableGameRotationVector(&IMU, 300);
	IMU.enableLinearAccelerometer(&IMU, 300);
	IMU.enableGyro(&IMU, 200);

	// Get starting position of GPS
//	uint8_t i = 0;
//	while (i < 25) {
//		if(complete_command) {
//			gps.get_position(&gps, &lat, &lon, &age);
//			startLat += lat;
//			startLon += lon;
//			i++;
//			complete_command = false;
//			index = 0;
//		}
//	}

	struct GPSStats stats;
	char str[512];
	PC_CRLF();
	snprintf(str, 512,
			"time,gps.x,gps.y,gps.dx,gps.dy,imu.dx,imu.dy,imu.dz,imu.ddx,imu.ddy,imu.ddz,imu.q,imu.p,imu.r,imu.roll,imu.pitch,imu.yaw\r\n");
	PC_SendStr(str);

	FC321_Reset();
	totalTime = 0;
	for (;;) {
		if (complete_command) {
			gps.get_position(&gps, &lat, &lon, &age);
			//gps.get_altitude(&gps, &gAlt);
			long zone;
			char hemisphere;
			double easting;
			double northing;
			long res = Convert_Geodetic_To_UTM(lat * 0.0174533f, lon * 0.0174533f, &zone, &hemisphere,
					&easting, &northing);

			stats.x = (float)easting;
			stats.y = (float)northing;
			if (!firstTime) {
				float gYaw = gps.course_to(&gps, startLat, startLon, lat, lon);
				// Velocity
				mps = gps.speed_mps(&gps);
				if (mps != -1.0) {
					stats.dx = mps * cosf(gYaw);
					stats.dy = mps * sinf(gYaw);
				}
			}
			startLat = lat;
			startLon = lon;
			firstTime = false;
			complete_command = false;
			index = 0;
			haveGps = true;
		}

		if (IMU.dataAvailable(&IMU)) {
			roll = (float) (IMU.getRoll(&IMU)) * 180.0 / 3.14159f; // Convert roll to degrees
			pitch = (float) (IMU.getPitch(&IMU)) * 180.0 / 3.14159f; // Convert pitch to degrees
			yaw = (float) (IMU.getYaw(&IMU)) * 180.0 / 3.14159f; // Convert yaw to degrees
			IMU.getGyro(&IMU, &p, &q, &r, &gyroAccuracy);
			IMU.getLinAccel(&IMU, &lx, &ly, &lz, &linAccuracy);
			if (lx > 9 || ly > 9 || lz > 9) {
				// Initially the IMU hasn't removed the gravity vector. if this is the case, set linear accelerations to 0
				lx = ly = lz = 0;
			}
			FC321_GetTimeUS(&time);
			dt = time * 1e-6;
			totalTime += dt * 13.25;

			// GPS: x, y, dx, dy
			// IMU: ddx, ddy, roll, pitch, yaw
			// Time: time, dt
			// Interpolated: IMU dx, dy, dz
			imudx = imudx + lx * dt;
			imudy = imudy + ly * dt;
			imudz = imudz + lz * dt;

			if (haveGps) {
				snprintf(str, 512, "%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f\r\n", totalTime, stats.x, stats.y, stats.dx,
						stats.dy, imudx, imudy, imudz, lx, ly, lz, q, p, r,
						roll, pitch, yaw);
				haveGps = false;
			} else {
				snprintf(str, 512, "%f,"
						"NaN,"
						"NaN,"
						"NaN,"
						"NaN,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f,"
						"%f\r\n", totalTime, imudx, imudy, imudz, lx, ly, lz, q,
						p, r, roll, pitch, yaw);
			}
			PC_SendStr(str);
			FC321_Reset();
		}

		//sendBT();
	}
	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
