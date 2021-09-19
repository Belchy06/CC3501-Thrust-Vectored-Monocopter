/* ###################################################################
**     Filename    : main.c
**     Project     : GPS
**     Processor   : MK22FN512VDC12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2021-09-18, 16:50, # CodeGen: 0
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
#include "PC.h"
#include "ASerialLdd2.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
#include "Sensors.h"
#include "GPS.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
volatile char buffer[128];
volatile uint8 index;
volatile bool complete_command;
GPS gps;
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  NEW_GPS(&gps);
  /* Write your code here */
  /* For example: for(;;) { } */
  float lat, lon;
  float prevLat, prevLon;
  float distance;
  bool firstTime = true;
  // Gps Alitude, Baro Altitude
  float gAlt, bAlt;

  unsigned long age;

  for(;;) {
	  while(!complete_command) {
		  __asm("nop");
	  }

	  if(complete_command) {
		  gps.get_position(&gps, &lat, &lon, &age);
		  //gps.get_altitude(&gps, &gAlt);

		  complete_command = false;
		  index = 0;

		  if(!firstTime) {
			  distance = gps.distance_between(&gps, lat, lon, prevLat, prevLon);
		  }
		  prevLat = lat;
		  prevLon = lon;
		  firstTime = false;
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
