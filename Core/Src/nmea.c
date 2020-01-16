/*
 * nmea.c
 *
 *  Created on: 2020. 1. 16.
 *      Author: young
 */

#include "nmea.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t MakeChecksum(char *sentence) {
	uint8_t checksum = 0;
	uint8_t checksum_len = strlen(sentence);
	for(int i=1; i<checksum_len; i++) {
		checksum ^= sentence[i];
	}

	return checksum;
}

void GenerateNMEA0183Heading(double heading_data) {
	char sentence[20] = {0,};

	sprintf(sentence, "%c", '$');
	sprintf(sentence, "%s%s", sentence, "HE");
	sprintf(sentence, "%s%s", sentence, "HDT,");

	sprintf(sentence, "%s%.1f,", sentence, heading_data);
	sprintf(sentence, "%s%c", sentence, 'A');

	sprintf(sentence, "%s*%02x\r\n", sentence, MakeChecksum(sentence));

	printf(sentence);
}

