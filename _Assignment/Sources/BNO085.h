/*
 * MPU6050.h
 *
 *  Created on: Aug 16, 2021
 *      Author: willi
 */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"
#include "Sensors.h"
#include "I2C.h"
#include <stdbool.h>
#include <math.h>

#ifndef SOURCES_ACCELOROMETER_H_
#define SOURCES_ACCELOROMETER_H_

//The catch-all default is 32
#define I2C_BUFFER_LENGTH 32
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Registers
#define CHANNEL_COMMAND 0
#define CHANNEL_EXECUTABLE 1
#define CHANNEL_CONTROL 2
#define CHANNEL_REPORTS 3
#define CHANNEL_WAKE_REPORTS 4
#define CHANNEL_GYRO 5

//All the ways we can configure or talk to the BNO080, figure 34, page 36 reference manual
//These are used for low level communication with the sensor, on channel 2
#define SHTP_REPORT_COMMAND_RESPONSE 0xF1
#define SHTP_REPORT_COMMAND_REQUEST 0xF2
#define SHTP_REPORT_FRS_READ_RESPONSE 0xF3
#define SHTP_REPORT_FRS_READ_REQUEST 0xF4
#define SHTP_REPORT_PRODUCT_ID_RESPONSE 0xF8
#define SHTP_REPORT_PRODUCT_ID_REQUEST 0xF9
#define SHTP_REPORT_BASE_TIMESTAMP 0xFB
#define SHTP_REPORT_SET_FEATURE_COMMAND 0xFD

//All the different sensors and features we can get reports from
//These are used when enabling a given sensor
#define SENSOR_REPORTID_ACCELEROMETER 0x01
#define SENSOR_REPORTID_GYROSCOPE 0x02
#define SENSOR_REPORTID_MAGNETIC_FIELD 0x03
#define SENSOR_REPORTID_LINEAR_ACCELERATION 0x04
#define SENSOR_REPORTID_ROTATION_VECTOR 0x05
#define SENSOR_REPORTID_GRAVITY 0x06
#define SENSOR_REPORTID_GAME_ROTATION_VECTOR 0x08
#define SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR 0x09
#define SENSOR_REPORTID_GYRO_INTEGRATED_ROTATION_VECTOR 0x2A
#define SENSOR_REPORTID_TAP_DETECTOR 0x10
#define SENSOR_REPORTID_STEP_COUNTER 0x11
#define SENSOR_REPORTID_STABILITY_CLASSIFIER 0x13
#define SENSOR_REPORTID_RAW_ACCELEROMETER 0x14
#define SENSOR_REPORTID_RAW_GYROSCOPE 0x15
#define SENSOR_REPORTID_RAW_MAGNETOMETER 0x16
#define SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER 0x1E
#define SENSOR_REPORTID_AR_VR_STABILIZED_ROTATION_VECTOR 0x28
#define SENSOR_REPORTID_AR_VR_STABILIZED_GAME_ROTATION_VECTOR 0x29

//Record IDs from figure 29, page 29 reference manual
//These are used to read the metadata for each sensor type
#define FRS_RECORDID_ACCELEROMETER 0xE302
#define FRS_RECORDID_GYROSCOPE_CALIBRATED 0xE306
#define FRS_RECORDID_MAGNETIC_FIELD_CALIBRATED 0xE309
#define FRS_RECORDID_ROTATION_VECTOR 0xE30B

// Reset complete packet (BNO08X Datasheet p.24 Figure 1-27)
#define EXECUTABLE_RESET_COMPLETE 0x1

//Command IDs from section 6.4, page 42
//These are used to calibrate, initialize, set orientation, tare etc the sensor
#define COMMAND_ERRORS 1
#define COMMAND_COUNTER 2
#define COMMAND_TARE 3
#define COMMAND_INITIALIZE 4
#define COMMAND_DCD 6
#define COMMAND_ME_CALIBRATE 7
#define COMMAND_DCD_PERIOD_SAVE 9
#define COMMAND_OSCILLATOR 10
#define COMMAND_CLEAR_DCD 11

#define CALIBRATE_ACCEL 0
#define CALIBRATE_GYRO 1
#define CALIBRATE_MAG 2
#define CALIBRATE_PLANAR_ACCEL 3
#define CALIBRATE_ACCEL_GYRO_MAG 4
#define CALIBRATE_STOP 5

#define MAX_PACKET_SIZE 128 //Packets can be up to 32k but we don't have that much RAM.
#define MAX_METADATA_SIZE 9 //This is in words. There can be many but we mostly only care about the first 9 (Qs, range, etc)



void New_BNO085(BNO085 *self, uint8_t addr);
bool begin(BNO085 *self);
void softReset(BNO085 *self);
bool receivePacket(BNO085 *self);
void enableRotationVector(BNO085 *self, uint16_t timeBetweenReports);
void enableAccelerometer(BNO085 *self, uint16_t timeBetweenReports);
void enableGyro(BNO085 *self, uint16_t timeBetweenReports);
void enableMagnetometer(BNO085 *self, uint16_t timeBetweenReports);
void setFeatureCommand(BNO085 *self, uint8_t reportID, uint16_t timeBetweenReports, uint32_t specificConfig);
bool sendPacket(BNO085 *self, uint8_t channelNumber, uint8_t dataLength);
bool getData(BNO085 *self, uint16_t bytesRemaining);
bool dataAvailable(BNO085 *self);
uint16_t getReadings(BNO085 *self);
uint16_t parseInputReport(BNO085 *self);
uint16_t parseCommandReport(BNO085 *self);
float getRoll(BNO085 *self);
float getPitch(BNO085 *self);
float getYaw(BNO085 *self);
float getQuatReal(BNO085 *self);
float getQuatI(BNO085 *self);
float getQuatJ(BNO085 *self);
float getQuatK(BNO085 *self);
float getQuatRadianAccuracy(BNO085 *self);
void getAccel(BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy);
void getGyro(BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy);
void getMag(BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy);
float qToFloat(int16_t fixedPointValue, uint8_t qPoint);

#endif /* SOURCES_ACCELOROMETER_H_ */


