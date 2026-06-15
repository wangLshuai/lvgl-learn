#ifndef __LCD_H
#define __LCD_H
#include "stdint.h"
#define WHITE      0xFFFF /* 白色 */
#define BLACK      0x0000 /* 黑色 */
#define GREY       0xF7DE /* 灰色 */
#define BLUE       0x001F /* 蓝色 */
#define BLUE2      0x051F /* 浅蓝色 */
#define RED        0xF800 /* 红色 */
#define MAGENTA    0xF81F /* 红紫色，洋红色 */
#define GREEN      0x07E0 /* 绿色 */
#define CYAN       0x7FFF /* 蓝绿色，青色 */
#define YELLOW     0xFFE0 /* 黄色 */
#define BRED       0XF81F
#define GRED       0XFFE0
#define GBLUE      0X07FF

#define BACKGROUND BLACK

#define HEIGHT     320
#define WIDTH      240

void lcd_init();
void Lcd_gram_scan(uint16_t option);
void lcd_clear(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void lcd_set_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_fulsh_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *pixel);
#endif