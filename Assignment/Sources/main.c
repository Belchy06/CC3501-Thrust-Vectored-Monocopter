/* ###################################################################
 **     Filename    : main.c
 **     Project     : Lab_03
 **     Processor   : MK22FN512VDC12
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2021-08-13, 13:30, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @version 01.01
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
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
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "Sensors.h"
#include "BNO085.h"
#include "BMP384.h"
#include "GPS.h"
#include "printf.h"
#include "Controllers.h"
#include "VelocityEstimator.h"
#include "PositionEstimator.h"
#include "utm.h"

#define BNO080_DEFAULT_ADDRESS 0x4A
#define BMP384_DEFAULT_ADDRESS 0x77
#define MAX_DEGREES 30
#define MAX_POSITION 20
#define MAX_ALTITUDE 50
#define CALIBRATION_COUNT 100
#define GYRO_PRECESSION 0.9

// Barometer Variables
BMP384 BARO;
float temperature, pressure, altitude;
float altitudeInitial;
float altitudeAGL;
// IMU Variables
BNO085 IMU;
float pitch, roll, yaw;
float p, q, r;
float lx, ly, lz;
uint8_t gyroAccuracy, accAccuracy;
// Generic Variables 
char outputBuffer[256];
unsigned int time;
float dt;
float xOff, yOff;
float xMps, yMps, zMps = 0;
bool bstatusBlue, bstatusRed, bstatusGreen = false;
bool bFirstRun = true;
bool bCalculateLandingHeight = true;
float timeSinceLandingStarted;
// GPS Variables
volatile NEO6M GPS;
float lat, lon, prevLat, prevLon, distance;
float gYaw;
unsigned long age;
float mps;
bool haveGps = false;
double easting;
double northing;
// GPS ISR Variables
volatile uint8_t gpsIndex;
volatile char gpsBuffer[128];
volatile bool bGpsCompleteCommand = false;
// Bluetooth ISR Variables
volatile uint8_t btIndex;
volatile char btBuffer[128];
volatile bool bbtCompleteCommand = false;
// Controller Variables
VelocityEstimator EstimatorVelocity;
PositionEstimator EstimatorPosition;
// World Reference Frame Values
float worldAcceleration[3] = { 0, 0, 0 };
float worldVelocity[3] = { 0, 0, 0 };
// Control Values
float tau_yaw, tau_pitch, tau_roll = 0;
float altitude_cmd = 0;
float w0 = 1.108f * 1.790f;
// Servo Variables
// -x, +x, -y, +y, edf
float servoVals[5] = { 0, 0, 0, 0, 0 };
//                   total thrust, yaw, pitch, roll
// float servoWeights[4] = { 0.4, 1.57, 1.57, 1.57 };
float servoWeights[4] = { 0.2, 0, 5, 5 };
// Structs
struct setpoints {
	float x;
	float y;
	float altitude;
	float yaw;
	float pitch;
	float roll;
} setpoint;

struct GPSStats {
	float dx;
	float dy;
	float x;
	float y;
	float prevEasting;
	float prevNorthing;
} GPSstats;
// State Machine 
typedef enum {
	START,
	CALIBRATING,
	IDLE,
	ASCENT,
	ATTITUDE_HOVER,
	POSITION_AND_ATTITUDE_HOVER,
	LANDING,
	ERROR
} state_t;
typedef enum {
	GREEN, BLUE, RED
} LEDS_t;

state_t state = START;

float clamp(float f, float min, float max) {
	float t = f < min ? min : f;
	return t > max ? max : t;
}

bool initializeSensors() {
	/*
	 * Barometer start and configuration
	 */
	if (!BARO.start(&BARO)) {
		BT_SendStr("Error initializing barometer!\r\n");
		ledOn(RED);
		return false;
	}
	BARO.setTimeStandby(&BARO, TIME_STANDBY_40MS);
	BARO.startNormalConversion(&BARO);
	BARO.setIIRFilter(&BARO, IIR_FILTER_4);

	/*
	 * IMU start and configuration
	 */
	IMU.begin(&IMU);
	if (IMU.begin(&IMU) == false) {
		BT_SendStr("Error initializing IMU!\r\n");
		ledOn(RED);
		return false;
	}
	IMU.enableLinearAccelerometer(&IMU, 40);
	IMU.enableGameRotationVector(&IMU, 40);
	IMU.enableGyro(&IMU, 40);

	return true;
}

