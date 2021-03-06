/*
 * Sensors.h
 *
 *  Created on: 18 Sep 2021
 *      Author: willi
 */

#ifndef SOURCES_SENSORS_H_
#define SOURCES_SENSORS_H_

#include <stdbool.h>
#include <stdint.h>

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
	char *(*cardinal)(struct GPS *self, float course);
} GPS;

#endif /* SOURCES_SENSORS_H_ */
