/*
 * lcd.h
 *
 *  Created on: Jan 15, 2020
 *      Author: young
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "bno_055.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"

#define RGB565_BLACK       0x0000      /*   0,   0,   0 */
#define RGB565_NAVY        0x000F      /*   0,   0, 128 */
#define RGB565_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define RGB565_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define RGB565_MAROON      0x7800      /* 128,   0,   0 */
#define RGB565_PURPLE      0x780F      /* 128,   0, 128 */
#define RGB565_OLIVE       0x7BE0      /* 128, 128,   0 */
#define RGB565_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define RGB565_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define RGB565_BLUE        0x001F      /*   0,   0, 255 */
#define RGB565_GREEN       0x07E0      /*   0, 255,   0 */
#define RGB565_CYAN        0x07FF      /*   0, 255, 255 */
#define RGB565_RED         0xF800      /* 255,   0,   0 */
#define RGB565_MAGENTA     0xF81F      /* 255,   0, 255 */
#define RGB565_YELLOW      0xFFE0      /* 255, 255,   0 */
#define RGB565_WHITE       0xFFFF      /* 255, 255, 255 */
#define RGB565_ORANGE      0xFD20      /* 255, 165,   0 */
#define RGB565_GREENYELLOW 0xAFE5      /* 173, 255,  47 */

void LCD_Initialization();
void DrawAll(bno055_t *bno055);

void DrawTitle();
void DrawOperationMode(uint8_t opmode);
void DrawEulerData(short *euler_data);
void DrawCalibrationStatus(uint8_t *cal_status);
void DrawCalibrationProfile(uint16_t *cal_profile);


#endif /* INC_LCD_H_ */
