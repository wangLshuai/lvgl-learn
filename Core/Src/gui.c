#include "cmsis_os.h"
#include "lvgl.h"
#include "lcd.h"

static uint8_t buf[WIDTH * HEIGHT / 10 * 2];

void disp_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_buf)
{
  lcd_fulsh_window(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1,
                   (uint16_t *) px_buf);
  lv_display_flush_ready(disp);
}

uint32_t get_tick_cb(void)
{
  return xTaskGetTickCount();
}

void gui_init()
{
  lv_init();
  lv_tick_set_cb(get_tick_cb);
  lv_display_t *disp = lv_display_create(WIDTH, HEIGHT);
  lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, disp_flush_cb);
}
void StartGUITask(void *argument)
{
  uint32_t wait_ms = 0;
  lcd_init();
  Lcd_gram_scan(1);
  gui_init();

  static lv_style_t style1;
  lv_style_init(&style1);
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text(label, "Hello LVGL!");

  while (1)
  {
    wait_ms = lv_timer_handler();
    osDelayUntil(pdMS_TO_TICKS(wait_ms));
  }
}