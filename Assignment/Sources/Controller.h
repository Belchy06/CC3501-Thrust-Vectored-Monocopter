/*
 * Controller.h
 *
 *  Created on: Aug 30, 2021
 *      Author: willi
 */

#ifndef SOURCES_CONTROLLER_H_
#define SOURCES_CONTROLLER_H_

#include "PID.h"
#include "Sensors.h"

void New_Controller(Controller *self);
void setGains(Controller *self);
void init(Controller *self, double *x, double *y, double *z, double *h);
void calculate(Controller *self);
void setXPoint(Controller *self, double setX);
void setYPoint(Controller *self, double setY);
void setZPoint(Controller *self, double setZ);
void setHPoint(Controller *self, double setH);

#endif /* SOURCES_CONTROLLER_H_ */
