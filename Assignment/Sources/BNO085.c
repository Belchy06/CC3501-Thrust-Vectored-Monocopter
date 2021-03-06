/*
 * MPU6050.c
 *
 *  Created on: Aug 16, 2021
 *      Author: willi
 */
#include "BNO085.h"

void New_BNO085(BNO085 *self, byte addr) {
	self->devAddr = addr;
	self->err = 0x0;
	self->_hasReset = false;
	self->rotationVector_Q1 = 14;
	self->rotationVectorAccuracy_Q1 = 12; //Heading accuracy estimate in radians. The Q point is 12.
	self->accelerometer_Q1 = 8;
	self->linear_accelerometer_Q1 = 8;
	self->gyro_Q1 = 9;
	self->magnetometer_Q1 = 4;
	self->angular_velocity_Q1 = 10;
	self->sequenceNumber[0] = 0;
	self->sequenceNumber[1] = 0;
	self->sequenceNumber[2] = 0;
	self->sequenceNumber[3] = 0;
	self->sequenceNumber[4] = 0;
	self->sequenceNumber[5] = 0;
	self->shtpHeader[0] = 0;
	self->shtpHeader[1] = 0;
	self->shtpHeader[2] = 0;
	self->shtpHeader[3] = 0;
	for (uint8_t i = 0; i < 128; i++) {
		self->shtpData[i] = 0;
	}
	self->begin = &begin;
	self->enableRotationVector = &enableRotationVector;
	self->enableAccelerometer = &enableAccelerometer;
	self->enableLinearAccelerometer = &enableLinearAccelerometer;
	self->enableGyro = &enableGyro;
	self->enableMagnetometer = &enableMagnetometer;
	self->enableGameRotationVector = &enableGameRotationVector;
	self->dataAvailable = &dataAvailable;
	self->getRoll = &getRoll;
	self->getYaw = &getYaw;
	self->getPitch = &getPitch;
	self->getQuatReal = &getQuatReal;
	self->getQuatI = &getQuatI;
	self->getQuatJ = &getQuatJ;
	self->getQuatK = &getQuatK;
	self->getQuatRadianAccuracy = &getQuatRadianAccuracy;
	self->getAccel = &getAccel;
	self->getLinAccel = &getLinAccel;
	self->getGyro = &getGyro;
	self->getMag = &getMag;
	self->getReadings = &getReadings;
}

bool begin(BNO085 *self) {
	softReset(self);
	//Check communication with device
	self->shtpData[0] = SHTP_REPORT_PRODUCT_ID_REQUEST; //Request the product ID and reset info
	self->shtpData[1] = 0;
	//Transmit packet on channel 2, 2 bytes
	sendPacket(self, CHANNEL_CONTROL, 2);

	if (receivePacket(self) == true) {
		return true;
	}

	return false;
}

void softReset(BNO085 *self) {
	self->shtpData[0] = 1; //Reset

	sendPacket(self, CHANNEL_EXECUTABLE, 1);

	//Read all incoming data and flush it
	for (int counter = 0; counter < 500000; counter++) {
		__asm volatile ("nop");
	}
	while (receivePacket(self) == true)
		;
	for (int counter = 0; counter < 500000; counter++) {
		__asm volatile ("nop");
	}
	while (receivePacket(self) == true)
		;
}

bool receivePacket(BNO085 *self) {
	CI2C1_SelectSlave(self->devAddr);
	word recv;
	uint8_t data[4];
	self->err = CI2C1_RecvBlock(data, 4, &recv);
	if (self->err != ERR_OK) {
		//do something
		;
	}
	uint8_t packetLSB = data[0];
	uint8_t packetMSB = data[1];
	uint8_t channelNumber = data[2];
	uint8_t sequenceNumber = data[3];

	//Store the header info.
	self->shtpHeader[0] = packetLSB;
	self->shtpHeader[1] = packetMSB;
	self->shtpHeader[2] = channelNumber;
	self->shtpHeader[3] = sequenceNumber;

	//Calculate the number of data bytes in this packet
	uint16_t dataLength = (((uint16_t) packetMSB) << 8) | (packetLSB);
	dataLength &= ~(1 << 15); //Clear the MSbit.

	if (dataLength == 0) {
		//Packet is empty
		return false; //All done
	}
	dataLength -= 4; //Remove the header bytes from the data count
	getData(self, dataLength);

	if (self->shtpHeader[2] == CHANNEL_EXECUTABLE
			&& self->shtpData[0] == EXECUTABLE_RESET_COMPLETE) {
		self->_hasReset = true;
	}

	return true;
}

