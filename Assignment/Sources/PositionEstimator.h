/*
 * PositionEstimator.h
 *
 *  Created on: 30 Sep 2021
 *      Author: Will
 */

#ifndef SOURCES_POSITIONESTIMATOR_H_
#define SOURCES_POSITIONESTIMATOR_H_

#include "Controllers.h"
#include <math.h>
#include <string.h>
#include <stdint.h>

void initializePositionEstimator(PositionEstimator *self);
void estimatePosition(PositionEstimator *self, const float measurements[4], const float delta_t);
void getPositionEstimate(PositionEstimator *self, float *xVel, float *yVel);


#endif /* SOURCES_POSITIONESTIMATOR_H_ */
