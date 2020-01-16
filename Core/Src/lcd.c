/*
 * lcd.c
 *
 *  Created on: Jan 15, 2020
 *      Author: young
 */

#include "lcd.h"

#include <stdio.h>
#include <string.h>

void LCD_Init() {
	BSP_LCD_Init();

	BSP_LCD_LayerRgb565Init(0, LCD_FB_START_ADDRESS);
	BSP_LCD_DisplayOn();
	BSP_LCD_SelectLayer(0);
	BSP_LCD_SetTransparency(0, 255);
	BSP_LCD_Clear(RGB565_BLACK);

	BSP_LCD_SetTextColor(RGB565_WHITE);
	BSP_LCD_SetBackColor(RGB565_BLACK);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(0, 110, (uint8_t *)"Device Initializing...", CENTER_MODE);
}

void DrawTitle() {
	BSP_LCD_Clear(RGB565_BLACK);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"BNO 055", CENTER_MODE);
}

void DrawOperationMode(uint8_t opmode) {
	char str_opmode[20] = {0,};

	switch(opmode)
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

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(0, 50, (uint8_t*)str_opmode, LEFT_MODE);
}

void DrawEulerData(short *euler_data) {
	char str_heading[20] = {0,};
	char str_roll[20] = {0,};
	char str_pitch[20] = {0,};

	sprintf(str_heading, "Heading %4d", euler_data[0]);
	sprintf(str_roll, "Roll    %4d", euler_data[1]);
	sprintf(str_pitch, "Pitch   %4d", euler_data[2]);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(0, 70, (uint8_t*)str_heading, LEFT_MODE);
	BSP_LCD_DisplayStringAt(0, 90, (uint8_t*)str_roll, LEFT_MODE);
	BSP_LCD_DisplayStringAt(0, 110, (uint8_t*)str_pitch, LEFT_MODE);
}

void DrawCalibrationStatus(uint8_t *cal_status) {
	char str_cal_stat_acc[20] = {0,};
	char str_cal_stat_gyr[20] = {0,};
	char str_cal_stat_mag[20] = {0,};
	char str_cal_stat_sys[20] = {0,};

	sprintf(str_cal_stat_acc, "ACC %d", cal_status[1]);
	sprintf(str_cal_stat_mag, "MAG %d", cal_status[0]);
	sprintf(str_cal_stat_gyr, "GYR %d", cal_status[2]);
	sprintf(str_cal_stat_sys, "SYS %d", cal_status[3]);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_DisplayStringAt(0, 130, (uint8_t*)str_cal_stat_acc, LEFT_MODE);
	BSP_LCD_DisplayStringAt(70, 130, (uint8_t*)str_cal_stat_mag, LEFT_MODE);
	BSP_LCD_DisplayStringAt(140, 130, (uint8_t*)str_cal_stat_gyr, LEFT_MODE);
	BSP_LCD_DisplayStringAt(210, 130, (uint8_t*)str_cal_stat_sys, LEFT_MODE);
}

void DrawCalibrationProfile(uint16_t *cal_profile) {
	char str_cal_offset_acc[40] = {0,};
	char str_cal_offset_mag[40] = {0,};
	char str_cal_offset_gyr[40] = {0,};
	char str_cal_radius_acc[40] = {0,};
	char str_cal_radius_mag[40] = {0,};

	sprintf(str_cal_offset_acc, "A_OFFSET X %d, Y %d, Z %d", (short)cal_profile[0], (short)cal_profile[1], (short)cal_profile[2]);
	sprintf(str_cal_offset_mag, "M_OFFSET X %d, Y %d, Z %d", (short)cal_profile[3], (short)cal_profile[4], (short)cal_profile[5]);
	sprintf(str_cal_offset_gyr, "G_OFFSET X %d, Y %d, Z %d", (short)cal_profile[6], (short)cal_profile[7], (short)cal_profile[8]);
	sprintf(str_cal_radius_acc, "A_RADIUS %d", (short)cal_profile[9]);
	sprintf(str_cal_radius_mag, "M_RADIUS %d", (short)cal_profile[10]);

	BSP_LCD_SetFont(&Font12);
	BSP_LCD_DisplayStringAt(0, 150, (uint8_t*)str_cal_offset_acc, LEFT_MODE);
	BSP_LCD_DisplayStringAt(0, 170, (uint8_t*)str_cal_offset_mag, LEFT_MODE);
	BSP_LCD_DisplayStringAt(0, 190, (uint8_t*)str_cal_offset_gyr, LEFT_MODE);
	BSP_LCD_DisplayStringAt(0, 210, (uint8_t*)str_cal_radius_acc, LEFT_MODE);
	BSP_LCD_DisplayStringAt(0, 230, (uint8_t*)str_cal_radius_mag, LEFT_MODE);
}

void DrawAll(bno055_t *bno055) {
	DrawTitle();
	DrawOperationMode(bno055->opmode);
	DrawEulerData(bno055->euler_data);
	DrawCalibrationStatus(bno055->cal_status);
	DrawCalibrationProfile(bno055->cal_profile);
}
