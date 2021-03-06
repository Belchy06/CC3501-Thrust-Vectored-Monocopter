/*

 * GPS.c
 *
 *  Created on: 18 Sep 2021
 *      Author: willi

 */
#include "GPS.h"

void New_NEO6M(NEO6M *self) {
	self->_time = GPS_INVALID_TIME;
	self->_date = GPS_INVALID_DATE;
	self->_latitude = GPS_INVALID_ANGLE;
	self->_longitude = GPS_INVALID_ANGLE;
	self->_altitude = GPS_INVALID_ALTITUDE;
	self->_speed = GPS_INVALID_SPEED;
	self->_course = GPS_INVALID_ANGLE;
	self->_hdop = GPS_INVALID_HDOP;
	self->_numsats = GPS_INVALID_SATELLITES;
	self->_last_time_fix = GPS_INVALID_FIX_TIME;
	self->_last_position_fix = GPS_INVALID_FIX_TIME;
	self->_parity = 0;
	self->_is_checksum_term = false;
	self->_sentence_type = _GPS_SENTENCE_OTHER;
	self->_term_number = 0;
	self->_term_offset = 0;
	self->_gps_data_good = false;
	self->_term[0] = '\0';

	self->encode = &encode;
	self->term_complete = &term_complete;
	self->get_position = &get_position;
	self->get_altitude = &get_altitude;
	self->distance_between = &distance_between;
	self->course_to = &course_to;
	self->speed_knots = &speed_knots;
	self->speed_mps = &speed_mps;
}

bool encode(NEO6M *self, char c) {
	bool valid_sentence = false;

	switch (c) {
	case ',': // term terminators
		self->_parity ^= c;
	case '\r':
	case '\n':
	case '*':
		if (self->_term_offset < sizeof(self->_term)) {
			self->_term[self->_term_offset] = 0;
			valid_sentence = term_complete(self);
		}
		++self->_term_number;
		self->_term_offset = 0;
		self->_is_checksum_term = (c == '*');
		return valid_sentence;

	case '$': //sentence begin
		self->_term_number = self->_term_offset = 0;
		self->_parity = 0;
		self->_sentence_type = _GPS_SENTENCE_OTHER;
		self->_is_checksum_term = false;
		self->_gps_data_good = false;
		return valid_sentence;
	}

	// ordinary characters
	if (self->_term_offset < sizeof(self->_term) - 1)
		self->_term[self->_term_offset++] = c;
	if (!self->_is_checksum_term)
		self->_parity ^= c;
	return valid_sentence;
}

bool term_complete(NEO6M *self) {
	if (self->_is_checksum_term) {
		char checksum = 16 * from_hex(self->_term[0])
				+ from_hex(self->_term[1]);
		if (checksum == self->_parity) {
			if (self->_gps_data_good) {
				self->_last_time_fix = self->_new_time_fix;
				self->_last_position_fix = self->_new_position_fix;

				switch (self->_sentence_type) {
				case _GPS_SENTENCE_GPRMC:
					self->_time = self->_new_time;
					self->_date = self->_new_date;
					self->_latitude = self->_new_latitude;
					self->_longitude = self->_new_longitude;
					self->_speed = self->_new_speed;
					self->_course = self->_new_course;
					break;
				case _GPS_SENTENCE_GPGGA:
					self->_altitude = self->_new_altitude;
					self->_time = self->_new_time;
					self->_latitude = self->_new_latitude;
					self->_longitude = self->_new_longitude;
					self->_numsats = self->_new_numsats;
					self->_hdop = self->_new_hdop;
					break;
				}

				return true;
			}
		}
		return false;
	}

	// the first term determines the sentence type
	if (self->_term_number == 0) {
		if (!gpsstrcmp(self->_term, _GPRMC_TERM))
			self->_sentence_type = _GPS_SENTENCE_GPRMC;
		else if (!gpsstrcmp(self->_term, _GPGGA_TERM))
			self->_sentence_type = _GPS_SENTENCE_GPGGA;
		else
			self->_sentence_type = _GPS_SENTENCE_OTHER;
		return false;
	}

	if (self->_sentence_type != _GPS_SENTENCE_OTHER && self->_term[0])
		switch (COMBINE(self->_sentence_type, self->_term_number)) {
		case COMBINE(_GPS_SENTENCE_GPRMC, 1): // Time in both sentences
		case COMBINE(_GPS_SENTENCE_GPGGA, 1):
			self->_new_time = parse_decimal(self);
			GPSTimer_GetTimeMS(&self->_new_time_fix);
			GPSTimer_Reset();
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 2): // GPRMC validity
			self->_gps_data_good = self->_term[0] == 'A';
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 3): // Latitude
		case COMBINE(_GPS_SENTENCE_GPGGA, 2):
			self->_new_latitude = parse_degrees(self);
			GPSTimer_GetTimeMS(&self->_new_time_fix);
			GPSTimer_Reset();
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 4): // N/S
		case COMBINE(_GPS_SENTENCE_GPGGA, 3):
			if (self->_term[0] == 'S')
				self->_new_latitude = -self->_new_latitude;
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 5): // Longitude
		case COMBINE(_GPS_SENTENCE_GPGGA, 4):
			self->_new_longitude = parse_degrees(self);
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 6): // E/W
		case COMBINE(_GPS_SENTENCE_GPGGA, 5):
			if (self->_term[0] == 'W')
				self->_new_longitude = -self->_new_longitude;
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 7): // Speed (GPRMC)
			self->_new_speed = parse_decimal(self);
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 8): // Course (GPRMC)
			self->_new_course = parse_decimal(self);
			break;
		case COMBINE(_GPS_SENTENCE_GPRMC, 9): // Date (GPRMC)
			self->_new_date = gpsatol(self->_term);
			break;
		case COMBINE(_GPS_SENTENCE_GPGGA, 6): // Fix data (GPGGA)
			self->_gps_data_good = self->_term[0] > '0';
			break;
		case COMBINE(_GPS_SENTENCE_GPGGA, 7): // Satellites used (GPGGA)
			self->_new_numsats = (unsigned char) atoi(self->_term);
			break;
		case COMBINE(_GPS_SENTENCE_GPGGA, 8): // HDOP
			self->_new_hdop = parse_decimal(self);
			break;
		case COMBINE(_GPS_SENTENCE_GPGGA, 9): // Altitude (GPGGA)
			self->_new_altitude = parse_decimal(self);
			break;
		}

	return false;
}