void ledToggle(LEDS_t led) {
	switch (led) {
	case GREEN:
		bstatusGreen = !bstatusGreen;
		statusGreen_PutVal(bstatusGreen);
		break;
	case BLUE:
		bstatusBlue = !bstatusBlue;
		statusBlue_PutVal(bstatusBlue);
		break;
	case RED:
		bstatusRed = !bstatusRed;
		statusRed_PutVal(bstatusRed);
		break;
	}
}

void ledOn(LEDS_t led) {
	switch (led) {
	case GREEN:
		bstatusGreen = true;
		statusGreen_PutVal(bstatusGreen);
		break;
	case BLUE:
		bstatusBlue = true;
		statusBlue_PutVal(bstatusBlue);
		break;
	case RED:
		bstatusRed = true;
		statusRed_PutVal(bstatusRed);
		break;
	}
}

void ledOff(LEDS_t led) {
	switch (led) {
	case GREEN:
		bstatusGreen = false;
		statusGreen_PutVal(bstatusGreen);
		break;
	case BLUE:
		bstatusBlue = false;
		statusBlue_PutVal(bstatusBlue);
		break;
	case RED:
		bstatusRed = false;
		statusRed_PutVal(bstatusRed);
		break;
	}
}

void calibrateAltitude() {
	float tempAlt = 0;
	BT_SendStr("Calibrating starting altitude...\r\n");
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++) {
		while (!BARO.getMeasurements(&BARO, &temperature, &pressure, &altitude)) {
			;
		}
		tempAlt += altitude;
		ledToggle(BLUE);
	}
	setpoint.altitude = tempAlt / CALIBRATION_COUNT;
	altitudeInitial = setpoint.altitude;
	bstatusBlue = false;
	statusBlue_PutVal(bstatusBlue);
	snprintf(outputBuffer, 128, "Starting altitude: %f\r\n", setpoint.altitude);
	BT_SendStr(outputBuffer);
}

void calibrateYaw() {
	float tempYaw = 0;
	BT_SendStr("Calibrating starting yaw...\r\n");
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++) {
		while (!IMU.dataAvailable(&IMU)) {
			;
		}
		tempYaw += (float) (IMU.getYaw(&IMU)) * 180.0 / 3.14159f; // Convert yaw to degrees
		ledToggle(GREEN);
	}
	setpoint.yaw = tempYaw / CALIBRATION_COUNT;
	snprintf(outputBuffer, 128, "Starting yaw: %f\r\n", setpoint.yaw);
	BT_SendStr(outputBuffer);
}

void calibratePitch() {
	float tempPitch= 0;
	BT_SendStr("Calibrating starting pitch...\r\n");
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++) {
		while (!IMU.dataAvailable(&IMU)) {
			;
		}
		tempPitch += (float) (IMU.getPitch(&IMU)) * 180.0 / 3.14159f; // Convert yaw to degrees
		ledToggle(GREEN);
	}
	setpoint.pitch = tempPitch / CALIBRATION_COUNT;
	snprintf(outputBuffer, 128, "Starting pitch: %f\r\n", setpoint.pitch);
	BT_SendStr(outputBuffer);
}

void calibrateRoll() {
	float tempRoll = 0;
	BT_SendStr("Calibrating starting roll...\r\n");
	for (uint8_t i = 0; i < CALIBRATION_COUNT; i++) {
		while (!IMU.dataAvailable(&IMU)) {
			;
		}
		tempRoll += (float) (IMU.getRoll(&IMU)) * 180.0 / 3.14159f; // Convert yaw to degrees
		ledToggle(GREEN);
	}
	setpoint.roll = tempRoll / CALIBRATION_COUNT;
	snprintf(outputBuffer, 128, "Starting roll: %f\r\n", setpoint.roll);
	BT_SendStr(outputBuffer);
}

void startupServos() {
	xPos_SetPWMDutyUs(1000);
	xNeg_SetPWMDutyUs(1000);
	yPos_SetPWMDutyUs(1000);
	yNeg_SetPWMDutyUs(1000);
	// Wait
	WAIT1_Waitms(500);
	xPos_SetPWMDutyUs(2000);
	xNeg_SetPWMDutyUs(2000);
	yPos_SetPWMDutyUs(2000);
	yNeg_SetPWMDutyUs(2000);
	// Wait
	WAIT1_Waitms(500);
	xPos_SetPWMDutyUs(1500);
	xNeg_SetPWMDutyUs(1500);
	yPos_SetPWMDutyUs(1500);
	yNeg_SetPWMDutyUs(1500);
}

