/*
 * bno_055.c
 *
 *  Created on: Dec 30, 2019
 *      Author: young
 */

#include "main.h"
#include "bno_055.h"
#include <math.h>

printf(PI);

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

uint8_t getChipId(I2C_HandleTypeDef *hi2c) {
	uint8_t temp;
	if(readData(hi2c, BNO055_CHIP_ID_ADDR, &temp, 1) != HAL_OK) {
		printf("getChipId() error!\r\n");
		return 0;
	}
	return temp;
}

uint8_t getOperationMode(I2C_HandleTypeDef *hi2c) {
	uint8_t temp;
	if(readData(hi2c, BNO055_OPR_MODE_ADDR, &temp, 1) != HAL_OK) {
		printf("getOperationMode() error!\r\n");
		return 0;
	}
	return (temp&0X0F);
}

void setOperationMode(I2C_HandleTypeDef *hi2c, uint8_t op_mode) {
	if(writeData(hi2c, BNO055_OPR_MODE_ADDR, op_mode, 1) != HAL_OK) {
		printf("setOperatingMode() error!\r\n");
	}
}

uint8_t getAcceleration(I2C_HandleTypeDef *hi2c, short *buf) {
	uint8_t temp[6] = {0,};
	if(readData(hi2c, BNO055_ACCEL_DATA_X_LSB_ADDR, temp, 6) == HAL_OK) {
		for(int i=0; i<3; i++) {
			buf[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
		}
		return 1;
	}

	return 0;
}

uint8_t getMagnetometer(I2C_HandleTypeDef *hi2c, short *buf) {
	uint8_t temp[6] = {0,};
	if(readData(hi2c, BNO055_MAG_DATA_X_LSB_ADDR, temp, 6) == HAL_OK) {
		for(int i=0; i<3; i++) {
			buf[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
		}
		return 1;
	}

	return 0;
}

uint8_t getGyroscope(I2C_HandleTypeDef *hi2c, short *buf) {
	uint8_t temp[6] = {0,};
	if(readData(hi2c, BNO055_GYRO_DATA_X_LSB_ADDR, temp, 6) == HAL_OK) {
		for(int i=0; i<3; i++) {
			buf[i] = ( (temp[i*2+1] << 8) | temp[i*2] );
		}
		return 1;
	}

	return 0;
}

uint8_t getEuler(I2C_HandleTypeDef *hi2c, short *euler_data, uint8_t mode) {
	uint8_t temp[6] = {0,};

	if(readData(hi2c, BNO055_EULER_H_LSB_ADDR, temp, 6) == HAL_OK) {
		for(int i=0; i<3; i++) {
			euler_data[i] = (short)( (temp[i*2+1] << 8) | temp[i*2] );
		}

		if(mode == EULER_DATA_DEGREE) {
			euler_data[0] /= 16;
			euler_data[1] /= 16;
			euler_data[2] /= 16;
		} else {// EULER_DATA_RADIAN
			euler_data[0] /= 900;
			euler_data[1] /= 900;
			euler_data[2] /= 900;
		}

		return 1;
	}

	return 0;
}

//uint8_t getQuaternion(I2C_HandleTypeDef *hi2c, short *quaternion_data) {
//	uint8_t temp[8] = {0,};
//
//	if(readData(hi2c, BNO055_QUATERNION_DATA_W_LSB_ADDR, temp, 8) == HAL_OK) {
//		for(int i=0; i<4; i++) {
//			quaternion_data[i] = (short)( (temp[i*2+1] << 8) | temp[i*2] );
//		}
//		quaternion_data[0] /= 2^14; // w
//		quaternion_data[1] /= 2^14; // x
//		quaternion_data[2] /= 2^14; // y
//		quaternion_data[3] /= 2^14; // z
//
//		return 1;
//	}
//	return 0;
//}
//
//double getQuaternionPitch(short *quaternion_data) {
//	double val_1 = 2.0 * ( quaternion_data[0] * quaternion_data[1] + quaternion_data[2] * quaternion_data[3] );
//	double val_2 = 1.0 - 2.0 * (quaternion_data[1] * quaternion_data[1] + quaternion_data[2] * quaternion_data[2]);
//
//	double roll = atan2(val_1, val_2);
//
//	return roll * (180.0 / M_PI);
//}
//
//double getQuaternionYaw(short *quaternion_data) {
//	double val = 2.0 * ( quaternion_data[0] * quaternion_data[2] - quaternion_data[3] * quaternion_data[1] );
//	val = (val > 1.0) ? 1.0 : val;
//	val = (val < -1.0) ? -1.0 : val;
//
//	double pitch = asin(val);
//
//	return pitch * (180.0 / M_PI);
//}
//
//double getQuaternionRoll(short *quaternion_data) {
//	double val_1 = 2.0 * ( quaternion_data[0] * quaternion_data[3] + quaternion_data[1] * quaternion_data[2] );
//	double val_2 = 1.0 - 2.0 * (quaternion_data[2] * quaternion_data[2] + quaternion_data[3] * quaternion_data[3]);
//
//	double yaw = atan2(val_1, val_2);
//
//	return yaw * (180.0 / M_PI);
//}

uint8_t getCalibrationStatus(I2C_HandleTypeDef *hi2c) {
	uint8_t temp;
	if(readData(hi2c, BNO055_CALIB_STAT_ADDR, &temp, 1) == HAL_OK) {
		return temp;
	}

	return 0;
}

uint8_t getAxisMap(I2C_HandleTypeDef *hi2c) {
	uint8_t temp;
	if(readData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, &temp, 1) == HAL_OK) {
		return temp;
	}

	return 0;
}

void setAxisMap(I2C_HandleTypeDef *hi2c, uint8_t axis) {
	if(writeData(hi2c, BNO055_AXIS_MAP_CONFIG_ADDR, axis, 1) != HAL_OK) {
		printf("setAxisMap() error!\r\n");
	}
}

uint8_t getAxisSign(I2C_HandleTypeDef *hi2c) {
	uint8_t temp;
	if(readData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, &temp, 1) == HAL_OK) {
		return temp;
	}

	return 0;
}