bool getData(BNO085 *self, uint16_t bytesRemaining) {
	uint16_t dataSpot = 0;
	word recv;
	while (bytesRemaining > 0) {
		uint16_t numberOfBytesToRead = bytesRemaining;
		if (numberOfBytesToRead > (I2C_BUFFER_LENGTH - 4))
			numberOfBytesToRead = (I2C_BUFFER_LENGTH - 4);
		uint8_t dataLength = numberOfBytesToRead + 4;
		uint8_t data[dataLength];

		self->err = CI2C1_RecvBlock(data, dataLength, &recv);

		for (uint8_t i = 4; i < dataLength; i++) {
			if (dataSpot < MAX_PACKET_SIZE) {
				// First 4 bytes are header bytes and can be disregarded
				self->shtpData[dataSpot++] = data[i];
			}
		}
		bytesRemaining -= numberOfBytesToRead;
	}
	CI2C1_SendStop();
	return true;
}

void enableRotationVector(BNO085 *self, uint16_t timeBetweenReports) {
	setFeatureCommand(self, SENSOR_REPORTID_ROTATION_VECTOR, timeBetweenReports,
			0);
}

void enableAccelerometer(BNO085 *self, uint16_t timeBetweenReports) {
	setFeatureCommand(self, SENSOR_REPORTID_ACCELEROMETER, timeBetweenReports,
			0);
}

void enableLinearAccelerometer(BNO085 *self, uint16_t timeBetweenReports) {
	setFeatureCommand(self, SENSOR_REPORTID_LINEAR_ACCELERATION,
			timeBetweenReports, 0);
}

void enableGyro(BNO085 *self, uint16_t timeBetweenReports) {
	// SENSOR_REPORTID_GYROSCOPE
	setFeatureCommand(self, SENSOR_REPORTID_GYROSCOPE, timeBetweenReports, 0);
}

void enableMagnetometer(BNO085 *self, uint16_t timeBetweenReports) {
	setFeatureCommand(self, SENSOR_REPORTID_MAGNETIC_FIELD, timeBetweenReports,
			0);
}

void enableGameRotationVector(BNO085 *self, uint16_t timeBetweenReports) {
	setFeatureCommand(self,
	SENSOR_REPORTID_AR_VR_STABILIZED_GAME_ROTATION_VECTOR, timeBetweenReports,
			0);
}

void setFeatureCommand(BNO085 *self, uint8_t reportID,
		uint16_t timeBetweenReports, uint32_t specificConfig) {
	long microsBetweenReports = (long) timeBetweenReports * 1000L;

	self->shtpData[0] = SHTP_REPORT_SET_FEATURE_COMMAND; //Set feature command. Reference page 55
	self->shtpData[1] = reportID; //Feature Report ID. 0x01 = Accelerometer, 0x05 = Rotation vector
	self->shtpData[2] = 0;								   //Feature flags
	self->shtpData[3] = 0;							//Change sensitivity (LSB)
	self->shtpData[4] = 0;							//Change sensitivity (MSB)
	self->shtpData[5] = (microsBetweenReports >> 0) & 0xFF; //Report interval (LSB) in microseconds. 0x7A120 = 500ms
	self->shtpData[6] = (microsBetweenReports >> 8) & 0xFF;  //Report interval
	self->shtpData[7] = (microsBetweenReports >> 16) & 0xFF; //Report interval
	self->shtpData[8] = (microsBetweenReports >> 24) & 0xFF; //Report interval (MSB)
	self->shtpData[9] = 0;								  //Batch Interval (LSB)
	self->shtpData[10] = 0;								   //Batch Interval
	self->shtpData[11] = 0;								   //Batch Interval
	self->shtpData[12] = 0;								  //Batch Interval (MSB)
	self->shtpData[13] = (specificConfig >> 0) & 0xFF;//Sensor-specific config (LSB)
	self->shtpData[14] = (specificConfig >> 8) & 0xFF;	//Sensor-specific config
	self->shtpData[15] = (specificConfig >> 16) & 0xFF;	//Sensor-specific config
	self->shtpData[16] = (specificConfig >> 24) & 0xFF;	//Sensor-specific config (MSB)

	//Transmit packet on channel 2, 17 bytes
	sendPacket(self, CHANNEL_CONTROL, 17);
}

bool sendPacket(BNO085 *self, uint8_t channelNumber, uint8_t dataLength) {
	word sent;
	uint8_t packetLength = dataLength + 4;
	byte message[packetLength];
	message[0] = packetLength & 0xFF;
	message[1] = packetLength >> 8;
	message[2] = channelNumber;
	message[3] = self->sequenceNumber[channelNumber]++;

	for (int i = 4; i < packetLength; i++) {
		message[i] = self->shtpData[i - 4];
	}

	self->err = CI2C1_SelectSlave(self->devAddr);
	self->err = CI2C1_SendBlock(message, packetLength, &sent);
	CI2C1_SendStop();

	return self->err ? false : true;
}

