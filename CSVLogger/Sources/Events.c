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

/* User includes (#include below this line is not maintained by Processor Expert) */
extern volatile GPS gps;
extern volatile uint8_t index;
extern volatile char buffer[128];
extern volatile bool complete_command;
extern bool debug;

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
	/* Write your code here ... */
	/* Write your code here ... */
	char c;
	if (ERR_OK == Inhr1_RecvChar(&c)) {
		switch (c) {
		case '\r':
			// new line received
			if (index > 0) {
				buffer[index] = '\0';
				if (debug)
					PC_SendChar(c);
				// complete_command = true;
				if (gps.encode(&gps, c)) {
					if (gps._sentence_type == _GPS_SENTENCE_GPRMC) {
						complete_command = true;
					} else {
						index = 0;
					}
				}

			}
			break;

		case 0x7F:
			// Backspace
		{
			if (index > 0) {
				buffer[index] = '\0';
				index--;
				if (debug)
					PC_SendChar(0x7f);
			}
		}
			break;

		default:
			// Normal characters
		{
			if (index < 256) {
				buffer[index] = c;
				if (gps.encode(&gps, c)) {
					// Check if it's a GPRMC sentence type
					if (gps._sentence_type == _GPS_SENTENCE_GPRMC) {
						complete_command = true;
					}
				}
				index++;
				if (debug)
					PC_SendChar(c);
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
