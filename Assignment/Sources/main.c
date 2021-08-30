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
#include "TU1.h"
#include "WAIT1.h"
#include "MCUC1.h"
#include "SERVO1.h"
#include "Pwm2.h"
#include "PwmLdd2.h"
#include "BT.h"
#include "Inhr1.h"
#include "ASerialLdd1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
#include "BNO085.h"
#include "PID.h"
#include "Sensors.h"
#include <stdbool.h>
#include "Controller.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#define BNO080_DEFAULT_ADDRESS 0x4A
#define PI 3.14159

double x, y, z, h;
float quatRadianAccuracy;
Controller controller;

void sendBT() {
	BT_SendStr("X: ");
	BT_SendFloatNum(x);
	BT_SendStr(" Y: ");
	BT_SendFloatNum(y);
	BT_SendStr(" Z: ");
	BT_SendFloatNum(z);
	BT_SendStr(" H: ");
	BT_SendFloatNum(h);
	BT_SendStr("\r\n");

	BT_SendStr("Accuracy: ");
	BT_SendFloatNum(quatRadianAccuracy);
	BT_SendStr("\r\n");

	BT_SendStr("Servo +X: ");
	BT_SendFloatNum(controller.outX);
	BT_SendStr(" Servo -X: ");
	BT_SendFloatNum((3000 - controller.outX));
	BT_SendStr("\r\n");

	BT_SendStr("Servo +Y: ");
	BT_SendFloatNum(controller.outY);
	BT_SendStr(" Servo -Y: ");
	BT_SendFloatNum((3000 - controller.outY));
	BT_SendStr("\r\n");

	BT_SendStr("Throttle: ");
	BT_SendFloatNum(controller.outH);
	BT_SendStr("\r\n");

}

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

//	double x, y, z;
//	double setX, setY, setZ;
//	double outX, outY, outZ;
	New_Controller(&controller);
	// Set the controller gains
	controller.setGains(&controller);
	controller.setXPoint(&controller, 0);
	controller.setYPoint(&controller, 0);
	controller.setZPoint(&controller, 0);
	controller.setHPoint(&controller, 1);
	// Give the controller the memory addresses of the variables that will store our rotation and height data
	controller.init(&controller, &x, &y, &z, &h);


	BNO085 IMU;
	New_BNO085(&IMU, BNO080_DEFAULT_ADDRESS);
	if(IMU.begin(&IMU) == false) {
		while(1)
			;
	}


	IMU.enableRotationVector(&IMU, 50);

	for(;;) {
		if(IMU.dataAvailable(&IMU)) {
			x = (double)(IMU.getRoll(&IMU)) * 180.0 / 3.14159; // Convert roll to degrees
			y = (double)(IMU.getPitch(&IMU)) * 180.0 / 3.14159; // Convert pitch to degrees
			z = (double)(IMU.getYaw(&IMU)) * 180.0 / 3.14159; // Convert yaw to degrees
			quatRadianAccuracy = IMU.getQuatRadianAccuracy(&IMU); // Return the rotation vector accuracy
			controller.calculate(&controller);


			sendBT();


			// SERVO1_SetPWMDutyUs(outX);
//			float i = IMU.getQuatI(&IMU.sensor);
//			float j = IMU.getQuatJ(&IMU.sensor);
//			float k = IMU.getQuatK(&IMU.sensor);
//			float real = IMU.getQuatReal(&IMU.sensor);
//			quatRadianAccuracy = IMU.getQuatRadianAccuracy(&IMU.sensor);
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
