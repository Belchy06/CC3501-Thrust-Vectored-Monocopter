/* ###################################################################
 **     Filename    : Events.c
 **     Project     : Assignment
 **     Processor   : MK22FN512VDC12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2021-09-20, 19:05, # CodeGen: 44
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Contents    :
 **         Inhr1_OnRxChar - void Inhr1_OnRxChar(void);
 **         Cpu_OnNMI      - void Cpu_OnNMI(void);
 **
 ** ###################################################################*/
/*!
 ** @file Events.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#ifdef __cplusplus
extern "C" {
#endif 

#include "GPS.h"
#include "BNO085.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
extern volatile NEO6M GPS;
extern volatile BNO085 IMU;

extern volatile uint8_t gpsIndex;
extern volatile char gpsBuffer[128];
extern volatile bool bGpsCompleteCommand;

extern volatile uint8_t btIndex;
extern volatile char btBuffer[128];
extern volatile bool bbtCompleteCommand;

/*
 ** ===================================================================
 **     Event       :  Inhr1_OnRxChar (module Events)
 **
 **     Component   :  Inhr1 [AsynchroSerial]
 **     Description :
 **         This event is called after a correct character is received.
 **         The event is available only when the <Interrupt
 **         service/event> property is enabled and either the <Receiver>
 **         property is enabled or the <SCI output mode> property (if
 **         supported) is set to Single-wire mode.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Inhr1_OnRxChar(void) {
	char c;
	if (ERR_OK == Inhr1_RecvChar(&c)) {
		switch (c) {
		case '\r':
			// new line received
			if (gpsIndex > 0) {
				gpsBuffer[gpsIndex] = '\0';
				// complete_command = true;
				if (GPS.encode(&GPS, c))
					bGpsCompleteCommand = true;
			}
			break;

		default:
			// Normal characters
		{
			if (gpsIndex < 128) {
				gpsBuffer[gpsIndex] = c;
				if (GPS.encode(&GPS, c))
					bGpsCompleteCommand = true;
				gpsIndex++;
			}
		}
			break;
		}
	}
}

/*
 ** ===================================================================
 **     Event       :  Cpu_OnNMI (module Events)
 **
 **     Component   :  Cpu [MK22FN512DC12]
 */
/*!
 **     @brief
 **         This event is called when the Non maskable interrupt had
 **         occurred. This event is automatically enabled when the [NMI
 **         interrupt] property is set to 'Enabled'.
 */
/* ===================================================================*/
void Cpu_OnNMI(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Inhr2_OnRxChar (module Events)
 **
 **     Component   :  Inhr2 [AsynchroSerial]
 **     Description :
 **         This event is called after a correct character is received.
 **         The event is available only when the <Interrupt
 **         service/event> property is enabled and either the <Receiver>
 **         property is enabled or the <SCI output mode> property (if
 **         supported) is set to Single-wire mode.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Inhr2_OnRxChar(void) {
	char c;
	if (ERR_OK == Inhr2_RecvChar(&c)) {
		switch (c) {
		case '\r':
			// new line received
			if (btIndex > 0) {
				btBuffer[btIndex] = '\0';
				bbtCompleteCommand = true;

			}
			break;

		default:
			// Normal characters
		{
			if (btIndex < 128) {
				btBuffer[btIndex] = c;
				btIndex++;
			}
		}
			break;
		}
	}
}

/* END Events */

#ifdef __cplusplus
} /* extern "C" */
#endif 

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
