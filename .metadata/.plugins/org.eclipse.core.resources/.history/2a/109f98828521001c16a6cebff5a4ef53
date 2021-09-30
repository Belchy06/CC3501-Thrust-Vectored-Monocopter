/*
 * VelocityEstimator.h
 *
 *  Created on: 30 Sep 2021
 *      Author: Will
 */

#ifndef SOURCES_VELOCITYESTIMATOR_H_
#define SOURCES_VELOCITYESTIMATOR_H_

#include "Controllers.h"
#include <math.h>
#include <string.h>
#include <stdint.h>

void initializeVelocityEstimator(VelocityEstimator *self);
void estimateVelocity(VelocityEstimator *self, const float measurements[4], const float delta_t);
void getVelocityEstimate(VelocityEstimator *self, float *xVel, float *yVel);


#endif /* SOURCES_VELOCITYESTIMATOR_H_ */