void readIMU() {
	if (IMU.dataAvailable(&IMU)) {
		// Get Euler angle orientation 
		yaw = (float) (IMU.getYaw(&IMU)) * 180.0 / 3.14159f; // Convert yaw to degrees
		pitch = (float) (IMU.getPitch(&IMU)) * 180.0 / 3.14159; // Convert pitch to degrees
		roll = (float) (IMU.getRoll(&IMU)) * 180.0 / 3.14159f; // Convert roll to degrees
		// Get body frame acceleration (gravity removed)
		IMU.getLinAccel(&IMU, &lx, &ly, &lz, &accAccuracy);
		// Get body frame angular acceleration
		IMU.getGyro(&IMU, &p, &q, &r, &gyroAccuracy);
		ledToggle(GREEN);
	}
}

void readBarometer() {
	if (BARO.getMeasurements(&BARO, &temperature, &pressure, &altitude)) {
		ledToggle(BLUE);
	}
}

void checkSensorErrors() {
	if (BARO.err || IMU.err) {
		ledOn(RED);
		if (BARO.err) {
			// Barometer Error
			BT_SendStr("Barometer error!\r\n");
		} else {
			// IMU error
			BT_SendStr("IMU error!\r\n");
		}
		state = ERROR;
	}
}

void readGPS() {
	if (bGpsCompleteCommand) {
		GPS.get_position(&GPS, &lat, &lon, &age);
		long zone;
		char hemisphere;
		long res = Convert_Geodetic_To_UTM(lat * 0.0174533f, lon * 0.0174533f,
				&zone, &hemisphere, &easting, &northing);
		if (res == UTM_NO_ERROR) {
			GPSstats.x = (float) (GPSstats.prevEasting - easting);
			GPSstats.y = (float) (GPSstats.prevNorthing - northing);
			GPSstats.prevEasting = easting;
			GPSstats.prevNorthing = northing;
			if (!bFirstRun) {
				float gYaw = GPS.course_to(&GPS, prevLat, prevLon, lat, lon);
				// Velocity
				mps = GPS.speed_mps(&GPS);
				if (mps != -1.0) {
					GPSstats.dx = mps * cosf(gYaw);
					GPSstats.dy = mps * sinf(gYaw);
				}
			}
			prevLat = lat;
			prevLon = lon;
			bFirstRun = false;
			haveGps = true;
		} else {
			haveGps = false;
		}
		bGpsCompleteCommand = false;
		gpsIndex = 0;
	}
}

void calculateWorldAcceleration() {
	/*
	 * Intertial to World Acceleration Handling
	 */
	float dcm[3][3];
	float phi = yaw * 0.0174533;
	float theta = pitch * 0.0174533;
	float psi = roll * 0.0174533;
	// Direction Cosine Matrix (DCM) (How to go from world to body)
	dcm[0][0] = cosf(theta) * cosf(phi);
	dcm[0][1] = cosf(theta) * sinf(phi);
	dcm[0][2] = -sinf(theta);
	dcm[1][0] = sinf(psi) * sinf(theta) * cosf(phi) - cosf(psi) * sinf(phi);
	dcm[1][1] = sinf(psi) * sinf(theta) * sinf(phi) + cosf(psi) * cosf(phi);
	dcm[1][2] = sinf(psi) * cosf(theta);
	dcm[2][0] = cosf(psi) * sinf(theta) * cosf(phi) + sinf(psi) * sinf(phi);
	dcm[2][1] = cosf(psi) * sinf(theta) * sinf(phi) - sinf(psi) * cosf(phi);
	dcm[2][2] = cosf(psi) * cosf(theta);
	// Calculate DCM transpose (How to go from body to world)
	float dcmT[3][3];
	for (uint8_t i = 0; i < 3; ++i) {
		for (uint8_t j = 0; j < 3; ++j) {
			dcmT[j][i] = dcm[i][j];
		}
	}

	worldAcceleration[0] = dcmT[0][0] * lx + dcmT[0][1] * ly + dcmT[0][2] * lz;
	worldAcceleration[1] = dcmT[1][0] * lx + dcmT[1][1] * ly + dcmT[1][2] * lz;
	worldAcceleration[2] = dcmT[2][0] * lx + dcmT[2][1] * ly + dcmT[2][2] * lz;
}

