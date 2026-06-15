#include "lcd.h"
#include "main.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
/*
  *bank4 base address 6C000000。 A23 to D/Cx
*/
#define LCD_CMD_ADDR  (*((__IO uint16_t *) (0x6C000000))) // A23=0, command
#define lcd_data_ADDR (*((__IO uint16_t *) (0x6D000000))) // A23=1, data

void lcd_cmd(uint16_t cmd)
{
  LCD_CMD_ADDR = cmd;
}
void lcd_write_data(uint16_t data)
{
  lcd_data_ADDR = data;
}

uint16_t lcd_read_data()
{
  uint16_t d = lcd_data_ADDR;
  return d;
}

void lcd_config(void)
{
  /*  Power control B (CFh)  */
  lcd_cmd(0xCF);
  lcd_write_data(0x00);
  lcd_write_data(0x81);
  lcd_write_data(0x30);

  /*  Power on sequence control (EDh) */
  lcd_cmd(0xED);
  lcd_write_data(0x64);
  lcd_write_data(0x03);
  lcd_write_data(0x12);
  lcd_write_data(0x81);

  /*  Driver timing control A (E8h) */
  lcd_cmd(0xE8);
  lcd_write_data(0x85);
  lcd_write_data(0x10);
  lcd_write_data(0x78);

  /*  Power control A (CBh) */
  lcd_cmd(0xCB);
  lcd_write_data(0x39);
  lcd_write_data(0x2C);
  lcd_write_data(0x00);
  lcd_write_data(0x34);
  lcd_write_data(0x02);

  /* Pump ratio control (F7h) */
  lcd_cmd(0xF7);
  lcd_write_data(0x20);

  /* Driver timing control B */
  lcd_cmd(0xEA);
  lcd_write_data(0x00);
  lcd_write_data(0x00);

  /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
  lcd_cmd(0xB1);
  lcd_write_data(0x00);
  lcd_write_data(0x1B);

  /*  Display Function Control (B6h) */
  lcd_cmd(0xB6);
  lcd_write_data(0x0A);
  lcd_write_data(0xA2);

  /* Power Control 1 (C0h) */
  lcd_cmd(0xC0);
  lcd_write_data(0x35);

  /* Power Control 2 (C1h) */
  lcd_cmd(0xC1);
  lcd_write_data(0x11);

  /* VCOM Control 1(C5h) */
  lcd_cmd(0xC5);
  lcd_write_data(0x45);
  lcd_write_data(0x45);

  /*  VCOM Control 2(C7h)  */
  lcd_cmd(0xC7);
  lcd_write_data(0xA2);

  /* Enable 3G (F2h) */
  lcd_cmd(0xF2);
  lcd_write_data(0x00);

  /* Gamma Set (26h) */
  lcd_cmd(0x26);
  lcd_write_data(0x01);

  /* Positive Gamma Correction */
  lcd_cmd(0xE0); //Set Gamma
  lcd_write_data(0x0F);
  lcd_write_data(0x26);
  lcd_write_data(0x24);
  lcd_write_data(0x0B);
  lcd_write_data(0x0E);
  lcd_write_data(0x09);
  lcd_write_data(0x54);
  lcd_write_data(0xA8);
  lcd_write_data(0x46);
  lcd_write_data(0x0C);
  lcd_write_data(0x17);
  lcd_write_data(0x09);
  lcd_write_data(0x0F);
  lcd_write_data(0x07);
  lcd_write_data(0x00);

  /* Negative Gamma Correction (E1h) */
  lcd_cmd(0XE1); //Set Gamma
  lcd_write_data(0x00);
  lcd_write_data(0x19);
  lcd_write_data(0x1B);
  lcd_write_data(0x04);
  lcd_write_data(0x10);
  lcd_write_data(0x07);
  lcd_write_data(0x2A);
  lcd_write_data(0x47);
  lcd_write_data(0x39);
  lcd_write_data(0x03);
  lcd_write_data(0x06);
  lcd_write_data(0x06);
  lcd_write_data(0x30);
  lcd_write_data(0x38);
  lcd_write_data(0x0F);

  /* memory access control set */

  lcd_cmd(0x36);
  lcd_write_data(0xC8); /*竖屏  左上角到(起点)到右下角(终点)扫描方式*/

  /* column address control set */
  lcd_cmd(0X2A);
  lcd_write_data(0x00);
  lcd_write_data(0x00);
  lcd_write_data(0x00);
  lcd_write_data(0xEF);

  /* page address control set */
  lcd_cmd(0X2B);
  lcd_write_data(0x00);
  lcd_write_data(0x00);
  lcd_write_data(0x01);
  lcd_write_data(0x3F);

  /*  Pixel Format Set (3Ah)  */
  lcd_cmd(0x3a);
  lcd_write_data(0x55);
  /* Sleep Out (11h)  */
  lcd_cmd(0x11);
  osDelay(pdMS_TO_TICKS(100));

  /* Display ON (29h) */
  lcd_cmd(0x29);
}

void lcd_reset()
{
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
  osDelay(pdMS_TO_TICKS(100));
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
  osDelay(pdMS_TO_TICKS(100));
}

