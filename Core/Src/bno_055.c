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

	status = HAL_I2C_Master_Transmit(hi2c, (uint16_t)(BNO_055_DEVICE_ADDRESS << 1), buf, 2, 1000);
	if(status != HAL_OK) {
		printf("writeData HAL_I2C_Master_Transmit() error!\r\n");
	}

	free(buf);
	return status;
}

uint8_t getChipId(I2C_HandleTypeDef *hi2c, uint8_t *buf) {
	if(readData(hi2c, BNO055_CHIP_ID_ADDR, buf, 1) != HAL_OK) {
		printf("getChipId() error!\r\n");
		return 0;
	}
	return 1;
}

uint8_t getOperationModeStr(I2C_HandleTypeDef *hi2c, char *buf) {
	uint8_t temp;
	if(readData(hi2c, BNO055_OPR_MODE_ADDR, &temp, 1) != HAL_OK) {
		printf("getOperationMode() error!\r\n");
		return 0;
	}

	temp = (temp&0x0F);

	switch(temp)
	{
		/* Config mode */
		case OP_MODE_CONFIG_MODE:
			sprintf(buf, "Mode : CONFIG MODE");
			break;
		/* Non-Fusion mode*/
		case OP_MODE_ACC_ONLY:
			sprintf(buf, "Mode : ACC ONLY");
			break;
		case OP_MODE_MAG_ONLY:
			sprintf(buf, "Mode : MAG ONLY");
			break;
		case OP_MODE_GYRO_ONLY:
			sprintf(buf, "Mode : GYRO ONLY");
			break;
		case OP_MODE_ACC_MAG:
			sprintf(buf, "Mode : ACC MAG");
			break;
		case OP_MODE_ACC_GYRO:
			sprintf(buf, "Mode : ACC GYRO");
			break;
		case OP_MODE_MAG_GYRO:
			sprintf(buf, "Mode : MAG GYRO");
			break;
		case OP_MODE_AMG:
			sprintf(buf, "Mode : AMG");
			break;
		/* Fusion mode */
		case OP_MODE_IMU:
			sprintf(buf, "Mode : IMU");
			break;
		case OP_MODE_COMPASS:
			sprintf(buf, "Mode : COMPASS");
			break;
		case OP_MODE_M4G:
			sprintf(buf, "Mode : M4G");
			break;
		case OP_MODE_NDOF_FMC_OFF:
			sprintf(buf, "Mode : NDOF FMC OFF");
			break;
		case OP_MODE_NDOF:
			sprintf(buf, "Mode : NDOF");
			break;
		default:
			sprintf(buf, "Mode : ERROR");
	}

	return 1;
}

uint8_t getOperationMode(I2C_HandleTypeDef *hi2c, uint8_t *buf) {
	if(readData(hi2c, BNO055_OPR_MODE_ADDR, buf, 1) != HAL_OK) {
		printf("getOperationMode() error!\r\n");
		return 0;
	}
	*buf = (*buf & 0x0F);

	return 1;
}

uint8_t setOperationMode(I2C_HandleTypeDef *hi2c, uint8_t op_mode) {
	if(writeData(hi2c, BNO055_OPR_MODE_ADDR, op_mode, 1) != HAL_OK) {
		printf("setOperatingMode() error!\r\n");
		return 0;
	}

	return 1;
}

uint8_t getAcceleration(I2C_HandleTypeDef *hi2c, short *buf) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_ACCEL_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getAcceleration() error!\r\n");
		return 0;
	}

	for(int i=0; i<3; i++) {
		buf[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
	}

	return 1;

}

uint8_t getMagnetometer(I2C_HandleTypeDef *hi2c, short *buf) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_MAG_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getMagnetometer() error!\r\n");
		return 0;
	}

	for(int i=0; i<3; i++) {
		buf[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
	}

	return 1;

}

uint8_t getGyroscope(I2C_HandleTypeDef *hi2c, short *buf) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_GYRO_DATA_X_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getMagnetometer() error!\r\n");
		return 0;
	}

	for(int i=0; i<3; i++) {
		buf[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
	}

	return 1;
}

uint8_t getEuler(I2C_HandleTypeDef *hi2c, short *euler_data) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_EULER_H_LSB_ADDR, temp, 6) != HAL_OK) {
		printf("getEuler() error!\r\n");
		return 0;
	}

	for(int i=0; i<3; i++) {
		euler_data[i] = (short)( (temp[i*2+1] << 8) | temp[i*2] );
	}
	// EULER_DATA_DEGREE
	euler_data[0] /= 16;
	euler_data[1] /= 16;
	euler_data[2] /= 16;

	return 1;
}

uint8_t getCalibrationStatus(I2C_HandleTypeDef *hi2c, uint8_t* buf) {
	uint8_t temp;
	if(readData(hi2c, BNO055_CALIB_STAT_ADDR, &temp, 1) != HAL_OK) {
		printf("getCalibrationStatus() error!\r\n");
		return 0;
	}

	/*
	 * 0 : Not Calibrated, 3 : Fully Calibrated
	 */
	buf[0] = (temp & 0x03);			// MAG
	buf[1] = ( (temp >> 2) & 0x03 );	// ACC
	buf[2] = ( (temp >> 4) & 0x03 );	// GYR
	buf[3] = ( (temp >> 6) & 0x03 );	// SYS

	return 1;
}

uint8_t getAxisMap(I2C_HandleTypeDef *hi2c, uint8_t* buf) {
	if(readData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, buf, 1) != HAL_OK) {
		printf("getAxisMap() error!\r\n");
		return 0;
	}

	return 1;
}

uint8_t setAxisMap(I2C_HandleTypeDef *hi2c, uint8_t axis) {
	if(writeData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, axis, 1) != HAL_OK) {
		printf("setAxisMap() error!\r\n");
		return 0;
	}

	return 1;
}

uint8_t getAxisSign(I2C_HandleTypeDef *hi2c, uint8_t* buf) {
	if(readData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, buf, 1) != HAL_OK) {
		printf("setAxisMap() error!\r\n");
		return 0;
	}

	return 1;
}

uint8_t setAxisSign(I2C_HandleTypeDef *hi2c, uint8_t sign) {
	if(writeData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, sign, 1) != HAL_OK) {
		printf("setAxisSign() error!\r\n");
		return 0;
	}

	return 1;
}

void PrintCalibrationStatus(uint8_t status) {
	uint8_t cal_status[4];

	cal_status[0] = (status & 0x03) ;
	cal_status[1] = ( (status >> 2) & 0x03 );
	cal_status[2] = ( (status >> 4) & 0x03 );
	cal_status[3] = ( (status >> 6) & 0x03 );

	printf("CALIB_STATUS : MAG=%d, ACC=%d, GYR=%d SYS=%d\r\n", cal_status[0], cal_status[1], cal_status[2], cal_status[3]);
}
