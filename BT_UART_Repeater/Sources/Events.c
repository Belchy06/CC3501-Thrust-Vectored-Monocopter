/* ###################################################################
 **     Filename    : Events.c
 **     Project     : BT_UART_Repeater
 **     Processor   : MK22FN512VDC12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2021-10-17, 11:39, # CodeGen: 0
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
 **     Event       :  Board_OnError (module Events)
 **
 **     Component   :  Board [AsynchroSerial]
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
void Board_OnError(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Board_OnRxChar (module Events)
 **
 **     Component   :  Board [AsynchroSerial]
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
void Board_OnRxChar(void) {
	/* Write your code here ... */
	char c;
	Board_RecvChar(&c);
	PuTTY_SendChar(c);
}

/*
 ** ===================================================================
 **     Event       :  Board_OnTxChar (module Events)
 **
 **     Component   :  Board [AsynchroSerial]
 **     Description :
 **         This event is called after a character is transmitted.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Board_OnTxChar(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Board_OnFullRxBuf (module Events)
 **
 **     Component   :  Board [AsynchroSerial]
 **     Description :
 **         This event is called when the input buffer is full;
 **         i.e. after reception of the last character
 **         that was successfully placed into input buffer.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Board_OnFullRxBuf(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  Board_OnFreeTxBuf (module Events)
 **
 **     Component   :  Board [AsynchroSerial]
 **     Description :
 **         This event is called after the last character in output
 **         buffer is transmitted.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void Board_OnFreeTxBuf(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  PuTTY_OnError (module Events)
 **
 **     Component   :  PuTTY [AsynchroSerial]
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
void PuTTY_OnError(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  PuTTY_OnRxChar (module Events)
 **
 **     Component   :  PuTTY [AsynchroSerial]
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
void PuTTY_OnRxChar(void) {
	/* Write your code here ... */
	char c;
	PuTTY_RecvChar(&c);
	PuTTY_SendChar(c);
	Board_SendChar(c);
}

/*
 ** ===================================================================
 **     Event       :  PuTTY_OnTxChar (module Events)
 **
 **     Component   :  PuTTY [AsynchroSerial]
 **     Description :
 **         This event is called after a character is transmitted.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void PuTTY_OnTxChar(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  PuTTY_OnFullRxBuf (module Events)
 **
 **     Component   :  PuTTY [AsynchroSerial]
 **     Description :
 **         This event is called when the input buffer is full;
 **         i.e. after reception of the last character
 **         that was successfully placed into input buffer.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void PuTTY_OnFullRxBuf(void) {
	/* Write your code here ... */
}

/*
 ** ===================================================================
 **     Event       :  PuTTY_OnFreeTxBuf (module Events)
 **
 **     Component   :  PuTTY [AsynchroSerial]
 **     Description :
 **         This event is called after the last character in output
 **         buffer is transmitted.
 **     Parameters  : None
 **     Returns     : Nothing
 ** ===================================================================
 */
void PuTTY_OnFreeTxBuf(void) {
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
