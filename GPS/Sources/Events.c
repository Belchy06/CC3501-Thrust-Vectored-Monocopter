/* ###################################################################
 **     Filename    : Events.c
 **     Project     : GPS
 **     Processor   : MK22FN512VDC12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2021-09-18, 16:50, # CodeGen: 0
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Contents    :
 **         Cpu_OnNMI - void Cpu_OnNMI(void);
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

/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdbool.h>
#include "Sensors.h"

extern volatile char buffer[128];
extern volatile uint8 index;
extern volatile bool complete_command;
extern GPS gps;
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
 **     Event       :  PC_OnError (module Events)
 **
 **     Component   :  PC [AsynchroSerial]
 **     Description :
 **         This event is called when a channel error (not the error
 **         returned by a given method) occurs. The errors can be read
 **         using <GetError> method.
 **         The event is available only when the <Interrupt
 **         service/event> property is enabled.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void PC_OnError(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  PC_OnRxChar (module Events)
 **
 **     Component   :  PC [AsynchroSerial]
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
void PC_OnRxChar(void) {
	/* Write your code here ... */
	char c;
	if (ERR_OK == PC_RecvChar(&c)) {
		switch (c) {
		case '\r':
			// new line received
			if (index > 0) {
				buffer[index] = '\0';
				PC_SendChar(c);
				// complete_command = true;
				if(gps.encode(&gps, c)) complete_command = true;
			}
		break;

		case 0x7F:
			// Backspace
		{
			if (index > 0) {
				buffer[index] = '\0';
				index--;
				PC_SendChar(0x7f);
			}
		}
		break;

		default:
			// Normal characters
		{
			if (index < 128) {
				buffer[index] = c;
				if(gps.encode(&gps, c)) complete_command = true;
				index++;
				PC_SendChar(c);
			}
		}
		break;
		}
	}
}

/*
 ** ===================================================================
 **     Event       :  PC_OnTxChar (module Events)
 **
 **     Component   :  PC [AsynchroSerial]
 **     Description :
 **         This event is called after a character is transmitted.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void PC_OnTxChar(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  PC_OnFullRxBuf (module Events)
 **
 **     Component   :  PC [AsynchroSerial]
 **     Description :
 **         This event is called when the input buffer is full;
 **         i.e. after reception of the last character
 **         that was successfully placed into input buffer.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void PC_OnFullRxBuf(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  PC_OnFreeTxBuf (module Events)
 **
 **     Component   :  PC [AsynchroSerial]
 **     Description :
 **         This event is called after the last character in output
 **         buffer is transmitted.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void PC_OnFreeTxBuf(void) {
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
