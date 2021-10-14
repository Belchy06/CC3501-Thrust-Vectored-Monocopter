/*
 * BMP384.c
 *
 *  Created on: 13 Oct 2021
 *      Author: Will
 */

#ifndef SOURCES_BMP384_C_
#define SOURCES_BMP384_C_

#include "BMP384.h"

void New_BMP384(BMP384 *self, uint8_t devAddr) {
	self->devAddr = devAddr;
	self->start = &start;
	self->setTimeStandby = &setTimeStandby;
	self->startNormalConversion = &startNormalConversion;
}

uint8_t start(BMP384 *self) {
	if (!reset(self)) {
		return 0;
	}

	byte chipId;
	self->err = readByte(self->devAddr, BMP388_CHIP_ID, &chipId);
	if (chipId != BMP388_ID && chipId != BMP390_ID)       // Check the device ID
	{
		return 0;                             // If the ID is incorrect return 0
	}

	readBytes(self->devAddr, BMP388_TRIM_PARAMS, sizeof(self->params),
			(uint8_t*) &self->params);
	self->floatParams.param_T1 = (float) self->params.param_T1 / powf(2.0f, -8.0f); // Calculate the floating point trim parameters
	self->floatParams.param_T2 = (float) self->params.param_T2 / powf(2.0f, 30.0f);
	self->floatParams.param_T3 = (float) self->params.param_T3 / powf(2.0f, 48.0f);
	self->floatParams.param_P1 = ((float) self->params.param_P1 - powf(2.0f, 14.0f))
			/ powf(2.0f, 20.0f);
	self->floatParams.param_P2 = ((float) self->params.param_P2 - powf(2.0f, 14.0f))
			/ powf(2.0f, 29.0f);
	self->floatParams.param_P3 = (float) self->params.param_P3 / powf(2.0f, 32.0f);
	self->floatParams.param_P4 = (float) self->params.param_P4 / powf(2.0f, 37.0f);
	self->floatParams.param_P5 = (float) self->params.param_P5 / powf(2.0f, -3.0f);
	self->floatParams.param_P6 = (float) self->params.param_P6 / powf(2.0f, 6.0f);
	self->floatParams.param_P7 = (float) self->params.param_P7 / powf(2.0f, 8.0f);
	self->floatParams.param_P8 = (float) self->params.param_P8 / powf(2.0f, 15.0f);
	self->floatParams.param_P9 = (float) self->params.param_P9 / powf(2.0f, 48.0f);
	self->floatParams.param_P10 = (float) self->params.param_P10 / powf(2.0f, 48.0f);
	self->floatParams.param_P11 = (float) self->params.param_P11 / powf(2.0f, 65.0f);
	setIIRFilter(self, IIR_FILTER_OFF);
	setTimeStandby(self, TIME_STANDBY_5MS);
	setOversamplingRegister(self, OVERSAMPLING_X16, OVERSAMPLING_X2);
	self->pwr_ctrl.bit.press_en = 1;
	self->pwr_ctrl.bit.temp_en = 1;
	setMode(self, SLEEP_MODE);
	return 1;
}

uint8_t reset(BMP384 *self) {
	self->err = writeByte(self->devAddr, BMP388_CMD, RESET_CODE);
	for (uint32_t i = 0; i < 1000000; i++) {
		__asm("nop");
	}
	uint8_t data;
	self->err = readByte(self->devAddr, BMP388_EVENT, &data);
	self->event.reg = data;
	return self->event.bit.por_detected;
}

void setIIRFilter(BMP384 *self, IIRFilter iirFilter) {
	self->config.bit.iir_filter = iirFilter;
	self->err = writeByte(self->devAddr, BMP388_CONFIG, self->config.reg);
}

void setTimeStandby(BMP384 *self, TimeStandby timeStandby) {
	self->odr.bit.odr_sel = timeStandby;
	self->err = writeByte(self->devAddr, BMP388_ODR, self->odr.reg);
}

void setOversamplingRegister(BMP384 *self, Oversampling presOversampling, Oversampling tempOversampling) {
	self->osr.reg = tempOversampling << 3 | presOversampling;
	self->err = writeByte(self->devAddr, BMP388_OSR, self->osr.reg);
}

void setMode(BMP384 *self, Mode mode) {
	self->pwr_ctrl.bit.mode = mode;
	self->err = writeByte(self->devAddr, BMP388_PWR_CTRL, self->pwr_ctrl.reg);
}
void startNormalConversion(BMP384 *self) {
	setMode(self, NORMAL_MODE);
}

#endif /* SOURCES_BMP384_C_ */