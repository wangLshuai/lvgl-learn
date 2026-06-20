#include "cmsis_os.h"
#include "lvgl.h"
#include "lcd.h"
#include "main.h"

static uint8_t buf[WIDTH * HEIGHT / 10 * 2];
static lv_obj_t *led, *add_button, *del_button, *boxes[2], *label;
static uint8_t red_color = 80;
static bool led_status;
static uint8_t current_obj_index = 0;

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
  else if (code == LV_EVENT_LONG_PRESSED_REPEAT)
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

  lv_label_set_text_fmt(label, "%d", red_color);
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
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED)
  {
    lv_obj_set_flag(boxes[current_obj_index], LV_OBJ_FLAG_HIDDEN, true);
    current_obj_index++;
    current_obj_index = current_obj_index % (sizeof(boxes) / sizeof(boxes[0]));
    lv_obj_set_flag(boxes[current_obj_index], LV_OBJ_FLAG_HIDDEN, false);
    ;
  }
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

static void add_data(lv_timer_t *t)
{
  lv_obj_t *chart = (lv_obj_t *) lv_timer_get_user_data(t);
  lv_chart_series_t *ser = lv_chart_get_series_next(chart, NULL);

  lv_chart_set_next_value(chart, ser, (int32_t) lv_rand(10, 90));

  uint32_t p = lv_chart_get_point_count(chart);
  uint32_t s = lv_chart_get_x_start_point(chart, ser);
  int32_t *a = lv_chart_get_series_y_array(chart, ser);

  a[(s + 1) % p] = LV_CHART_POINT_NONE;
  a[(s + 2) % p] = LV_CHART_POINT_NONE;
  a[(s + 2) % p] = LV_CHART_POINT_NONE;

  lv_chart_refresh(chart);
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
  static const lv_color_t grad_colors[2] = {LV_COLOR_MAKE(0x9B, 0x18, 0x42),
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
  lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
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

  lv_obj_t *next_button = lv_button_create(lv_screen_active());
  lv_group_add_obj(g, next_button);
  lv_obj_align(next_button, LV_ALIGN_BOTTOM_LEFT, 10, 0);
  lv_obj_t *next_label = lv_label_create(next_button);
  lv_label_set_text(next_label, "next");
  lv_obj_add_event_cb(next_button, next_button_cb, LV_EVENT_CLICKED, NULL);

  boxes[0] = lv_obj_create(lv_screen_active());

  // lv_obj_add_style(boxes[0], &style_transparent, LV_PART_MAIN);
  lv_obj_set_style_border_width(boxes[0], 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(boxes[0], 0, LV_PART_MAIN);
  lv_obj_align(boxes[0], LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_size(boxes[0], 200, 150);
  lv_obj_set_layout(boxes[0], LV_LAYOUT_GRID);

  static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1),
                                 LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(boxes[0], col_dsc, row_dsc);

  led = lv_led_create(boxes[0]);
  lv_group_add_obj(g, led);
  static lv_style_t style_led_focused;
  lv_style_init(&style_led_focused);

  // 设置焦点时的样式：例如添加一个黄色的轮廓线
  lv_style_set_outline_color(&style_led_focused, lv_color_hex(0xFFFF00));
  lv_style_set_outline_width(&style_led_focused, 3);
  lv_style_set_outline_pad(&style_led_focused, 2);
  lv_obj_add_style(led, &style_led_focused, LV_STATE_FOCUSED);
  ;
  lv_obj_set_size(led, 50, 50);
  lv_obj_set_grid_cell(led, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
  lv_obj_add_event(led, btn_event_cb, LV_EVENT_CLICKED, NULL);

  add_button = lv_button_create(boxes[0]);
  lv_obj_set_style_bg_img_src(add_button, LV_SYMBOL_PLUS, 0);
  lv_obj_add_event(add_button, btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_set_grid_cell(add_button, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
  lv_group_add_obj(g, add_button);

  del_button = lv_button_create(boxes[0]);
  lv_obj_set_style_bg_img_src(del_button, LV_SYMBOL_MINUS, 0);
  lv_obj_add_event_cb(del_button, btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_set_grid_cell(del_button, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 1, 1);
  lv_group_add_obj(g, del_button);

  label = lv_label_create(boxes[0]);
  lv_label_set_text_fmt(label, "%d", red_color);
  // lv_obj_add_style(boxes[0], &style_transparent, LV_PART_MAIN);
  // lv_spinbox_set_range(label, 80, 255);
  lv_obj_set_grid_cell(label, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 2, 1);
  // lv_spinbox_set_digit_format(spinbox, 3, 3);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  boxes[1] = lv_obj_create(lv_screen_active());
  lv_obj_set_size(boxes[1], LV_PCT(100), LV_PCT(80));
  lv_obj_align_to(boxes[1], next_button, LV_ALIGN_OUT_TOP_LEFT, 0, -20);
  lv_obj_set_flag(boxes[1], LV_OBJ_FLAG_HIDDEN, true);

  lv_obj_t *chart = lv_chart_create(boxes[1]);
  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
  lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
  lv_obj_set_size(chart, lv_pct(100), lv_pct(100));
  lv_obj_center(chart);

  lv_chart_set_point_count(chart, 80);
  lv_chart_series_t *ser =
    lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
  /*Prefill with data*/
  uint32_t i;
  for (i = 0; i < 80; i++)
  {
    lv_chart_set_next_value(chart, ser, (int32_t) lv_rand(10, 90));
  }

  lv_timer_create(add_data, 300, chart);

  while (1)
  {
    wait_ms = lv_timer_handler();
    osDelayUntil(pdMS_TO_TICKS(wait_ms));
  }
}