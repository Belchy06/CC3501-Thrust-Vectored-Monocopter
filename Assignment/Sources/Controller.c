/*
 * Controller.c
 *
 *  Created on: Aug 30, 2021
 *      Author: willi
 */

#include "Controller.h"


void New_Controller(Controller *self) {
	PID xPid, yPid, zPid, hPid;
	self->pidX = xPid;
	self->pidY = yPid;
	self->pidZ = zPid;
	self->pidH = hPid;
	self->setGains = &setGains;
	self->init = &init;
	self->calculate = &calculate;
	self->setXPoint = &setXPoint;
	self->setYPoint = &setYPoint;
	self->setZPoint = &setZPoint;
	self->setHPoint = &setHPoint;
}

void setGains(Controller *self) {
	double p = 2;
	double i = 5;
	double d = 1;
	self->xKp = p;
	self->xKi = i;
	self->xKd = d;
	self->yKp = p;
	self->yKi = i;
	self->yKd = d;
	self->zKp = p;
	self->zKi = i;
	self->zKd = d;
	self->hKp = p;
	self->hKi = i;
	self->hKd = d;
}
void setXPoint(Controller *self, double setX) {
	self->setX = setX;
}

void setYPoint(Controller *self, double setY) {
	self->setY = setY;
}

void setZPoint(Controller *self, double setZ) {
	self->setZ = setZ;
}

void setHPoint(Controller *self, double setH) {
	self->setH = setH;
}

void init(Controller *self, double *x, double *y, double *z, double *h) {
	New_PID(&self->pidX, x, &self->outX, &self->setX, self->xKp, self->xKi, self->xKd, DIRECT);
	self->pidX.setMode(&self->pidX, AUTOMATIC);
	self->pidX.setOutputLimits(&self->pidX, 1000, 2000);

	New_PID(&self->pidY, y, &self->outY, &self->setY, self->yKp, self->yKi, self->yKd, DIRECT);
	self->pidY.setMode(&self->pidY, AUTOMATIC);
	self->pidY.setOutputLimits(&self->pidY, 1000, 2000);

	New_PID(&self->pidZ, z, &self->outZ, &self->setZ, self->zKp, self->zKi, self->zKd, DIRECT);
	self->pidZ.setMode(&self->pidZ, AUTOMATIC);
	self->pidZ.setOutputLimits(&self->pidZ, 1000, 2000);

	New_PID(&self->pidH, h, &self->outH, &self->setH, self->hKp, self->hKi, self->hKd, DIRECT);
	self->pidH.setMode(&self->pidH, AUTOMATIC);
	self->pidH.setOutputLimits(&self->pidH, 1000, 2000);
}

void calculate(Controller *self) {
	self->pidX.compute(&self->pidX);
	self->pidY.compute(&self->pidY);
	self->pidZ.compute(&self->pidZ);
	self->pidH.compute(&self->pidH);
}




