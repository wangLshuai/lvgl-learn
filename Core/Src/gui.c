#include "cmsis_os.h"
#include "lvgl.h"
#include "lcd.h"
#include "main.h"

static uint8_t buf[WIDTH * HEIGHT / 10 * 2];
static lv_obj_t *led, *add_button, *del_button, *box, *spinbox;
static uint32_t red_color = 80;
static bool led_status;

static void btn_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *target = lv_event_get_target_obj(e);
  if (code == LV_EVENT_CLICKED)
  {
    if (target == led)
    {
      led_status = !led_status;
    }
    if (target == add_button)
    {
      if (red_color < 0xff)
        red_color++;
    }
    if (target == del_button)
    {
      if (red_color > 80)
        red_color--;
    }
  }
  else if (code == LV_EVENT_LONG_PRESSED)
  {
    if (target == add_button)
    {
      if (red_color < 0xff)
        red_color++;
    }
    if (target == del_button)
    {
      if (red_color > 80)
        red_color--;
    }
  }

  lv_spinbox_set_value(spinbox, red_color);
  if (led_status)
  {
    lv_led_set_color(led, lv_color_hex((red_color << 16) + 0x0000));
  }
  else
  {
    lv_led_set_color(led, lv_color_hex(0x808080));
  }
}

static void next_button_cb(lv_event_t *e)
{
  static bool hiddlen = false;
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    hiddlen = !hiddlen;
    if (hiddlen)
      lv_obj_add_flag(box, LV_OBJ_FLAG_HIDDEN);
    else
      lv_obj_remove_flag(box, LV_OBJ_FLAG_HIDDEN);
    ;
  }
}

static void slider_event_cb(lv_event_t *e)
{
  lv_obj_t *slider = lv_event_get_target_obj(e);
  red_color = lv_slider_get_value(slider);
  lv_spinbox_set_value(spinbox, red_color);
}
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

static void key_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
  /* Default: no key pressed */
  data->key = 0;
  data->state = LV_INDEV_STATE_RELEASED;
  if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin))
  {
    data->key = LV_KEY_NEXT;
    data->state = LV_INDEV_STATE_PRESSED;
  }
  else if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin))
  {
    data->key = LV_KEY_ENTER;
    data->state = LV_INDEV_STATE_PRESSED;
  }
}