void lcd_init()
{
  lcd_reset();
  lcd_config();
  HAL_GPIO_WritePin(LCD_BLACK_LIGHT_GPIO_Port, LCD_BLACK_LIGHT_Pin, GPIO_PIN_RESET);
}

void Lcd_gram_scan(uint16_t option)
{
  switch (option)
  {
  case 1:
  { /* 左上角 是原点 ，-> 右上角是 x+      显示中英文时用的是这种模式 */
    lcd_cmd(0x36);
    lcd_write_data(0xC8);
    lcd_cmd(0X2A);
    lcd_write_data(0x00); /* x start */
    lcd_write_data(0x00);
    lcd_write_data(0x00); /* x end */
    lcd_write_data(0xEF);

    lcd_cmd(0X2B);
    lcd_write_data(0x00); /* y start */
    lcd_write_data(0x00);
    lcd_write_data(0x01); /* y end */
    lcd_write_data(0x3F);
  }
  break;
  case 2:
  { /* 左下角 是原点 -> 左上角是 x+      显示摄像头图像时用的是这种模式 */
    lcd_cmd(0x36);
    lcd_write_data(0x68);
    lcd_cmd(0X2A);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x01);
    lcd_write_data(0x3F);

    lcd_cmd(0X2B);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0xEF);
  }
  break;
  case 3:
  { /* 右下角是原点 右上角是 x+     显示BMP图片时用的是这种模式 */
    lcd_cmd(0x36);
    lcd_write_data(0x28);
    lcd_cmd(0X2A);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x01);
    lcd_write_data(0x3F);

    lcd_cmd(0X2B);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0xEF);
  }
  break;
  case 4:
  { /* 左下角是原点，右下角是x+     显示BMP图片时用的是这种模式 */
    lcd_cmd(0x36);
    lcd_write_data(0x48);
    lcd_cmd(0X2A);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0xEF);

    lcd_cmd(0X2B);
    lcd_write_data(0x00);
    lcd_write_data(0x00);
    lcd_write_data(0x01);
    lcd_write_data(0x3F);
  }
  break;
  }

  /* write gram start */
  lcd_cmd(0x2C);
}

void lcd_clear(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
  uint32_t i = 0;

  /* column address control set */
  lcd_cmd(0X2A);
  lcd_write_data(x >> 8); /* 先高8位，然后低8位 */
  lcd_write_data(x & 0xff); /* column start   */
  lcd_write_data((x + width - 1) >> 8); /* column end   */
  lcd_write_data((x + width - 1) & 0xff);

  /* page address control set */
  lcd_cmd(0X2B);
  lcd_write_data(y >> 8); /* page start   */
  lcd_write_data(y & 0xff);
  lcd_write_data((y + height - 1) >> 8); /* page end     */
  lcd_write_data((y + height - 1) & 0xff);

  /* memory write */
  lcd_cmd(0x2c);

  for (i = 0; i < width * height; i++)
  {
    lcd_write_data(color);
    //Delay(0x0FFf);
  }
}

void lcd_set_cursor(uint16_t x, uint16_t y)
{
  lcd_cmd(0X2A); /* 设置X坐标 */
  lcd_write_data(x >> 8); /* 先高8位，然后低8位 */
  lcd_write_data(x & 0xff); /* 设置起始点和结束点*/
  lcd_write_data(x >> 8);
  lcd_write_data(x & 0xff);

  lcd_cmd(0X2B); /* 设置Y坐标*/
  lcd_write_data(y >> 8);
  lcd_write_data(y & 0xff);
  lcd_write_data(y >> 8);
  lcd_write_data(y & 0xff);
}

void lcd_open_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  lcd_cmd(0X2A); /* 设置X坐标 */
  lcd_write_data(x >> 8); /* 先高8位，然后低8位 */
  lcd_write_data(x & 0xff); /* 设置起始点和结束点*/
  lcd_write_data((x + width - 1) >> 8);
  lcd_write_data((x + width - 1) & 0xff);

  lcd_cmd(0X2B); /* 设置Y坐标*/
  lcd_write_data(y >> 8);
  lcd_write_data(y & 0xff);
  lcd_write_data((y + height - 1) >> 8);
  lcd_write_data((y + height - 1) & 0xff);

  lcd_cmd(0x2C);
}

void lcd_set_point(uint16_t x, uint16_t y, uint16_t color)
{
  lcd_open_window(x, y, 1, 1);
  lcd_write_data(color);
}
void lcd_fulsh_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *pixel)
{
  lcd_open_window(x, y, width, height);
  for (uint16_t h = 0; h < height; h++)
  {
    for (uint16_t w = 0; w < width; w++)
    {
      lcd_write_data(pixel[h * width + w]);
    }
  }
}

uint16_t lcd_get_point(uint16_t x, uint16_t y)
{
  uint16_t temp;
  uint16_t R = 0, G = 0, B = 0;

  lcd_set_cursor(x, y);
  lcd_cmd(0x2e); /* 读数据 */

  R = lcd_read_data(); /*FIRST READ OUT DUMMY DATA*/
  R = lcd_read_data(); /*READ OUT RED DATA  */
  B = lcd_read_data(); /*READ OUT BLACK DATA*/
  G = lcd_read_data(); /*READ OUT GREEN DATA*/
  temp = (((R >> 11) << 11) | ((G >> 10) << 5) | (B >> 11));
  return (temp);
}
