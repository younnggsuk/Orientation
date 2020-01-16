/*
 * nmea.h
 *
 *  Created on: 2020. 1. 16.
 *      Author: young
 */

#ifndef INC_NMEA_H_
#define INC_NMEA_H_

#include "main.h"

uint8_t MakeChecksum(char *sentence);
void GenerateNMEA0183Heading(double heading_data);

#endif /* INC_NMEA_H_ */
