/*!
 * @file HD44780/lcdMain.c
 *
 * @author ollo
 * @date Jul 20, 2014
 *
 * 
 */

#include "lcdMain.h"
#include "tdisp/tdisp.h"

static WORKING_AREA(waThreadTdisp, 128);

static msg_t ThreadTdisp(void *arg) {

  (void)arg;

  chRegSetThreadName("Tdisp");


  tdispInit();


  tdispHome();

  tdispClear();

  chThdSleepMilliseconds(50);


  tdispDrawString("STM32 ChibiOS HD44780");
  tdispSetCursor(0, 1);
  tdispDrawString(":-D   ");

  chThdSleepMilliseconds(1000);

  tdispScroll(displayScrollLeft, -1, 500);

  return (msg_t) 0;
}

void lcd_startMain()
{
  chThdCreateStatic(waThreadTdisp, sizeof(waThreadTdisp), NORMALPRIO, ThreadTdisp, NULL);
}