int from_hex(char a) {
	if (a >= 'A' && a <= 'F')
		return a - 'A' + 10;
	else if (a >= 'a' && a <= 'f')
		return a - 'a' + 10;
	else
		return a - '0';
}

int gpsstrcmp(const char *str1, const char *str2) {
	while (*str1 && *str1 == *str2)
		++str1, ++str2;
	return *str1;
}

unsigned long parse_decimal(NEO6M *self) {
	char *p = self->_term;
	bool isneg = *p == '-';
	if (isneg)
		++p;
	unsigned long ret = 100UL * gpsatol(p);
	while (gpsisdigit(*p))
		++p;
	if (*p == '.') {
		if (gpsisdigit(p[1])) {
			ret += 10 * (p[1] - '0');
			if (gpsisdigit(p[2]))
				ret += p[2] - '0';
		}
	}
	return isneg ? -ret : ret;
}

// Parse a string in the form ddmm.mmmmmmm...
unsigned long parse_degrees(NEO6M *self) {
	char *p;
	unsigned long left_of_decimal = gpsatol(self->_term);
	unsigned long hundred1000ths_of_minute = (left_of_decimal % 100UL)
			* 100000UL;
	for (p = self->_term; gpsisdigit(*p); ++p)
		;
	if (*p == '.') {
		unsigned long mult = 10000;
		while (gpsisdigit(*++p)) {
			hundred1000ths_of_minute += mult * (*p - '0');
			mult /= 10;
		}
	}
	return (left_of_decimal / 100) * 1000000
			+ (hundred1000ths_of_minute + 3) / 6;
}

long gpsatol(const char *str) {
	long ret = 0;
	while (gpsisdigit(*str))
		ret = 10 * ret + *str++ - '0';
	return ret;
}

bool gpsisdigit(char c) {
	return c >= '0' && c <= '9';
}

void get_position(NEO6M *self, float *lat, float *lon, unsigned long *age) {
	*lat = self->_latitude == GPS_INVALID_ANGLE ?
			GPS_INVALID_F_ANGLE : (self->_latitude / 1000000.0f);
	*lon = self->_longitude == GPS_INVALID_ANGLE ?
			GPS_INVALID_F_ANGLE : (self->_longitude / 1000000.0f);
}

void get_altitude(NEO6M *self, float *alt) {
	*alt = self->_altitude == GPS_INVALID_ALTITUDE ?
			GPS_INVALID_F_ALTITUDE : self->_altitude / 100.0f;
}

float distance_between(NEO6M *self, float lat1, float long1, float lat2,
		float long2) {
	// returns distance in meters between two positions, both specified
	// as signed decimal-degrees latitude and longitude. Uses great-circle
	// distance computation for hypothetical sphere of radius 6372795 meters.
	// Because Earth is no exact sphere, rounding errors may be up to 0.5%.
	// Courtesy of Maarten Lamers
	float delta = radians(long1 - long2);
	float sdlong = sinf(delta);
	float cdlong = cosf(delta);
	lat1 = radians(lat1);
	lat2 = radians(lat2);
	float slat1 = sinf(lat1);
	float clat1 = cosf(lat1);
	float slat2 = sinf(lat2);
	float clat2 = cosf(lat2);
	delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
	delta = sq(delta);
	delta += sq(clat2 * sdlong);
	delta = sqrt(delta);
	float denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
	delta = atan2f(delta, denom);
	return delta * 6372795;
}

float radians(float degrees) {
	return degrees * 3.14159265358979323846f / 180;
}

float degrees(float radians) {
	return radians * 180 / 3.14159265358979323846f;
}

float sq(float input) {
	return input * input;
}

float course_to(NEO6M *self, float lat1, float long1, float lat2, float long2) {
	// returns course in degrees (North=0, West=270) from position 1 to position 2,
	// both specified as signed decimal-degrees latitude and longitude.
	// Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
	// Courtesy of Maarten Lamers
	float dlon = radians(long2 - long1);
	lat1 = radians(lat1);
	lat2 = radians(lat2);
	float a1 = sinf(dlon) * cosf(lat2);
	float a2 = sinf(lat1) * cosf(lat2) * cosf(dlon);
	a2 = cosf(lat1) * sinf(lat2) - a2;
	a2 = atan2f(a1, a2);
	if (a2 < 0.0) {
		a2 += 2 * 3.14159265358979323846f;
	}
	return degrees(a2);
}

float speed_knots(NEO6M *self) {
	return self->_speed == GPS_INVALID_SPEED ? GPS_INVALID_F_SPEED : self->_speed / 100.0;
}

float speed_mps(NEO6M *self) {
	float sk = speed_knots(self);
	return sk == GPS_INVALID_F_SPEED ? GPS_INVALID_F_SPEED : _GPS_MPS_PER_KNOT * sk;
}

