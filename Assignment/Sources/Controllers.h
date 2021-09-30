/*
 * Controllers.h
 *
 *  Created on: 30 Sep 2021
 *      Author: Will
 */

#ifndef SOURCES_CONTROLLERS_H_
#define SOURCES_CONTROLLERS_H_

typedef struct VelocityEstimator {
	float x[4];
	float P[16];

	void (*estimateVelocity)(struct VelocityEstimator *self, const float measurements[4], const float delta_t);
	void (*getVelocityEstimate)(struct VelocityEstimator *self, float *xVel, float *yVel);
} VelocityEstimator;

typedef struct PositionEstimator {
	float x[4];
	float P[16];

	void (*estimatePosition)(struct PositionEstimator *self, const float measurements[4], const float delta_t);
	void (*getPositionEstimate)(struct PositionEstimator *self, float *xPos, float *yPos);
} PositionEstimator ;



#endif /* SOURCES_CONTROLLERS_H_ */
