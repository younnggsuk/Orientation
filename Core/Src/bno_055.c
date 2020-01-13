/*
 * bno_055.c
 *
 *  Created on: Dec 30, 2019
 *      Author: young
 */

#include "main.h"
#include "bno_055.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

HAL_StatusTypeDef readData(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data, uint8_t len) {
	data[0] = reg;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(hi2c, BNO_055_DEVICE_ADDRESS << 1, data, 1, 100);
	if(status != HAL_OK) {
		printf("readData HAL_I2C_Master_Transmit() error!\r\n");
		return status;
	}

	status = HAL_I2C_Master_Receive(hi2c, (BNO_055_DEVICE_ADDRESS << 1), data, len, 100);
	if(status != HAL_OK) {
		printf("readData HAL_I2C_Master_Receive() error!\r\n");
		return status;
	}

	return status;
}

HAL_StatusTypeDef writeData(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t data, size_t data_len) {
	uint8_t *buf = (uint8_t*)malloc(data_len+1);
	if(buf == NULL) {
		printf("writeData malloc() error!\r\n");
		return -1;
	}

	memcpy(buf, &reg, 1);
	memcpy(buf+1, &data, data_len);
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(hi2c, (uint16_t)(BNO_055_DEVICE_ADDRESS << 1), buf, 2, 100);
	if(status != HAL_OK) {
		printf("writeData HAL_I2C_Master_Transmit() error!\r\n");
	}

	free(buf);
	return status;
}

void getChipId(I2C_HandleTypeDef *hi2c, uint8_t *chip) {
	if(readData(hi2c, BNO055_CHIP_ID_ADDR, chip, 1) != HAL_OK) {
		printf("getChipId() error!\r\n");
	}
}

void getOperationModeStr(I2C_HandleTypeDef *hi2c, char *str_opmode) {
	uint8_t temp;
	if(readData(hi2c, BNO055_OPR_MODE_ADDR, &temp, 1) != HAL_OK) {
		printf("getOperationMode() error!\r\n");
	}

	temp = (temp&0x0F);
	switch(temp)
	{
		/* Config mode */
		case OP_MODE_CONFIG_MODE:
			sprintf(str_opmode, "Mode : CONFIG MODE");
			break;
		/* Non-Fusion mode*/
		case OP_MODE_ACC_ONLY:
			sprintf(str_opmode, "Mode : ACC ONLY");
			break;
		case OP_MODE_MAG_ONLY:
			sprintf(str_opmode, "Mode : MAG ONLY");
			break;
		case OP_MODE_GYRO_ONLY:
			sprintf(str_opmode, "Mode : GYRO ONLY");
			break;
		case OP_MODE_ACC_MAG:
			sprintf(str_opmode, "Mode : ACC MAG");
			break;
		case OP_MODE_ACC_GYRO:
			sprintf(str_opmode, "Mode : ACC GYRO");
			break;
		case OP_MODE_MAG_GYRO:
			sprintf(str_opmode, "Mode : MAG GYRO");
			break;
		case OP_MODE_AMG:
			sprintf(str_opmode, "Mode : AMG");
			break;
		/* Fusion mode */
		case OP_MODE_IMU:
			sprintf(str_opmode, "Mode : IMU");
			break;
		case OP_MODE_COMPASS:
			sprintf(str_opmode, "Mode : COMPASS");
			break;
		case OP_MODE_M4G:
			sprintf(str_opmode, "Mode : M4G");
			break;
		case OP_MODE_NDOF_FMC_OFF:
			sprintf(str_opmode, "Mode : NDOF FMC OFF");
			break;
		case OP_MODE_NDOF:
			sprintf(str_opmode, "Mode : NDOF");
			break;
		default:
			sprintf(str_opmode, "Mode : ERROR");
	}
}

void getOperationMode(I2C_HandleTypeDef *hi2c, uint8_t *opmode) {
	if(readData(hi2c, BNO055_OPR_MODE_ADDR, opmode, 1) != HAL_OK) {
		printf("getOperationMode() error!\r\n");
	}

	(*opmode) = ((*opmode) & 0x0F);
}

void setOperationMode(I2C_HandleTypeDef *hi2c, uint8_t opmode) {
	if(writeData(hi2c, BNO055_OPR_MODE_ADDR, opmode, 1) != HAL_OK) {
		printf("setOperatingMode() error!\r\n");
	}
}

void getAcceleration(I2C_HandleTypeDef *hi2c, short *acc) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_ACCEL_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getAcceleration() error!\r\n");
	}

	for(int i=0; i<3; i++) {
		acc[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
	}
}

void getMagnetometer(I2C_HandleTypeDef *hi2c, short *mag) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_MAG_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getMagnetometer() error!\r\n");
	}

	for(int i=0; i<3; i++) {
		mag[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
	}
}

void getGyroscope(I2C_HandleTypeDef *hi2c, short *gyr) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_GYRO_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getMagnetometer() error!\r\n");
	}

	for(int i=0; i<3; i++) {
		gyr[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
	}
}