bool dataAvailable(BNO085 *self) {
	bool dataAvail = (getReadings(self) != 0);
	return dataAvail;
}

uint16_t getReadings(BNO085 *self) {
	uint16_t returnVal;
	if (receivePacket(self) == true) {
		if (self->shtpHeader[2] == CHANNEL_REPORTS
				&& self->shtpData[0] == SHTP_REPORT_BASE_TIMESTAMP) {
			returnVal = parseInputReport(self);
			return returnVal;
		} else if (self->shtpHeader[2] == CHANNEL_CONTROL) {
			returnVal = parseCommandReport(self);
			return returnVal;
		} else if (self->shtpHeader[2] == CHANNEL_GYRO) {
			returnVal = parseInputReport(self); //This will update the rawAccelX, etc variables depending on which feature report is found
			return returnVal;
		}
	}
	return 0;
}

//This function pulls the data from the input report
//The input reports vary in length so this function stores the various 16-bit values as globals

//Unit responds with packet that contains the following:
//shtpHeader[0:3]: First, a 4 byte header
//shtpData[0:4]: Then a 5 byte timestamp of microsecond clicks since reading was taken
//shtpData[5 + 0]: Then a feature report ID (0x01 for Accel, 0x05 for Rotation Vector)
//shtpData[5 + 1]: Sequence number (See 6.5.18.2)
//shtpData[5 + 2]: Status
//shtpData[3]: Delay
//shtpData[4:5]: i/accel x/gyro x/etc
//shtpData[6:7]: j/accel y/gyro y/etc
//shtpData[8:9]: k/accel z/gyro z/etc
//shtpData[10:11]: real/gyro temp/etc
//shtpData[12:13]: Accuracy estimate
uint16_t parseInputReport(BNO085 *self) {
	int16_t dataLength = ((uint16_t) self->shtpHeader[1] << 8
			| self->shtpHeader[0]);
	dataLength &= ~(1 << 15);
	dataLength -= 4;
	self->timeStamp = ((uint32_t) self->shtpData[4] << (8 * 3))
			| ((uint32_t) self->shtpData[3] << (8 * 2))
			| ((uint32_t) self->shtpData[2] << (8 * 1))
			| ((uint32_t) self->shtpData[1]);
	if (self->shtpHeader[2] == CHANNEL_GYRO) {
		self->rawQuatI = (uint16_t) self->shtpData[1] << 8 | self->shtpData[0];
		self->rawQuatJ = (uint16_t) self->shtpData[3] << 8 | self->shtpData[2];
		self->rawQuatK = (uint16_t) self->shtpData[5] << 8 | self->shtpData[4];
		self->rawQuatReal = (uint16_t) self->shtpData[7] << 8
				| self->shtpData[6];
		self->rawFastGyroX = (uint16_t) self->shtpData[9] << 8
				| self->shtpData[8];
		self->rawFastGyroY = (uint16_t) self->shtpData[11] << 8
				| self->shtpData[10];
		self->rawFastGyroZ = (uint16_t) self->shtpData[13] << 8
				| self->shtpData[12];

		return SENSOR_REPORTID_GYRO_INTEGRATED_ROTATION_VECTOR;
	}

	uint8_t status = self->shtpData[5 + 2] & 0x03; //Get status bits
	uint16_t data1 = (uint16_t) self->shtpData[5 + 5] << 8
			| self->shtpData[5 + 4];
	uint16_t data2 = (uint16_t) self->shtpData[5 + 7] << 8
			| self->shtpData[5 + 6];
	uint16_t data3 = (uint16_t) self->shtpData[5 + 9] << 8
			| self->shtpData[5 + 8];
	uint16_t data4 = 0;
	uint16_t data5 = 0; //We would need to change this to uin32_t to capture time stamp value on Raw Accel/Gyro/Mag reports

	if (dataLength - 5 > 9) {
		data4 = (uint16_t) self->shtpData[5 + 11] << 8 | self->shtpData[5 + 10];
	}
	if (dataLength - 5 > 11) {
		data5 = (uint16_t) self->shtpData[5 + 13] << 8 | self->shtpData[5 + 12];
	}

	//Store these generic values to their proper global variable
	if (self->shtpData[5] == SENSOR_REPORTID_ACCELEROMETER) {
		self->accelAccuracy = status;
		self->rawAccelX = data1;
		self->rawAccelY = data2;
		self->rawAccelZ = data3;
	} else if (self->shtpData[5] == SENSOR_REPORTID_LINEAR_ACCELERATION) {
		self->accelLinAccuracy = status;
		self->rawLinAccelX = data1;
		self->rawLinAccelY = data2;
		self->rawLinAccelZ = data3;
	} else if (self->shtpData[5] == SENSOR_REPORTID_GYROSCOPE) {
		self->gyroAccuracy = status;
		self->rawGyroX = data1;
		self->rawGyroY = data2;
		self->rawGyroZ = data3;
	} else if (self->shtpData[5] == SENSOR_REPORTID_MAGNETIC_FIELD) {
		self->magAccuracy = status;
		self->rawMagX = data1;
		self->rawMagY = data2;
		self->rawMagZ = data3;
	} else if (self->shtpData[5] == SENSOR_REPORTID_ROTATION_VECTOR
			|| self->shtpData[5] == SENSOR_REPORTID_GAME_ROTATION_VECTOR
			|| self->shtpData[5]
					== SENSOR_REPORTID_AR_VR_STABILIZED_ROTATION_VECTOR
			|| self->shtpData[5]
					== SENSOR_REPORTID_AR_VR_STABILIZED_GAME_ROTATION_VECTOR) {
		self->quatAccuracy = status;
		self->rawQuatI = data1;
		self->rawQuatJ = data2;
		self->rawQuatK = data3;
		self->rawQuatReal = data4;

		//Only available on rotation vector and ar/vr stabilized rotation vector,
		// not game rot vector and not ar/vr stabilized rotation vector
		self->rawQuatRadianAccuracy = data5;
	} else if (self->shtpData[5] == SENSOR_REPORTID_TAP_DETECTOR) {
		self->tapDetector = self->shtpData[5 + 4]; //Byte 4 only
	} else if (self->shtpData[5] == SENSOR_REPORTID_STEP_COUNTER) {
		self->stepCount = data3; //Bytes 8/9
	} else if (self->shtpData[5] == SENSOR_REPORTID_STABILITY_CLASSIFIER) {
		self->stabilityClassifier = self->shtpData[5 + 4]; //Byte 4 only
	} else if (self->shtpData[5] == SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER) {
		self->activityClassifier = self->shtpData[5 + 5]; //Most likely state

		//Load activity classification confidences into the array
		for (uint8_t x = 0; x < 9; x++)	//Hardcoded to max of 9. TODO - bring in array size
			self->_activityConfidences[x] = self->shtpData[5 + 6 + x]; //5 bytes of timestamp, byte 6 is first confidence byte
	} else if (self->shtpData[5] == SENSOR_REPORTID_RAW_ACCELEROMETER) {
		self->memsRawAccelX = data1;
		self->memsRawAccelY = data2;
		self->memsRawAccelZ = data3;
	} else if (self->shtpData[5] == SENSOR_REPORTID_RAW_GYROSCOPE) {
		self->memsRawGyroX = data1;
		self->memsRawGyroY = data2;
		self->memsRawGyroZ = data3;
	} else if (self->shtpData[5] == SENSOR_REPORTID_RAW_MAGNETOMETER) {
		self->memsRawMagX = data1;
		self->memsRawMagY = data2;
		self->memsRawMagZ = data3;
	} else if (self->shtpData[5] == SHTP_REPORT_COMMAND_RESPONSE) {
		uint8_t command = self->shtpData[5 + 2];
		if (command == COMMAND_ME_CALIBRATE) {
			self->calibrationStatus = self->shtpData[5 + 5];
		}
	} else {
		//Unknown sensor ID
		return 0;
	}
	uint16_t returnVal = (uint16_t) self->shtpData[5];
	return returnVal;
}

