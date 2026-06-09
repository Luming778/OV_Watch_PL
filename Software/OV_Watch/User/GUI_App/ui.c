#include "ui.h"
#include "lcd_init.h"   /* LCD_W / LCD_H */

static int lvgl_cnt = 0;
static lv_obj_t *lvgl_cnt_label;

static void lvgl_btn_click_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    lvgl_cnt++;
    lv_label_set_text_fmt(lvgl_cnt_label, "Taps: %d", lvgl_cnt);
}

void ui_init(void)
{
    lv_obj_t *scr = lv_scr_act();

    /* ---------- 1. 标题 ---------- */
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "LVGL v8 移植测试");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 12);

    /* ---------- 2. 屏幕信息 ---------- */
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text_fmt(info,
        "STM32F411 + FreeRTOS\n"
        "ST7789  %dx%d (竖屏)\n"
        "SPI DMA + LVGL v8.2",
        LCD_W, LCD_H);
    lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(info, lv_color_hex(0xAAAAAA), 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, -30);

    /* ---------- 3. 测试按钮 ---------- */
    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 160, 48);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1E88E5), 0);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, LV_SYMBOL_OK "  Press Me");
    lv_obj_center(btn_label);
    lv_obj_add_event_cb(btn, lvgl_btn_click_cb, LV_EVENT_CLICKED, NULL);

    /* ---------- 4. 点击计数 ---------- */
    lvgl_cnt_label = lv_label_create(scr);
    lv_label_set_text_fmt(lvgl_cnt_label, "Taps: %d", lvgl_cnt);
    lv_obj_set_style_text_color(lvgl_cnt_label, lv_color_hex(0x4CAF50), 0);
    lv_obj_align(lvgl_cnt_label, LV_ALIGN_BOTTOM_MID, 0, -20);
}
