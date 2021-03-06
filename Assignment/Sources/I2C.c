/*
 * I2C.c
 *
 *  Created on: 17 Aug 2021
 *      Author: Will
 */

#include <stdbool.h>
#include "I2C.h"

bool writeChar(uint8_t devAddr, uint8_t data) {
	CI2C1_SelectSlave(devAddr);


	byte err = CI2C1_SendChar(data);
	CI2C1_SendStop();
	return err != ERR_OK ? true : false;
}

bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
	for(uint8 i = 0; i < length; i++) {
		(void) writeByte(devAddr, regAddr, data[i]);
	}
	return true;
}

uint8_t writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
	CI2C1_SelectSlave(devAddr);
	word sent;
	uint8_t dat[2] = { regAddr, data };
	byte err = CI2C1_SendBlock(dat, 2, &sent);
	CI2C1_SendStop();
	return err;
}

bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
	uint8_t b;
	(void) readByte(devAddr, regAddr, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	return writeByte(devAddr, regAddr, b);
}

bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
	uint8_t b;
	if(readByte(devAddr, regAddr, &b) != 0) {
		uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
		data <<= (bitStart - length + 1); // shift data into correct position
		data &= mask; // zero all non-important bits in data
		b &= ~(mask); // zero all important bits in existing byte
		b |= data; // combine data with existing byte
		return writeByte(devAddr, regAddr, b);
	} else {
		return false;
	}
}


uint8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
	return readBytes(devAddr, regAddr, 1, data);
}

uint8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
	CI2C1_SelectSlave(devAddr);
	byte err;
	err = CI2C1_SendChar(regAddr);
	if (err != ERR_OK) {
		CI2C1_SendStop();
		return err;
	}

	word recv;

	err = CI2C1_RecvBlock(data, length, &recv);
	CI2C1_SendStop();
	return err;
}

uint8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
	uint8_t b, r = 0;
	readByte(devAddr, regAddr, &b);
	for(uint8_t i = bitStart; i > bitStart - length; i--) {
		r |= (b & (1 << i));
	}
	r >>= (bitStart - length + 1);
	*data = r;
}

