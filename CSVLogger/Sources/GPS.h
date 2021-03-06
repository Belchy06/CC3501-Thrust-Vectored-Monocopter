/*
 * GPS.h
 *
 *  Created on: 18 Sep 2021
 *      Author: willi
 */

#ifndef SOURCES_GPS_H_
#define SOURCES_GPS_H_


#include "Sensors.h"
#include "GPSTimer.h"
#include <stdlib.h>
#include <math.h>

#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001

#define GPS_INVALID_AGE 0xFFFFFFFF
#define GPS_INVALID_ANGLE 999999999
#define GPS_INVALID_ALTITUDE 999999999
#define GPS_INVALID_DATE 0
#define GPS_INVALID_TIME 0xFFFFFFFF
#define GPS_INVALID_SPEED 999999999
#define GPS_INVALID_FIX_TIME 0xFFFFFFFF
#define GPS_INVALID_SATELLITES 0xFF
#define GPS_INVALID_HDOP 0xFFFFFFFF
#define _GPS_SENTENCE_GPGGA 0
#define _GPS_SENTENCE_GPRMC 1
#define _GPS_SENTENCE_OTHER 2
#define _GPRMC_TERM   "GPRMC"
#define _GPGGA_TERM   "GPGGA"
#define GPS_INVALID_F_ANGLE 1000.0f
#define GPS_INVALID_F_ALTITUDE 1000000.0f
#define GPS_INVALID_F_SPEED -1.0f

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

void NEW_GPS(GPS *self);
bool encode(GPS *self, char c);
bool term_complete(GPS *self);
int from_hex(char a);
int gpsstrcmp(const char *str1, const char *str2);
unsigned long parse_decimal(GPS *self);
unsigned long parse_degrees(GPS *self);
long gpsatol(const char *str);
bool gpsisdigit(char c);
void get_position(GPS *self, float *lat, float *lon, unsigned long *age);
void get_altitude(GPS *self, float *alt);
float distance_between(GPS *self, float lat1, float long1, float lat2, float long2);
float degrees(float radians);
float radians(float degrees);
float sq(float input);
float course_to(GPS *self, float lat1, float long1, float lat2, float long2);
float speed_knots(GPS *self);
float speed_mps(GPS *self);

#endif /* SOURCES_GPS_H_ */
