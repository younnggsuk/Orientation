/*
 * bno_055.c
 *
 *  Created on: Dec 30, 2019
 *      Author: young
 */

#include "bno_055.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HAL_StatusTypeDef readData(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data, uint8_t len) {
	data[0] = reg;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(hi2c, (uint16_t)BNO_055_DEVICE_ADDRESS << 1, data, 1, 100);
	if(status != HAL_OK) {
		printf("readData HAL_I2C_Master_Transmit() error!\r\n");
		return status;
	}

	status = HAL_I2C_Master_Receive(hi2c, (uint16_t)(BNO_055_DEVICE_ADDRESS << 1), data, len, 100);
	if(status != HAL_OK) {
		printf("readData HAL_I2C_Master_Receive() error!\r\n");
		return status;
	}

	return status;
}

HAL_StatusTypeDef writeData(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data, size_t data_len) {
	uint8_t *buf = (uint8_t*)malloc(data_len+1);
	if(buf == NULL) {
		printf("writeData malloc() error!\r\n");
		return -1;
	}

	buf[0] = reg;
	memcpy(buf+1, data, data_len);
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(hi2c, (uint16_t)(BNO_055_DEVICE_ADDRESS << 1), buf, data_len+1, 100);
	if(status != HAL_OK) {
		printf("writeData HAL_I2C_Master_Transmit() error!\r\n");
	}

	free(buf);
	return status;
}

void BNO055_Initialization(I2C_HandleTypeDef *hi2c, RTC_HandleTypeDef *hrtc, bno055_t *bno055) {
	while(1) {
		getChipId(hi2c, &(bno055->chip_id));
		if(bno055->chip_id == BNO_055_CHIP_ID) {
			break;
		}

		HAL_Delay(100);
	}

	if(LoadCalibrationProfile(hrtc, bno055->cal_profile)) {
		setCalibrationProfile(hi2c, bno055);
	}
	else {
		memset(bno055->cal_status, 0, 4);
		memset(bno055->cal_profile, 0, 22);
	}

	memset(bno055->euler_data, 0, 6);

    ChangeOperationModeTo(hi2c, bno055, OP_MODE_NDOF);
}

void BNO055_Update(I2C_HandleTypeDef *hi2c, bno055_t *bno055) {
	getEuler(hi2c, bno055->euler_data);
	getCalibrationStatus(hi2c, bno055->cal_status);
	getCalibrationProfile(hi2c, bno055);
}

void getChipId(I2C_HandleTypeDef *hi2c, uint8_t *chip_id) {
	if(readData(hi2c, BNO055_CHIP_ID_ADDR, chip_id, 1) != HAL_OK) {
		printf("getChipId() error!\r\n");
	}
}

void getOperationMode(I2C_HandleTypeDef *hi2c, uint8_t *opmode) {
	if(readData(hi2c, BNO055_OPR_MODE_ADDR, opmode, 1) != HAL_OK) {
		printf("getOperationMode() error!\r\n");
	}

	*opmode = ((*opmode) & 0x0F);
}

void setOperationMode(I2C_HandleTypeDef *hi2c, uint8_t *cur_opmode, uint8_t opmode) {
	if(writeData(hi2c, BNO055_OPR_MODE_ADDR, &opmode, 1) != HAL_OK) {
		printf("setOperatingMode() error!\r\n");
		return;
	}

	*cur_opmode = opmode;
}

void ChangeOperationModeTo(I2C_HandleTypeDef *hi2c, bno055_t *bno055, uint8_t opmode) {
	getOperationMode(hi2c, &(bno055->opmode));
	if(bno055->opmode != opmode) {
		setOperationMode(hi2c, &(bno055->opmode), opmode);
		HAL_Delay(1000);
	}
}

