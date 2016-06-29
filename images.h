//**************************************************************************
//  bitmap/image control variables
//**************************************************************************

//  LED-image indices
#define  LED_IDX_IDLE      0
#define  LED_IDX_OFF       1
#define  LED_IDX_BLUE      2
#define  LED_IDX_GREEN     3
#define  LED_IDX_RED       4
#define  LED_IDX_YELLOW    5

//  images.cpp
bool load_led_images(void);
void release_led_images(void);
// void draw_led(HWND hwndBmp, unsigned idx);
// void draw_ledb(HWND hwndBmp, bool idx);
// HWND resize_led_control(HWND hwnd, unsigned ctrl_id);
// HWND resize_led_control(HWND hwnd, unsigned ctrl_id, bool indicator_active);