void estimateWorldVelocity() {
	/*
	 * VELOCITY ESTIMATE
	 */
	if (haveGps) {
		float measurements[4] = { GPSstats.dx, GPSstats.dy,
				worldAcceleration[0], worldAcceleration[1] };
		EstimatorVelocity.estimateVelocity(&EstimatorVelocity, measurements,
				dt);
		EstimatorVelocity.getVelocityEstimate(&EstimatorVelocity, &xMps, &yMps);
	} else {
		float measurements[4] = { 999999999, 999999999, worldAcceleration[0],
				worldAcceleration[1] };
		EstimatorVelocity.estimateVelocity(&EstimatorVelocity, measurements,
				dt);
		EstimatorVelocity.getVelocityEstimate(&EstimatorVelocity, &xMps, &yMps);
	}
}

void estimateWorldPosition() {
	/*
	 * World Position Estimate
	 */
	worldVelocity[0] = xMps;
	worldVelocity[1] = yMps;
	worldVelocity[2] += worldAcceleration[2] * dt;
	if (haveGps) {
		float measurements[4] = { GPSstats.x, GPSstats.y, xMps, yMps };
		EstimatorPosition.estimatePosition(&EstimatorPosition, measurements,
				dt);
		EstimatorPosition.getPositionEstimate(&EstimatorPosition, &xOff, &yOff);
	} else {
		float measurements[4] = { 999999999, 999999999, xMps, yMps };
		EstimatorPosition.estimatePosition(&EstimatorPosition, measurements,
				dt);
		EstimatorPosition.getPositionEstimate(&EstimatorPosition, &xOff, &yOff);
	}
}

void calculatePitchRollSetpoint() {
	/*
	 * PD Controller to calculate the orientation the vehicle should be in in order to correct for its positional error
	 */
	float XYErr[2] = { (setpoint.x - xOff), (setpoint.y - yOff) };
	float P = 0.024;
	float P_xy[2] = { -P
			* clamp((XYErr[0] * cosf(yaw) - XYErr[1] * sinf(yaw)), -3, 3), P
			* clamp((XYErr[0] * sinf(yaw) - XYErr[1] * cosf(yaw)), -3, 3) };
	float D = 0.1;
	float D_xy[2] = { D * xMps, -D * yMps };
	setpoint.pitch = P_xy[0] + D_xy[0];
	setpoint.roll = P_xy[1] + D_xy[1];
}

void calculatePitchRollTorque() {
	/*
	 * Attitude Controller
	 */
	float PRErr[2] = { (setpoint.pitch - pitch), (setpoint.roll - roll) };
	// Proportional
	float P[2] = { 0.02, 0.02 };
	float P_pr[2] = { P[0] * PRErr[0], P[1] * PRErr[1] };
	// Integral
	float I = 0.0015;
	float I_pr[2] = { I * clamp(PRErr[0] * dt, -2, 2), I
			* clamp(PRErr[1] * dt, -2, 2) };
	// Derivative
	float D[2] = { 0.015, 0.015 };
	// x, y, z
	// p, q, r
	float D_pr[2] = { D[0] * q, D[1] * p };

	tau_pitch = clamp(P_pr[0] + I_pr[0] - D_pr[0], -0.768, 0.768);
	tau_roll = clamp(P_pr[1] + I_pr[1] - D_pr[1], -0.768, 0.768);
}

void calculateYawTorque() {
	float yawErr = setpoint.yaw - yaw;
	float P = 0.004;
	float P_yaw = P * yawErr;
	float D = -0.03 * 0.004;
	float D_yaw = D * r;
	tau_yaw = P_yaw + D_yaw;
}

void calculateThrust() {
	if (state == POSITION_AND_ATTITUDE_HOVER || state == ATTITUDE_HOVER) {
		// We're in a hovering state, try and maintain altitude
		float altErr = setpoint.altitude - altitude;
		float P = 0.4f;
		float P_alt = P * altErr;
		float D = 0.15f;
		float D_alt = D * worldVelocity[2];
		altitude_cmd = P_alt - D_alt;
	} else {
		// We're trying to take off, add some thrust to our minimum required hovering thrust
		altitude_cmd = w0 * 0.55f;
	}
	altitude_cmd += w0;
	altitude_cmd = clamp(altitude_cmd, 0, 3.0f);
}

float convertTorqueToAngle(float tau) {
	if(altitude_cmd == 0) {
		return 0;
	}
	return asinf(clamp(tau / (0.38 * altitude_cmd), -1, 1)) * 180 / 3.14159f;
}

