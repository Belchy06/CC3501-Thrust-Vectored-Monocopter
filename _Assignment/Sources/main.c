/* ###################################################################
 **     Filename    : main.c
 **     Project     : _Assignment
 **     Processor   : MK22FN512VDC12
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2021-09-11, 19:34, # CodeGen: 0
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
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Sensors.h"
#include "BNO085.h"

#define BNO080_DEFAULT_ADDRESS 0x4A
#define PI 3.14159
#define RAD_TO_DEGREES 57.2958
#define MS_TO_GS 0.1019

float h;
float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;
float pitch, roll, yaw;
uint8_t accelAccuracy, gyroAccuracy, magAccuracy;
float quatRadianAccuracy;
float tau = 0.98;

char str[128];
char level[32];
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	/* For example: for(;;) { } */
	BNO085 IMU;
	New_BNO085(&IMU, BNO080_DEFAULT_ADDRESS);
	if (IMU.begin(&IMU) == false) {
		while (1)
			;
	}

	IMU.enableAccelerometer(&IMU, 50);
	IMU.enableGyro(&IMU, 50);
	IMU.enableMagnetometer(&IMU, 50);
	// TODO Own sensor fusion. Get mag, accel, gyro
	for (;;) {
		if (IMU.dataAvailable(&IMU)) {
			// x = (double)(IMU.getRoll(&IMU)) * 180.0 / 3.14159; // Convert roll to degrees
			// y = (double)(IMU.getPitch(&IMU)) * 180.0 / 3.14159; // Convert pitch to degrees
			// z = (double)(IMU.getYaw(&IMU)) * 180.0 / 3.14159; // Convert yaw to degrees
			// quatRadianAccuracy = IMU.getQuatRadianAccuracy(&IMU); // Return the rotation vector accuracy
			// controller.calculate(&controller);
			unsigned int time;
			float dt = 0.0001;
			IMU.getAccel(&IMU, &ax, &ay, &az, &accelAccuracy);
			IMU.getGyro(&IMU, &gx, &gy, &gz, &gyroAccuracy);
			IMU.getMag(&IMU, &mx, &my, &mz, &magAccuracy);

			float rotation_x = gx * RAD_TO_DEGREES;
			float rotation_y = gy * RAD_TO_DEGREES;
			float rotation_z = gz * RAD_TO_DEGREES;

			float accel_x = ax * MS_TO_GS;
			float accel_y = ay * MS_TO_GS;
			float accel_z = az * MS_TO_GS;

			// Complementary filter
			float accelPitch = atan2(accel_y, accel_z) * RAD_TO_DEGREES;
			float accelRoll = atan2(accel_x, accel_z) * RAD_TO_DEGREES;

			pitch = (tau) * (pitch + rotation_x * dt)
					+ (1 - tau) * (accelPitch);
			roll = (tau) * (roll - rotation_y * dt) + (1 - tau) * (accelRoll);
			yaw += rotation_z * dt;
		}
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