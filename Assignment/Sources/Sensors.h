/*
 * Sensors.h
 *
 *  Created on: Aug 16, 2021
 *      Author: willi
 */

#ifndef SOURCES_SENSORS_H_
#define SOURCES_SENSORS_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct BNO085 {
	// Variables
	// Every sensor has an i2c address
	uint8_t devAddr;
	uint8_t err;
	uint8_t sequenceNumber[6];
	uint8_t shtpHeader[4];
	uint8_t shtpData[128];
	uint32_t timeStamp;
	uint16_t rawAccelX, rawAccelY, rawAccelZ, accelAccuracy;
	uint16_t rawLinAccelX, rawLinAccelY, rawLinAccelZ, accelLinAccuracy;
	uint16_t rawGyroX, rawGyroY, rawGyroZ, gyroAccuracy;
	uint16_t rawMagX, rawMagY, rawMagZ, magAccuracy;
	uint16_t rawQuatI, rawQuatJ, rawQuatK, rawQuatReal, rawQuatRadianAccuracy,
			quatAccuracy;
	uint16_t rawFastGyroX, rawFastGyroY, rawFastGyroZ;
	uint8_t tapDetector;
	uint16_t stepCount;
	uint8_t stabilityClassifier;
	uint8_t activityClassifier;
	uint8_t *_activityConfidences; //Array that store the confidences of the 9 possible activities
	uint8_t calibrationStatus;				//Byte R0 of ME Calibration Response
	uint16_t memsRawAccelX, memsRawAccelY, memsRawAccelZ; //Raw readings from MEMS sensor
	uint16_t memsRawGyroX, memsRawGyroY, memsRawGyroZ; //Raw readings from MEMS sensor
	uint16_t memsRawMagX, memsRawMagY, memsRawMagZ;
	bool _hasReset;
	// These Q values are defined in the datasheet but can also be obtained by querying the meta data records
	// See the read metadata example for more info
	int16_t rotationVector_Q1;
	int16_t rotationVectorAccuracy_Q1; //Heading accuracy estimate in radians. The Q point is 12.
	int16_t accelerometer_Q1;
	int16_t linear_accelerometer_Q1;
	int16_t gyro_Q1;
	int16_t magnetometer_Q1;
	int16_t angular_velocity_Q1;
	// Functions
	bool (*begin)(struct BNO085 *self);
	void (*enableRotationVector)(struct BNO085 *self,
			uint16_t timeBetweenReports);
	void (*enableAccelerometer)(struct BNO085 *self,
				uint16_t timeBetweenReports);
	void (*enableLinearAccelerometer)(struct BNO085 *self,
					uint16_t timeBetweenReports);
	void (*enableGyro)(struct BNO085 *self,
					uint16_t timeBetweenReports);
	void (*enableMagnetometer)(struct BNO085 *self,
						uint16_t timeBetweenReports);
	void (*enableGameRotationVector)(struct BNO085 *self, uint16_t timeBetweenReports);
	bool (*dataAvailable)(struct BNO085 *self);
	float (*getRoll)(struct BNO085 *self);
	float (*getPitch)(struct BNO085 *self);
	float (*getYaw)(struct BNO085 *self);
	float (*getQuatReal)(struct BNO085 *self);
	float (*getQuatI)(struct BNO085 *self);
	float (*getQuatJ)(struct BNO085 *self);
	float (*getQuatK)(struct BNO085 *self);
	float (*getQuatRadianAccuracy)(struct BNO085 *self);
	void (*getAccel)(struct BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy);
	void (*getLinAccel)(struct BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy);
	void (*getGyro)(struct BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy);
	void (*getMag)(struct BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy);
	uint16_t (*getReadings)(struct BNO085 *self);
} BNO085;

typedef struct GPS {
	// properties
	unsigned long _time, _new_time;
	unsigned long _date, _new_date;
	long _latitude, _new_latitude;
	long _longitude, _new_longitude;
	long _altitude, _new_altitude;
	unsigned long _speed, _new_speed;
	unsigned long _course, _new_course;
	unsigned long _hdop, _new_hdop;
	unsigned short _numsats, _new_numsats;

	unsigned long _last_time_fix, _new_time_fix;
	unsigned long _last_position_fix, _new_position_fix;

	// parsing state variables
	uint8_t _parity;
	bool _is_checksum_term;
	uint8_t _term[15];
	uint8_t _sentence_type;
	uint8_t _term_number;
	uint8_t _term_offset;
	bool _gps_data_good;

	bool (*encode)(struct GPS *self, char c);
	bool (*term_complete)(struct GPS *self);
	void (*get_position)(struct GPS *self, float *lat, float *lon, unsigned long *age);
	void (*get_altitude)(struct GPS *self, float *alt);
	float (*distance_between)(struct GPS *self, float lat1, float long1, float lat2, float long2);
	float (*course_to)(struct GPS *self, float lat1, float long1, float lat2, float long2);
	float (*speed_knots)(struct GPS *self);
	float (*speed_mps)(struct GPS *self);
} GPS;

#endif /* SOURCES_SENSORS_H_ */