void calculateServoValues() {
	/*
	 * Servo mixing
	 */
	float thrustComp, yawComp, pitchComp, rollComp = 0;
	thrustComp = servoWeights[0] * altitude_cmd;
	yawComp = servoWeights[1] * convertTorqueToAngle(tau_yaw);
	pitchComp = servoWeights[2] * convertTorqueToAngle(tau_pitch);
	rollComp = servoWeights[3] * convertTorqueToAngle(tau_roll);

	float gyro_precessionX = 1;
	float gyro_precessionY = 1;
	servoVals[0] = thrustComp - pitchComp + yawComp + p * gyro_precessionX;
	servoVals[1] = thrustComp + pitchComp + yawComp - p * gyro_precessionX;
	servoVals[2] = thrustComp + rollComp + yawComp + q * gyro_precessionY;
	servoVals[3] = thrustComp - rollComp + yawComp - q * gyro_precessionY;

	servoVals[4] = altitude_cmd;
	for (uint8_t i = 0; i < 4; i++) {
		servoVals[i] = 1500 + clamp(servoVals[i], -200, 200);
	}
	servoVals[4] = (clamp(servoVals[4] * 362.32f, 0, 1000)) + 1000;
	snprintf(outputBuffer, 256,
			"y_c: %f, p_c: %f, r_c: %f, y: %f, p: %f, r: %f, throttle: %f\r\n", yawComp,
			pitchComp, rollComp, yaw, pitch, roll, servoVals[4]);
	BT_SendStr(outputBuffer);
}

void writeServos() {
	xPos_SetPWMDutyUs(servoVals[0]);
	xNeg_SetPWMDutyUs(servoVals[1]);
	yPos_SetPWMDutyUs(servoVals[2]);
	yNeg_SetPWMDutyUs(servoVals[3]);
	EDF_SetPWMDutyUs(servoVals[4]);
}

void zeroServos() {
	xPos_SetPWMDutyUs(1500);
	xNeg_SetPWMDutyUs(1500);
	yPos_SetPWMDutyUs(1500);
	yNeg_SetPWMDutyUs(1500);
	EDF_SetPWMDutyUs(1000);
}

float sampleLandingFunction(unsigned int time) {
	// Function is the sigmoid flipped around the y axis, scaled so that the maximum value is the altitude above the starting altitude
	// The -0.025 ensures the function crosses the x-axis
	return altitudeAGL * (1 / (1 + expf(0.7 * (time - altitudeAGL)))) - 0.025;
}