void getEuler(I2C_HandleTypeDef *hi2c, short *euler) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_EULER_H_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getEuler() error!\r\n");
	}

	for(int i=0; i<3; i++) {
		euler[i] = (short)( (temp[i*2+1] << 8) | temp[i*2] );
	}
	// EULER_DATA_DEGREE
	euler[0] /= 16;
	euler[1] /= 16;
	euler[2] /= 16;
}

void getAxisMap(I2C_HandleTypeDef *hi2c, uint8_t* axis) {
	if(readData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, axis, 1) != HAL_OK) {
		printf("getAxisMap() error!\r\n");
	}
}

void setAxisMap(I2C_HandleTypeDef *hi2c, uint8_t axis) {
	if(writeData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, axis, 1) != HAL_OK) {
		printf("setAxisMap() error!\r\n");
	}
}

void getAxisSign(I2C_HandleTypeDef *hi2c, uint8_t* sign) {
	if(readData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, sign, 1) != HAL_OK) {
		printf("setAxisMap() error!\r\n");
	}
}

void setAxisSign(I2C_HandleTypeDef *hi2c, uint8_t sign) {
	if(writeData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, sign, 1) != HAL_OK) {
		printf("setAxisSign() error!\r\n");
	}
}

void getCalibrationStatus(I2C_HandleTypeDef *hi2c, uint8_t* cal_status) {
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

uint8_t getCalibrationProfile(I2C_HandleTypeDef *hi2c, uint8_t* cal_status, uint16_t *cal_profile) {
	if((cal_status[0] != 0x03U) || (cal_status[2] != 0x03U)) {
		return 0;
	}

	ChangeOperationModeTo(hi2c, OP_MODE_CONFIG_MODE);

	uint8_t temp[22] = {0,};
	if(readData(hi2c, ACCEL_OFFSET_X_LSB_ADDR, temp, 22) != HAL_OK) {
		printf("getCalibrationProfile() error!\r\n");
	}

	for(int i=0; i<9; i++) {
		cal_profile[i] = (uint16_t)( (temp[i*2+1] << 8) | temp[i*2] );
	}
	cal_profile[9] = (uint16_t)( (temp[19] << 8) | temp[18] );
	cal_profile[10] = (uint16_t)( (temp[21] << 8) | temp[20] );

//	 ACC_DATA index : 0, 1, 2
//	 MAG_DATA index : 3, 4, 5
//	 GYR_DATA index : 6, 7, 8
//	 RADIUS_ACC_DATA index : 9
//	 RADIUS_MAG_DATA index : 10

	ChangeOperationModeTo(hi2c, OP_MODE_NDOF);

	return 1;
}

void saveCalibrationProfile(RTC_HandleTypeDef* hrtc, uint16_t *cal_profile) {
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR1, (uint32_t)((cal_profile[1]<<16) | cal_profile[0]));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR2, (uint32_t)((cal_profile[3]<<16) | cal_profile[2]));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR3, (uint32_t)((cal_profile[5]<<16) | cal_profile[4]));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR4, (uint32_t)((cal_profile[7]<<16) | cal_profile[6]));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR5, (uint32_t)((cal_profile[9]<<16) | cal_profile[8]));
	HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR6, (uint32_t)(cal_profile[10]));
}

void loadCalibrationProfile(RTC_HandleTypeDef* hrtc, uint16_t *cal_profile) {
	uint16_t temp[6] = {0,};

	temp[0] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR1);
	temp[1] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR2);
	temp[2] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR3);
	temp[3] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR4);
	temp[4] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR5);
	temp[5] = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR6);

	cal_profile[0] = (uint16_t)(temp[0] & 0xFFFF);
	cal_profile[1] = (uint16_t)(temp[0]>>16 & 0xFFFF);
	cal_profile[2] = (uint16_t)(temp[1] & 0xFFFF);
	cal_profile[3] = (uint16_t)(temp[1]>>16 & 0xFFFF);
	cal_profile[4] = (uint16_t)(temp[2] & 0xFFFF);
	cal_profile[5] = (uint16_t)(temp[2]>>16 & 0xFFFF);
	cal_profile[6] = (uint16_t)(temp[3] & 0xFFFF);
	cal_profile[7] = (uint16_t)(temp[3]>>16 & 0xFFFF);
	cal_profile[8] = (uint16_t)(temp[4] & 0xFFFF);
	cal_profile[9] = (uint16_t)(temp[4]>>16 & 0xFFFF);
	cal_profile[10] = (uint16_t)(temp[5] & 0xFFFF);
}

void ChangeOperationModeTo(I2C_HandleTypeDef *hi2c, uint8_t opmode) {
	uint8_t temp;

	getOperationMode(hi2c, &temp);
	if(temp != opmode) {
		setOperationMode(hi2c, opmode);
		HAL_Delay(1000);
	}
}




