#ifndef __MATRIX_MAIN__
#define __MATRIX_MAIN__

//LED矩阵宽
#define MATRIX_WIDTH          32
//LED矩阵高
#define MATRIX_HEIGHT         8
//点阵屏驱动引脚
#define MATRIX_LED_PIN        17
//屏幕最大亮度
#define MATRIX_MAX_BRIGHTNESS 10

void vMatrixInit(void);

#endif