/*
 * I2C.h
 *
 *  Created on: 17 Aug 2021
 *      Author: Will
 */

#ifndef SOURCES_I2C_H_
#define SOURCES_I2C_H_

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "CI2C1.h"
#include "IntI2cLdd1.h"



uint8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
uint8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
uint8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
bool writeChar(uint8_t devAddr, uint8_t data);
bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
uint8_t writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);



#endif /* SOURCES_I2C_H_ */
