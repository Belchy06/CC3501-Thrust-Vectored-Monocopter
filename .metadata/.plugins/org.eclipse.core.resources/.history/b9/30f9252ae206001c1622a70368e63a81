/*
 * Helpers.c
 *
 *  Created on: Aug 25, 2021
 *      Author: willi
 */
#include "Helpers.h"

void delay(word delay_ms) {
	word time;
	FC32_Reset();

	do {
		__asm("wfi");
		FC32_GetTimeMS(&time);
	} while (time < delay_ms);
}


