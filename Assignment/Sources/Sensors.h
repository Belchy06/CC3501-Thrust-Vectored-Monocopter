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

typedef enum TimeStandby {
	TIME_STANDBY_5MS = 0x00, // Time standby bit field in the Output Data Rate (ODR) register
	TIME_STANDBY_10MS = 0x01,
	TIME_STANDBY_20MS = 0x02,
	TIME_STANDBY_40MS = 0x03,
	TIME_STANDBY_80MS = 0x04,
	TIME_STANDBY_160MS = 0x05,
	TIME_STANDBY_320MS = 0x06,
	TIME_STANDBY_640MS = 0x07,
	TIME_STANDBY_1280MS = 0x08,
	TIME_STANDBY_2560MS = 0x09,
	TIME_STANDBY_5120MS = 0x0A,
	TIME_STANDBY_10240MS = 0x0B,
	TIME_STANDBY_20480MS = 0x0C,
	TIME_STANDBY_40960MS = 0x0D,
	TIME_STANDBY_81920MS = 0x0E,
	TIME_STANDBY_163840MS = 0x0F,
	TIME_STANDBY_327680MS = 0x10,
	TIME_STANDBY_655360MS = 0x11
} TimeStandby;

typedef enum IIRFilter {
	IIR_FILTER_OFF = 0x00, // Infinite Impulse Response (IIR) filter bit field in the configuration register
	IIR_FILTER_2 = 0x01,
	IIR_FILTER_4 = 0x02,
	IIR_FILTER_8 = 0x03,
	IIR_FILTER_16 = 0x04,
	IIR_FILTER_32 = 0x05,
	IIR_FILTER_64 = 0x06,
	IIR_FILTER_128 = 0x07
} IIRFilter;


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

typedef struct NEO6M {
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

	bool (*encode)(struct NEO6M *self, char c);
	bool (*term_complete)(struct NEO6M *self);
	void (*get_position)(struct NEO6M *self, float *lat, float *lon, unsigned long *age);
	void (*get_altitude)(struct NEO6M *self, float *alt);
	float (*distance_between)(struct NEO6M *self, float lat1, float long1, float lat2, float long2);
	float (*course_to)(struct NEO6M *self, float lat1, float long1, float lat2, float long2);
	float (*speed_knots)(struct NEO6M *self);
	float (*speed_mps)(struct NEO6M *self);
} NEO6M;

typedef struct BMP384 {
	uint8_t devAddr;
	uint8_t err;

	union {								// Copy of the BMP388's event register
		struct {
			uint8_t por_detected :1;
		} bit;
		uint8_t reg;
	} event;

	union {						// Copy of the BMP388's configuration register
		struct {
			uint8_t :1;
			uint8_t iir_filter :3;
		} bit;
		uint8_t reg;
	} config;

	union {					// Copy of the BMP388's output data rate register
		struct {
			uint8_t odr_sel :5;
		} bit;
		uint8_t reg;
	} odr;

	union {						// Copy of the BMP388's oversampling register
		struct {
			uint8_t osr_p :3;
			uint8_t osr_t :3;
		} bit;
		uint8_t reg;
	} osr;

	volatile union {			// Copy of the BMP388's power control register
		struct {
			uint8_t press_en :1;
			uint8_t temp_en :1;
			uint8_t :2;
			uint8_t mode :2;
		} bit;
		uint8_t reg;
	} pwr_ctrl;

	volatile union {		// Copy of the BMP388's interrupt status register
		struct {
			uint8_t fwm_int : 1;
			uint8_t ffull_int : 1;
			uint8_t : 1;
			uint8_t drdy : 1;
		} bit;
		uint8_t reg;
	} int_status;

	struct {		// The BMP388 compensation trim parameters (coefficients)
		uint16_t param_T1;
		uint16_t param_T2;
		int8_t param_T3;
		int16_t param_P1;
		int16_t param_P2;
		int8_t param_P3;
		int8_t param_P4;
		uint16_t param_P5;
		uint16_t param_P6;
		int8_t param_P7;
		int8_t param_P8;
		int16_t param_P9;
		int8_t param_P10;
		int8_t param_P11;
	}__attribute__ ((packed)) params;

	struct FloatParams {// The BMP388 float point compensation trim parameters
		float param_T1;
		float param_T2;
		float param_T3;
		float param_P1;
		float param_P2;
		float param_P3;
		float param_P4;
		float param_P5;
		float param_P6;
		float param_P7;
		float param_P8;
		float param_P9;
		float param_P10;
		float param_P11;
	} floatParams;

	uint8_t (*start)(struct BMP384 *self);
	void (*setTimeStandby)(struct BMP384 *self, TimeStandby timeStandby);
	void (*startNormalConversion)(struct BMP384 *self);
	void (*setIIRFilter)(struct BMP384 *self, IIRFilter iirFilter);
	uint8_t (*getMeasurements)(struct BMP384 *self, float *temperature,
			float *pressure, float *altitude);

} BMP384;

#endif /* SOURCES_SENSORS_H_ */
