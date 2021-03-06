/* ###################################################################
**     Filename    : Events.h
**     Project     : Lab_03
**     Processor   : MK22FN512VDC12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2021-08-13, 13:30, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMI - void Cpu_OnNMI(void);
**
** ###################################################################*/
/*!
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Pins1.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "MCUC1.h"
#include "BT.h"
#include "Inhr2.h"
#include "ASerialLdd2.h"
#include "Inhr1.h"
#include "ASerialLdd1.h"
#include "statusBlue.h"
#include "BitIoLdd1.h"
#include "statusRed.h"
#include "BitIoLdd2.h"
#include "statusGreen.h"
#include "BitIoLdd3.h"
#include "EDF.h"
#include "FC321.h"
#include "RealTimeLdd1.h"
#include "TU1.h"
#include "WAIT1.h"
#include "Pwm1.h"
#include "PwmLdd1.h"
#include "TU3.h"
#include "xPos.h"
#include "Pwm2.h"
#include "PwmLdd2.h"
#include "xNeg.h"
#include "Pwm3.h"
#include "PwmLdd3.h"
#include "yPos.h"
#include "Pwm4.h"
#include "PwmLdd4.h"
#include "yNeg.h"
#include "Pwm5.h"
#include "PwmLdd5.h"
#include "GPSTimer.h"
#include "RealTimeLdd2.h"
#include "TU2.h"
#include "GPS.h"

#ifdef __cplusplus
extern "C" {
#endif 

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
void Cpu_OnNMI(void);

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
void Inhr1_OnRxChar(void);

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
void Inhr2_OnRxChar(void);

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
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