void setAxisSign(I2C_HandleTypeDef *hi2c, uint8_t sign) {
	if(writeData(hi2c, BNO055_AXIS_MAP_SIGN_ADDR, sign, 1) != HAL_OK) {
		printf("setAxisSign() error!\r\n");
	}
}

void PrintCalibrationStatus(uint8_t status) {
	uint8_t cal_status[4];

	cal_status[0] = (status & 0x03) ;
	cal_status[1] = ( (status >> 2) & 0x03 );
	cal_status[2] = ( (status >> 4) & 0x03 );
	cal_status[3] = ( (status >> 6) & 0x03 );

	printf("CALIB_STATUS : MAG=%d, ACC=%d, GYR=%d SYS=%d\r\n", cal_status[0], cal_status[1], cal_status[2], cal_status[3]);
}

void PrintOperationMode(uint8_t mode) {
	switch(mode) {
	/* Config mode */
	case OP_MODE_CONFIG_MODE:
		printf("Current Mode is CONFIG_MODE\r\n");
		break;

	/* Non-Fusion mode*/
    case OP_MODE_ACC_ONLY:
		printf("Current Mode is ACC_ONLY\r\n");
		break;
	case OP_MODE_MAG_ONLY:
		printf("Current Mode is MAG_ONLY\r\n");
		break;
	case OP_MODE_GYRO_ONLY:
		printf("Current Mode is GYRO_ONLY\r\n");
		break;
	case OP_MODE_ACC_MAG:
		printf("Current Mode is ACC_MAG\r\n");
		break;
	case OP_MODE_ACC_GYRO:
		printf("Current Mode is ACC_GYRO\r\n");
		break;
	case OP_MODE_MAG_GYRO:
		printf("Current Mode is MAG_GYRO\r\n");
		break;
	case OP_MODE_AMG:
		printf("Current Mode is AMG\r\n");
		break;

	/* Fusion mode */
	case OP_MODE_IMU:
		printf("Current Mode is IMU\r\n");
		break;
	case OP_MODE_COMPASS:
		printf("Current Mode is COMPASS\r\n");
		break;
	case OP_MODE_M4G:
		printf("Current Mode is M4G\r\n");
		break;
	case OP_MODE_NDOF_FMC_OFF:
		printf("Current Mode is NDOF_FMC_OFF\r\n");
		break;
	case OP_MODE_NDOF:
		printf("Current Mode is NDOF\r\n");
		break;

	default:
		printf("No Mode listed\r\n");
	}
}
