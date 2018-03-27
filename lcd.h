#ifndef _LCD_H_
#define _LCD_H_

//#include "compiler.h"


/*! type for Backlight options : use PWM or IO to drive the backlight
 *
 */
typedef enum {
  backlight_IO = 0,
  backlight_PWM
}backlight_options;

/*! type for Backlight power : increase or decrease the backlight
 *
 */
typedef enum {
  BACKLINGTH_OFF = 0,
  BACKLINGTH_ON
}backlight_state_t;


/*! Initialize the LCD
 *         (need void delay_ms(unsigned short time_ms) function to perform active wait)
 *
 * \param option backlight_IO if no PWM needed, backlight_PWM if PWM needed...
 * \param backlight_on Whether to start with backlight on or off.
 */
extern void lcd_init(int fd);//backlight_options option, Bool backlight_on);

/*! Change the backlight power
 *
 * \param power increase or decrease the backlight...
 */
extern void lcd_set_backlight(int fd, backlight_state_t led);

/*! Show blinking cursor
 *
 */
extern void lcd_show_cursor(int fd);

/*! Hide cursor
 *
 */
extern void lcd_hide_cursor(int fd);

/*! Write a byte at current position
 *
 * \param  data   Input. data to display
 *
 */
extern void lcd_set_cursor_position(int fd, unsigned char column, unsigned char line);

/*! Clear the LCD screen
 *         (need void delay_ms(unsigned short time_ms) function to perform active wait)
 *
 */
extern void lcd_clear_display(int fd);

/*! Write a string
 *
 * \param  string   Input. null terminated string to display
 *
 */
extern void lcd_write_string(int fd, const char *string);

/*! Write a formatted string
 *
 * \param  format   Input. Formatted null terminated string to display
 *
 */
extern void lcd_printf_string(int fd, const char *format, ...);

extern void lcd_fputc(int fd, char c);

#endif // _LCD_H_
