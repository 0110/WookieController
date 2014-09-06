/*!
 * @file /WookieController/src/HD44780/lcdMain.h
 *
 * @author ollo
 * @date Jul 20, 2014
 *
 * 
 */
#ifndef LCDMAIN_H_
#define LCDMAIN_H_

#include "ch.h"
#include "hal.h"
#include "gfxconf.h"

/** @fn void lcd_startMain(void)
 * @brief start a new thread to display something on the LCD.
 */
void lcd_startMain(void);

#endif /* LCDMAIN_H_ */