void getCalibrationStatus(I2C_HandleTypeDef *hi2c, uint8_t *cal_status) {
	uint8_t temp;
	if(readData(hi2c, BNO055_CALIB_STAT_ADDR, &temp, 1) != HAL_OK) {
		printf("getCalibrationStatus() error!\r\n");
	}

	/*
	 * 0 : Not Calibrated, 3 : Fully Calibrated
	 */
	cal_status[0] = (temp & 0x03);			// MAG
	cal_status[1] = ( (temp >> 2) & 0x03 );	// ACC
	cal_status[2] = ( (temp >> 4) & 0x03 );	// GYR
	cal_status[3] = ( (temp >> 6) & 0x03 );	// SYS
}

void getCalibrationProfile(I2C_HandleTypeDef *hi2c, bno055_t *bno055) {
	ChangeOperationModeTo(hi2c, bno055, OP_MODE_CONFIG_MODE);

	uint8_t temp[22] = {0,};
	if(readData(hi2c, ACCEL_OFFSET_X_LSB_ADDR, temp, 22) != HAL_OK) {
		printf("getCalibrationProfile() error!\r\n");
	}

	// ACC offset X, Y, Z
	bno055->cal_profile[0] = (uint16_t)( (temp[1] << 8) | temp[0] );
	bno055->cal_profile[1] = (uint16_t)( (temp[3] << 8) | temp[2] );
	bno055->cal_profile[2] = (uint16_t)( (temp[5] << 8) | temp[4] );

	// MAG offset X, Y, Z
	bno055->cal_profile[3] = (uint16_t)( (temp[7] << 8) | temp[6] );
	bno055->cal_profile[4] = (uint16_t)( (temp[9] << 8) | temp[8] );
	bno055->cal_profile[5] = (uint16_t)( (temp[11] << 8) | temp[10] );

	// GYR offset X, Y, Z
	bno055->cal_profile[6] = (uint16_t)( (temp[13] << 8) | temp[12] );
	bno055->cal_profile[7] = (uint16_t)( (temp[15] << 8) | temp[14] );
	bno055->cal_profile[8] = (uint16_t)( (temp[17] << 8) | temp[16] );

	// ACC radius
	bno055->cal_profile[9] = (uint16_t)( (temp[19] << 8) | temp[18] );

	// MAG radius
	bno055->cal_profile[10] = (uint16_t)( (temp[21] << 8) | temp[20] );

	ChangeOperationModeTo(hi2c, bno055, OP_MODE_NDOF);
}

void setCalibrationProfile(I2C_HandleTypeDef *hi2c, bno055_t *bno055) {
	ChangeOperationModeTo(hi2c, bno055, OP_MODE_CONFIG_MODE);

	uint8_t temp[22] = {0,};

	for(int i=0; i<11; i++) {
		temp[i*2] = (uint8_t)(bno055->cal_profile[i] & 0xFF);
		temp[i*2+1] = (uint8_t)( ((bno055->cal_profile[i])>>8) & 0xFF);
	}

	if(writeData(hi2c, ACCEL_OFFSET_X_LSB_ADDR, temp, 22) != HAL_OK) {
		printf("setCalibrationProfile() error!\r\n");
	}

	ChangeOperationModeTo(hi2c, bno055, OP_MODE_NDOF);
}


void SaveCalibrationProfile(I2C_HandleTypeDef *hi2c, RTC_HandleTypeDef* hrtc, bno055_t *bno055) {
	getCalibrationProfile(hi2c, bno055);
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR1, (uint32_t)( (bno055->cal_profile[0]<<16) | 0x0001U ));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR2, (uint32_t)( (bno055->cal_profile[2]<<16) | bno055->cal_profile[1] ));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR3, (uint32_t)( (bno055->cal_profile[4]<<16) | bno055->cal_profile[3] ));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR4, (uint32_t)( (bno055->cal_profile[6]<<16) | bno055->cal_profile[5] ));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR5, (uint32_t)( (bno055->cal_profile[8]<<16) | bno055->cal_profile[7] ));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR6, (uint32_t)( (bno055->cal_profile[10]<<16) | bno055->cal_profile[9] ));
}

