/*
 * PID.c
 *
 *  Created on: 27 Aug 2021
 *      Author: Will
 */
#include "PID.h"

void New_PID(PID *self, double* Input, double* Output, double* Setpoint,
		double Kp, double Ki, double Kd, int ControllerDirection) {
	self->myOutput = Output;
	self->myInput = Input;
	self->mySetpoint = Setpoint;
	self->inAuto = false;
	setOutputLimits(self, 1000, 2000); // Default pwm limits
	self->SampleTime = 100;
	setControllerDirection(self, ControllerDirection);
	setTunings(self, Kp, Ki, Kd);
	self->initialize = &initialize;
	self->setMode = &setMode;
	self->compute = &compute;
	self->setOutputLimits = &setOutputLimits;
	self->setTunings = &setTunings;
	self->setSampleTime = &setSampleTime;
}

void initialize(PID *self) {
	self->outputSum = *self->myOutput;
	self->lastInput = *self->myInput;
	if (self->outputSum > self->outMax)
		self->outputSum = self->outMax;
	else if (self->outputSum < self->outMin)
		self->outputSum = self->outMin;
}

void setMode(PID *self, int Mode) {
	bool newAuto = (Mode == AUTOMATIC);
	if (newAuto && !!self->inAuto) {
		// We just went from manual to auto mode
		initialize(self);
	}
	self->inAuto = newAuto;
}

bool compute(PID *self) {
	if (!self->inAuto)
		return false;

	double input = *self->myInput;
	double error = *self->mySetpoint - input;
	double dInput = input - self->lastInput;
	self->outputSum += self->ki * error;

	/*Add Proportional on Measurement, if P_ON_M is specified*/
	if (!self->pOnE)
		self->outputSum -= self->kp * dInput;

	if (self->outputSum > self->outMax)
		self->outputSum = self->outMax;
	else if (self->outputSum < self->outMin)
		self->outputSum = self->outMin;

	/*Add Proportional on Error, if P_ON_E is specified*/
	double output;
	if (self->pOnE)
		output = self->kp * error;
	else
		output = 0;

	/*Compute Rest of PID Output*/
	output += self->outputSum - self->kd * dInput;

	if (output > self->outMax)
		output = self->outMax;
	else if (output < self->outMin)
		output = self->outMin;
	*self->myOutput = output;

	/*Remember some variables for next time*/
	self->lastInput = input;
	return true;
}

void setOutputLimits(PID *self, double Min, double Max) {
	if (Min > Max)
		return;

	self->outMin = Min;
	self->outMax = Max;

	if (self->inAuto) {
		if (*self->myOutput > Max)
			*self->myOutput = Max;
		else if (*self->myOutput < Min)
			*self->myOutput = Min;

		if (self->outputSum > Max)
			self->outputSum = Max;
		else if (self->outputSum < Min)
			self->outputSum = Min;
	}
}

void setTunings(PID *self, double Kp, double Ki, double Kd) {
	if (Kp < 0 || Ki < 0 || Kd < 0)
		return;

	if (self->controllerDirection == REVERSE) {
		self->kp = -Kp;
		self->ki = -Ki;
		self->kd = -Kd;
	} else {
		self->kp = Kp;
		self->ki = Ki;
		self->kd = Kd;
	}

}

void setControllerDirection(PID *self, int Direction) {
	if (self->inAuto && self->controllerDirection != Direction) {
		self->kp = 0 - self->kp;
		self->ki = 0 - self->ki;
		self->kd = 0 - self->kd;
	}
	self->controllerDirection = Direction;
}

void setSampleTime(PID *self, int NewSampleTime) {
	if (NewSampleTime > 0) {
		double ratio = (double) NewSampleTime / (double) self->SampleTime;
		self->ki *= ratio;
		self->kd /= ratio;
		self->SampleTime = (unsigned long) NewSampleTime;
	}
}