void gui_init()
{
  lv_init();
  lv_tick_set_cb(get_tick_cb);
  lv_display_t *disp = lv_display_create(WIDTH, HEIGHT);
  lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, disp_flush_cb);
}
void set_bg_gradient_radial()
{
  static const lv_color_t grad_colors[2] = {LV_COLOR_MAKE(0x98, 0x18, 0x42),
                                            LV_COLOR_MAKE(0x00, 0x00, 0x00)};

  int32_t width = lv_display_get_horizontal_resolution(NULL);
  int32_t height = lv_display_get_vertical_resolution(NULL);

  static lv_style_t style;
  lv_style_init(&style);
  static lv_grad_dsc_t grad;

  lv_grad_init_stops(&grad, grad_colors, NULL, NULL, sizeof(grad_colors) / sizeof(lv_color_t));

  /*Make a radial gradient with the center in the middle of the object, extending to the farthest corner*/
  lv_grad_radial_init(&grad, LV_GRAD_CENTER, LV_GRAD_CENTER, LV_GRAD_RIGHT, LV_GRAD_BOTTOM,
                      LV_GRAD_EXTEND_PAD);

  /*Set gradient as background*/
  lv_style_set_bg_grad(&style, &grad);

  /*Create an object with the new style*/
  lv_obj_t *obj = lv_obj_create(lv_screen_active());
  lv_obj_add_style(obj, &style, 0);
  lv_obj_set_size(obj, width, height);
  lv_obj_center(obj);
}
void StartGUITask(void *argument)
{
  uint32_t wait_ms = 0;
  lcd_init();
  Lcd_gram_scan(1);
  gui_init();

  // set_bg_gradient_radial();

  // static lv_style_t style_transparent;
  // lv_style_init(&style_transparent);
  // lv_style_set_bg_opa(&style_transparent, LV_OPA_TRANSP);

  lv_group_t *g = lv_group_create();
  lv_indev_t *indev = lv_indev_create();

  lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
  lv_indev_set_read_cb(indev, key_read);

  lv_indev_set_group(indev, g);

  // lv_obj_t *tabview = lv_tabview_create(lv_screen_active());
  // lv_obj_set_style_bg_color(tabview, lv_color_hex(0x003a57), LV_PART_MAIN);
  // lv_tabview_set_tab_bar_size(tabview, 45);

  // lv_obj_t *tv0 = lv_tabview_add_tab(tabview, "page0");
  // lv_obj_t *label = lv_label_create(tv0);
  // lv_label_set_text(label, "Hello,world");
  // lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), LV_PART_MAIN);
  // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  spinbox = lv_spinbox_create(lv_screen_active());
  // lv_obj_add_style(box, &style_transparent, LV_PART_MAIN);
  lv_spinbox_set_range(spinbox, 0, 255);
  lv_obj_set_width(spinbox, 120);
  lv_spinbox_set_digit_format(spinbox, 3, 3);
  lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(spinbox, LV_ALIGN_TOP_MID, 0, 0);

  // lv_obj_t *slider = lv_slider_create(tv1);
  // lv_slider_set_range(slider, 80, 255);
  // lv_obj_set_width(slider, 200);
  // lv_obj_align_to(slider, spinbox, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
  // lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

  box = lv_obj_create(lv_screen_active());
  // lv_obj_add_style(box, &style_transparent, LV_PART_MAIN);
  lv_obj_align(box, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_size(box, 200, 150);
  lv_obj_set_layout(box, LV_LAYOUT_GRID);

  static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(box, col_dsc, row_dsc);

  led = lv_led_create(box);
  lv_group_add_obj(g, led);
  lv_obj_set_style_bg_color(led, lv_color_hex(0x808080), 0);
  lv_obj_set_size(led, 50, 50);
  // lv_obj_align(led, LV_ALIGN_TOP_MID, 0, 20);
  // lv_obj_set_style_radius(led, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_grid_cell(led, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
  // lv_group_add_obj(g, led);
  lv_obj_add_event(led, btn_event_cb, LV_EVENT_CLICKED, NULL);

  add_button = lv_button_create(box);
  lv_obj_set_style_bg_img_src(add_button, LV_SYMBOL_PLUS, 0);
  lv_obj_add_event(add_button, btn_event_cb, LV_EVENT_CLICKED | LV_EVENT_LONG_PRESSED, NULL);
  lv_obj_set_grid_cell(add_button, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
  lv_group_add_obj(g, add_button);

  del_button = lv_button_create(box);
  lv_obj_set_style_bg_img_src(del_button, LV_SYMBOL_MINUS, 0);
  lv_obj_add_event_cb(del_button, btn_event_cb, LV_EVENT_CLICKED | LV_EVENT_LONG_PRESSED, NULL);
  lv_obj_set_grid_cell(del_button, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 1, 1);
  lv_group_add_obj(g, del_button);

  lv_obj_t *next_button = lv_button_create(lv_screen_active());
  lv_group_add_obj(g, next_button);
  lv_obj_align(next_button, LV_ALIGN_BOTTOM_LEFT, 10, 0);
  lv_obj_t *next_label = lv_label_create(next_button);
  lv_label_set_text(next_label, "next");
  lv_obj_add_event_cb(next_button, next_button_cb, LV_EVENT_CLICKED, NULL);

  // lv_obj_t *arc = lv_arc_create(lv_screen_active());
  // lv_obj_set_size(arc, 160, 160);
  // lv_arc_set_min_value(arc, 0);
  // lv_arc_set_max_value(arc, 100);
  // lv_arc_set_value(arc, 68);

  // static lv_style_t style_bg;
  // lv_style_init(&style_bg);
  // lv_style_set_arc_color(&style_bg, lv_color_hex(0x6366f1));
  // lv_style_set_arc_width(&style_bg, 14);
  // lv_style_set_arc_opa(&style_bg, (255 * 20 / 100));
  // lv_style_set_arc_rounded(&style_bg, true);

  // lv_obj_add_style(arc, &style_bg, LV_PART_MAIN);
  // lv_tabview_set_active(tabview, 2, false);

  while (1)
  {
    wait_ms = lv_timer_handler();
    osDelayUntil(pdMS_TO_TICKS(wait_ms));
  }
}