void handleBTCommands() {
	if (bbtCompleteCommand) {
		char* element = strtok(btBuffer, " ");
		while (element != NULL) {
			int temp;
			if (sscanf(element, "y%d", &temp) > 0) {
				if (abs(temp) <= MAX_DEGREES) {
					setpoint.yaw = temp;
					snprintf(outputBuffer, 256, "New yaw setpoint: %f\r\n",
							setpoint.yaw);
					BT_SendStr(outputBuffer);
				}
			} else if (sscanf(element, "p%d", &temp) > 0) {
				if (abs(temp) <= MAX_DEGREES) {
					setpoint.pitch = temp;
					snprintf(outputBuffer, 256, "New pitch setpoint: %f\r\n",
							setpoint.pitch);
					BT_SendStr(outputBuffer);
				}
			} else if (sscanf(element, "r%d", &temp) > 0) {
				if (abs(temp) <= MAX_DEGREES) {
					setpoint.roll = temp;
					snprintf(outputBuffer, 256, "New roll setpoint: %f\r\n",
							setpoint.roll);
					BT_SendStr(outputBuffer);
				}
			} else if (sscanf(element, "X%d", &temp) > 0) {
				if (abs(temp) <= MAX_POSITION) {
					setpoint.x = temp;
					snprintf(outputBuffer, 256, "New Eastings setpoint: %f\r\n",
							setpoint.x);
					BT_SendStr(outputBuffer);
				}
			} else if (sscanf(element, "Y%d", &temp) > 0) {
				if (abs(temp) <= MAX_POSITION) {
					setpoint.y = temp;
					snprintf(outputBuffer, 256, "New Northings setpoint: %f\r\n",
							setpoint.y);
					BT_SendStr(outputBuffer);
				}
			} else if (sscanf(element, "Z%d", &temp) > 0) {
				if (abs(temp) <= MAX_ALTITUDE) {
					setpoint.altitude += temp;
					snprintf(outputBuffer, 256, "New altitude setpoint: %f\r\n",
							setpoint.altitude);
					BT_SendStr(outputBuffer);
				}
			} else if (*element == 'A') {
				if (state == IDLE) {
					state = ASCENT;
				}
			} else if (*element == 'L') {
				if (state == ATTITUDE_HOVER
						| state == POSITION_AND_ATTITUDE_HOVER | state == ASCENT) {
					state = LANDING;
				}
			}
			// Get the next element in the instruction.
			element = strtok(NULL, " ");
		}
		bbtCompleteCommand = false;
		btIndex = 0;
	}
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/
	float totalTime = 0;
	for (;;) {
		FC321_GetTimeUS(&time);
		dt = time * 1e-6;
		totalTime += dt;
		FC321_Reset();
		handleBTCommands();
		switch (state) {
		case START: {
			// Initialize setpoints and gps values
			setpoint.x = setpoint.y = setpoint.altitude = setpoint.yaw = 0;
			setpoint.pitch = -0.35f;
			setpoint.roll = 0.0f;
			GPSstats.x = GPSstats.y = GPSstats.dx = GPSstats.dy =
					GPSstats.prevEasting = GPSstats.prevNorthing = 0;
			// Initialize velocity and position kalman filters
			initializeVelocityEstimator(&EstimatorVelocity);
			initializePositionEstimator(&EstimatorPosition);
			// Instantiate GPS, IMU and Barometer 'objects'
			New_NEO6M(&GPS);
			New_BMP384(&BARO, BMP384_DEFAULT_ADDRESS);
			New_BNO085(&IMU, BNO080_DEFAULT_ADDRESS);
			// Initialize sensors
			bool bInitSuccess = initializeSensors();
			// Initialization failed, don't continue
			if (!bInitSuccess) {
				while (1)
					;
			}
			state = CALIBRATING;
		}
			break;
		case CALIBRATING: {
			calibrateAltitude();
			calibrateYaw();
			calibratePitch();
			calibrateRoll();
			startupServos();
			BT_SendStr("Calibration completed!\r\n");
			state = IDLE;
		}
			break;
		case IDLE: {
			while (!bbtCompleteCommand) {
				// Blink LED
				ledToggle(GREEN);
				WAIT1_Waitms(50);
				ledToggle(GREEN);
				// Wait
				WAIT1_Waitms(950);
			}
		}
			break;
		case ASCENT: {
			readIMU();
			readBarometer();
			checkSensorErrors();
			// We successfully read the barometer and are at least as high as our setpoint. Transition to hover
			if (altitude >= setpoint.altitude) {
				state = ATTITUDE_HOVER;
				continue;
			}
			calculateThrust();
			calculateWorldAcceleration();
			calculatePitchRollTorque();
			calculateYawTorque();
			calculateServoValues();
			writeServos();
		}
			break;
		case POSITION_AND_ATTITUDE_HOVER: {
			readGPS();
			readIMU();
			readBarometer();
			checkSensorErrors();
			calculateThrust();
			calculateWorldAcceleration();
			estimateWorldVelocity();
			estimateWorldPosition();
			calculatePitchRollSetpoint();
			calculatePitchRollTorque();
			calculateYawTorque();
			calculateServoValues();
			writeServos();
		}
			break;
		case ATTITUDE_HOVER: {
			readIMU();
			readBarometer();
			checkSensorErrors();
			calculateThrust();
			calculatePitchRollTorque();
			calculateYawTorque();
			calculateServoValues();
			writeServos();
		}
			break;
		case LANDING: {
			if (bCalculateLandingHeight) {
				altitudeAGL = setpoint.altitude - altitudeInitial;
				bCalculateLandingHeight = false;
			}
			timeSinceLandingStarted += dt;
			setpoint.altitude = sampleLandingFunction(timeSinceLandingStarted)
					+ altitudeInitial;
			if (setpoint.altitude < altitudeInitial) {
				zeroServos();
				ledOff(GREEN);
				ledOff(BLUE);
				ledOff(RED);
				state = IDLE;
				continue;
			}
			readIMU();
			readBarometer();
			checkSensorErrors();
			calculateThrust();
			calculatePitchRollTorque();
			calculateYawTorque();
			calculateServoValues();
			writeServos();
		}
			break;
		case ERROR: {
			// Turn fan off
			EDF_SetPWMDutyUs(1000);
		}
			break;
		}
		haveGps = false;
		// snprintf(outputBuffer, 256, "%f,%f,%f,%f,%f,%f,%f\r\n", totalTime, yaw, pitch, roll, setpoint.yaw, setpoint.pitch, setpoint.roll);
		// BT_SendStr(outputBuffer);
		WAIT1_Waitms(10);
	}
	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
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