uint16_t parseCommandReport(BNO085 *self) {
	if (self->shtpData[5] == SHTP_REPORT_COMMAND_RESPONSE) {
		uint8_t command = self->shtpData[2];

		if (command == COMMAND_ME_CALIBRATE) {
			self->calibrationStatus = self->shtpData[5];
		}
		return self->shtpData[0];
	}
	return 0;
}

// Return the roll (rotation around the x-axis) in Radians
float getRoll(BNO085 *self) {
	float dqw = getQuatReal(self);
	float dqx = getQuatI(self);
	float dqy = getQuatJ(self);
	float dqz = getQuatK(self);

	float norm = sqrt(dqw * dqw + dqx * dqx + dqy * dqy + dqz * dqz);
	if (norm == 0.0f) {
		dqw = 0;
		dqx = 0;
		dqy = 0;
		dqz = 0;
	} else {
		dqw = dqw / norm;
		dqx = dqx / norm;
		dqy = dqy / norm;
		dqz = dqz / norm;
	}

	float ysqr = dqy * dqy;
	// roll (x-axis rotation)
	float t0 = +2.0 * (dqw * dqx + dqy * dqz);
	float t1 = +1.0 - 2.0 * (dqx * dqx + ysqr);
	float roll = atan2(t0, t1);

	return roll;
}

