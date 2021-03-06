/*
 * BMP384.h
 *
 *  Created on: 13 Oct 2021
 *      Author: Will
 */

#ifndef SOURCES_BMP384_H_
#define SOURCES_BMP384_H_

#include "Sensors.h"
#include "I2C.h"
#include <math.h>

#define BMP388_I2C_ADDR		 		0x77				// The BMP388 I2C address
#define BMP388_I2C_ALT_ADDR 	0x76				// The BMP388 I2C alternate address
#define BMP388_ID 						0x50				// The BMP388 device ID
#define BMP390_ID							0x60				// The BMP390 device ID
#define RESET_CODE						0xB6				// The BMP388 reset code
#define FIFO_FLUSH						0xB0				// The BMP388 flush FIFO code

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Registers
////////////////////////////////////////////////////////////////////////////////

enum {
	BMP388_CHIP_ID = 0x00,					// Chip ID register sub-address
	BMP388_ERR_REG = 0x02,					// Error register sub-address
	BMP388_STATUS = 0x03,					// Status register sub-address
	BMP388_DATA_0 = 0x04,// Pressure eXtended Least Significant Byte (XLSB) register sub-address
	BMP388_DATA_1 = 0x05,// Pressure Least Significant Byte (LSB) register sub-address
	BMP388_DATA_2 = 0x06,// Pressure Most Significant Byte (MSB) register sub-address
	BMP388_DATA_3 = 0x07,// Temperature eXtended Least Significant Byte (XLSB) register sub-address
	BMP388_DATA_4 = 0x08,// Temperature Least Significant Byte (LSB) register sub-address
	BMP388_DATA_5 = 0x09,// Temperature Most Significant Byte (MSB) register sub-address
	BMP388_SENSORTIME_0 = 0x0C,			// Sensor time register 0 sub-address
	BMP388_SENSORTIME_1 = 0x0D,			// Sensor time register 1 sub-address
	BMP388_SENSORTIME_2 = 0x0E,			// Sensor time register 2 sub-address
	BMP388_EVENT = 0x10,					// Event register sub-address
	BMP388_INT_STATUS = 0x11,			// Interrupt Status register sub-address
	BMP388_FIFO_LENGTH_0 = 0x12,// FIFO Length Least Significant Byte (LSB) register sub-address
	BMP388_FIFO_LENGTH_1 = 0x13,// FIFO Length Most Significant Byte (MSB) register sub-address
	BMP388_FIFO_DATA = 0x14,				// FIFO Data register sub-address
	BMP388_FIFO_WTM_0 = 0x15,// FIFO Water Mark Least Significant Byte (LSB) register sub-address
	BMP388_FIFO_WTM_1 = 0x16,// FIFO Water Mark Most Significant Byte (MSB) register sub-address
	BMP388_FIFO_CONFIG_1 = 0x17,	// FIFO Configuration 1 register sub-address
	BMP388_FIFO_CONFIG_2 = 0x18,	// FIFO Configuration 2 register sub-address
	BMP388_INT_CTRL = 0x19,			// Interrupt Control register sub-address
	BMP388_IF_CONFIG = 0x1A,	// Interface Configuration register sub-address
	BMP388_PWR_CTRL = 0x1B,				// Power Control register sub-address
	BMP388_OSR = 0x1C,					// Oversampling register sub-address
	BMP388_ODR = 0x1D,					// Output Data Rate register sub-address
	BMP388_CONFIG = 0x1F,				// Configuration register sub-address
	BMP388_TRIM_PARAMS = 0x31,     // Trim parameter registers' base sub-address
	BMP388_CMD = 0x7E						// Command register sub-address
};

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Modes
////////////////////////////////////////////////////////////////////////////////

typedef enum Mode {
	SLEEP_MODE = 0x00, // Device mode bitfield in the control and measurement register
	FORCED_MODE = 0x01,
	NORMAL_MODE = 0x03
} Mode;

////////////////////////////////////////////////////////////////////////////////
// BMP388_DEV Register bit field Definitions
////////////////////////////////////////////////////////////////////////////////

typedef enum Oversampling {
	OVERSAMPLING_SKIP = 0x00, // Oversampling bit fields in the control and measurement register
	OVERSAMPLING_X2 = 0x01,
	OVERSAMPLING_X4 = 0x02,
	OVERSAMPLING_X8 = 0x03,
	OVERSAMPLING_X16 = 0x04,
	OVERSAMPLING_X32 = 0x05
} Oversampling;



enum OutputDrive {						// Interrupt output drive configuration
	PUSH_PULL = 0x00, OPEN_COLLECTOR = 0x01
};

enum ActiveLevel {				// Interrupt output active level configuration
	ACTIVE_LOW = 0x00, ACTIVE_HIGH = 0x01
};

enum LatchConfig {						// Interrupt output latch configuration
	UNLATCHED = 0x00,	// UNLATCHED: interrupt automatically clears after 2.5ms
	LATCHED = 0x01// LATCHED	: interrupt requires INT_STATUS register read to clear
};

enum PressEnable {						// FIFO pressure enable configuration
	PRESS_DISABLED = 0x00, PRESS_ENABLED = 0x01
};

enum AltEnable {						// FIFO altitude enable configuration
	ALT_DISABLED = 0x00, ALT_ENABLED = 0x01
};

enum TimeEnable {							// FIFO time enable configuration
	TIME_DISABLED = 0x00, TIME_ENABLED = 0x01
};

typedef enum Subsampling {							// FIFO sub-sampling configuration
	SUBSAMPLING_OFF = 0x00,
	SUBSAMPLING_DIV2 = 0x01,
	SUBSAMPLING_DIV4 = 0x02,
	SUBSAMPLING_DIV8 = 0x03,
	SUBSAMPLING_DIV16 = 0x04,
	SUBSAMPLING_DIV32 = 0x05,
	SUBSAMPLING_DIV64 = 0x06,
	SUBSAMPLING_DIV128 = 0x07
} Subsampling;

enum DataSelect {							// FIFO data select configuration
	UNFILTERED = 0x00, FILTERED = 0x01
};

enum StopOnFull {							// FIFO stop on full configuration
	STOP_ON_FULL_DISABLED = 0x00, STOP_ON_FULL_ENABLED = 0x01
};

enum FIFOStatus {												// FIFO status
	DATA_PENDING = 0x00, DATA_READY = 0X01, CONFIG_ERROR = 0x02
};

enum WatchdogTimout {									// I2C watchdog time-out
	WATCHDOG_TIMEOUT_1MS = 0x00, WATCHDOG_TIMEOUT_40MS = 0x01
};

void New_BMP384(BMP384 *self, uint8_t devAddr);
uint8_t start(BMP384 *self);
uint8_t reset(BMP384 *self);
void setIIRFilter(BMP384 *self, IIRFilter iirFilter);
void setTimeStandby(BMP384 *self, TimeStandby timeStandby);
void setOversamplingRegister(BMP384 *self, Oversampling presOversampling, Oversampling tempOversampling);
void setMode(BMP384 *self, Mode mode);
void startNormalConversion(BMP384 *self);
uint8_t getMeasurements(BMP384 *self, float *temperature, float *pressure, float *altitude);
uint8_t getTempPres(BMP384 *self, float *temperature, float *pressure);
uint8_t dataReady(BMP384 *self);
float compensate_temp(BMP384 *self, float uncomp_temp);
float compensate_press(BMP384 *self, float uncomp_press, float t_lin);

#endif /* SOURCES_BMP384_H_ */