uint8_t LoadCalibrationProfile(RTC_HandleTypeDef* hrtc, uint16_t *cal_profile) {
	if((uint16_t)(HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR1) & 0xFFFF) != 0x0001U) {
		return 0;
	}

	uint32_t temp[6] = {0,};
	temp[0] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR1);
	temp[1] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR2);
	temp[2] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR3);
	temp[3] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR4);
	temp[4] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR5);
	temp[5] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR6);

	cal_profile[0] 	= (uint16_t)( (temp[0]>>16) & 0xFFFF );
	cal_profile[1] 	= (uint16_t)( temp[1] & 0xFFFF );
	cal_profile[2] 	= (uint16_t)( (temp[1]>>16) & 0xFFFF );
	cal_profile[3] 	= (uint16_t)( temp[2] & 0xFFFF );
	cal_profile[4] 	= (uint16_t)( (temp[2]>>16) & 0xFFFF );
	cal_profile[5] 	= (uint16_t)( temp[3] & 0xFFFF );
	cal_profile[6] 	= (uint16_t)( (temp[3]>>16) & 0xFFFF );
	cal_profile[7] 	= (uint16_t)( temp[4] & 0xFFFF );
	cal_profile[8] 	= (uint16_t)( (temp[4]>>16) & 0xFFFF );
	cal_profile[9] 	= (uint16_t)( temp[5] & 0xFFFF );
	cal_profile[10] = (uint16_t)( (temp[5]>>16) & 0xFFFF );

	return 1;
}

void getEuler(I2C_HandleTypeDef *hi2c, short *euler_data) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_EULER_H_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getEuler() error!\r\n");
	}

	for(int i=0; i<3; i++) {
		euler_data[i] = (short)( (temp[i*2+1] << 8) | temp[i*2] );
		euler_data[i] /= 16;
	}
}


//void getAcceleration(I2C_HandleTypeDef *hi2c, short *acc) {
//	uint8_t temp[6] = {0,};
//
//	if(readData(hi2c, BNO055_ACCEL_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
//		printf("getAcceleration() error!\r\n");
//	}
//
//	for(int i=0; i<3; i++) {
//		acc[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
//	}
//}
//
//void getMagnetometer(I2C_HandleTypeDef *hi2c, short *mag) {
//	uint8_t temp[6] = {0,};
//
//	if(readData(hi2c, BNO055_MAG_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
//		printf("getMagnetometer() error!\r\n");
//	}
//
//	for(int i=0; i<3; i++) {
//		mag[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
//	}
//}
//
//void getGyroscope(I2C_HandleTypeDef *hi2c, short *gyr) {
//	uint8_t temp[6] = {0,};
//
//	if(readData(hi2c, BNO055_GYRO_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
//		printf("getMagnetometer() error!\r\n");
//	}
//
//	for(int i=0; i<3; i++) {
//		gyr[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
//	}
//}
//
//void getAxisMap(I2C_HandleTypeDef *hi2c, uint8_t* axis) {
//	if(readData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, axis, 1) != HAL_OK) {
//		printf("getAxisMap() error!\r\n");
//	}
//}
//
//void setAxisMap(I2C_HandleTypeDef *hi2c, uint8_t axis) {
//	if(writeData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, &axis, 1) != HAL_OK) {
//		printf("setAxisMap() error!\r\n");
//	}
//}
//
//void getAxisSign(I2C_HandleTypeDef *hi2c, uint8_t* sign) {
//	if(readData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, sign, 1) != HAL_OK) {
//		printf("setAxisMap() error!\r\n");
//	}
//}
//
//void setAxisSign(I2C_HandleTypeDef *hi2c, uint8_t sign) {
//	if(writeData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, &sign, 1) != HAL_OK) {
//		printf("setAxisSign() error!\r\n");
//	}
//}