// Return the pitch (rotation around the y-axis) in Radians
float getPitch(BNO085 *self) {
	float dqw = getQuatReal(self);
	float dqx = getQuatI(self);
	float dqy = getQuatJ(self);
	float dqz = getQuatK(self);

	float norm = sqrt(dqw * dqw + dqx * dqx + dqy * dqy + dqz * dqz);
	if (norm == 0.0f) {
		dqw = 0;
		dqx = 0;
		dqy = 0;
		dqz = 0;
	} else {
		dqw = dqw / norm;
		dqx = dqx / norm;
		dqy = dqy / norm;
		dqz = dqz / norm;
	}

	float ysqr = dqy * dqy;

	// pitch (y-axis rotation)
	float t2 = +2.0 * (dqw * dqy - dqz * dqx);
	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;
	float pitch = asin(t2);

	return pitch;
}

float getYaw(BNO085 *self) {
	float dqw = getQuatReal(self);
	float dqx = getQuatI(self);
	float dqy = getQuatJ(self);
	float dqz = getQuatK(self);

	float norm = sqrt(dqw * dqw + dqx * dqx + dqy * dqy + dqz * dqz);
	if (norm == 0.0f) {
		dqw = 0;
		dqx = 0;
		dqy = 0;
		dqz = 0;
	} else {
		dqw = dqw / norm;
		dqx = dqx / norm;
		dqy = dqy / norm;
		dqz = dqz / norm;
	}

	float ysqr = dqy * dqy;

	// yaw (z-axis rotation)
	float t3 = +2.0 * (dqw * dqz + dqx * dqy);
	float t4 = +1.0 - 2.0 * (ysqr + dqz * dqz);
	float yaw = atan2(t3, t4);

	return yaw;
}

float getQuatReal(BNO085 *self) {
	float quat = qToFloat(self->rawQuatReal, self->rotationVector_Q1);
	return quat;
}

float getQuatI(BNO085 *self) {
	float quat = qToFloat(self->rawQuatI, self->rotationVector_Q1);
	return quat;
}

float getQuatJ(BNO085 *self) {
	float quat = qToFloat(self->rawQuatJ, self->rotationVector_Q1);
	return quat;
}

float getQuatK(BNO085 *self) {
	float quat = qToFloat(self->rawQuatK, self->rotationVector_Q1);
	return quat;
}

float getQuatRadianAccuracy(BNO085 *self) {
	float quat = qToFloat(self->rawQuatRadianAccuracy,
			self->rotationVectorAccuracy_Q1);
	return quat;
}

float qToFloat(int16_t fixedPointValue, uint8_t qPoint) {
	float qFloat = fixedPointValue;
	qFloat *= powf(2, qPoint * -1);
	return qFloat;
}

void getAccel(BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy) {
	*x = qToFloat(self->rawAccelX, self->accelerometer_Q1);
	*y = qToFloat(self->rawAccelY, self->accelerometer_Q1);
	*z = qToFloat(self->rawAccelZ, self->accelerometer_Q1);
	*accuracy = self->accelAccuracy;
}

void getLinAccel(BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy) {
	*x = qToFloat(self->rawLinAccelX, self->linear_accelerometer_Q1);
	*y = qToFloat(self->rawLinAccelY, self->linear_accelerometer_Q1);
	*z = qToFloat(self->rawLinAccelZ, self->linear_accelerometer_Q1);
	*accuracy = self->accelLinAccuracy;
}

void getGyro(BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy) {
	*x = qToFloat(self->rawGyroX, self->gyro_Q1);
	*y = qToFloat(self->rawGyroY, self->gyro_Q1);
	*z = qToFloat(self->rawGyroZ, self->gyro_Q1);
	*accuracy = self->gyroAccuracy;
}

void getMag(BNO085 *self, float *x, float *y, float *z, uint8_t *accuracy) {
	*x = qToFloat(self->rawMagX, self->magnetometer_Q1);
	*y = qToFloat(self->rawMagY, self->magnetometer_Q1);
	*z = qToFloat(self->rawMagZ, self->magnetometer_Q1);
	*accuracy = self->magAccuracy;
}

