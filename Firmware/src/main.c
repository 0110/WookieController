/** @file main.c
 * @brief Entrypoint of the custom project
 * @author Ollo
 *
 * @date 28.06.2014
 * @defgroup WookieController
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"

#include "usbcdc/usbcdc.h"
#include "esp8266/esp8266.h"
#include "cmd/cmd.h"

#include "rpm/rpm.h"

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define PRINT( ... )	usbcdc_print(__VA_ARGS__); usbcdc_process(); /**< Uart print */


/*===========================================================================*/
/* Command line related. */
/*===========================================================================*/

/******************************************************************************
 * Tiny example to control an OUTPUT-PIN
 */
void cmd_led(BaseSequentialStream *chp, int argc, char *argv[])
{
  if (argc >= 1 && strcmp(argv[0], "test") == 0)
    {
      chprintf(chp, "LED test\r\n");
    }
  else if (argc >= 1 && strcmp(argv[0], "on") == 0)
    {
      palSetPad(GPIOD, GPIOD_LED5); /* Red ON*/
    }
  else if (argc >= 1 && strcmp(argv[0], "off") == 0)
    {
      palClearPad(GPIOD, GPIOD_LED5); /* Red OFF */
    }
  else /* Usage */
    {
      chprintf(chp, "possible arguments are:\r\n"
          "- test\r\n"
          "- on\r\n"
          "- off\r\n");
    }
}

static const ShellCommand commands[] = {
		{ "mem", cmd_mem },
		{ "threads", cmd_threads },
		{ "led" , cmd_led },
		{ NULL, NULL } };

/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/

/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(waThreadBlink, 128);
static msg_t
blinkerThread(void *arg)
{

  (void) arg;
  chRegSetThreadName("blinker");
  while (TRUE)
    {
      palSetPad(GPIOD, GPIOD_LED4); /* Green.  */
      chThdSleepMilliseconds(500);
      palClearPad(GPIOD, GPIOD_LED4); /* Green.  */
      chThdSleepMilliseconds(500);
    }
  return RDY_OK;
}

/*
 * Application entry point.
 */
int main(void)
{
  uint8_t i=0;
  uint32_t speed=0;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 6 to communicate with the WLAN (ESP8266) module
   */
  esp8266_init();

  rpm_init();

  /*
   * Initialize USB serial console
   */
  usbcdc_init(commands);

  /*
   * Shell manager initialization.
   */
  shellInit();

  PRINT("\x1b[1J\x1b[0;0HStarting ChibiOS\r\n");
  PRINT("Start blinker thread ...");
  chThdCreateStatic(waThreadBlink, sizeof(waThreadBlink), NORMALPRIO,
      blinkerThread, NULL);
  PRINT(" Done\r\n");


  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */
  while (TRUE)
  {
      usbcdc_process();

#if 0
      if (palReadPad(GPIOA, GPIOA_BUTTON))
      {
          PRINT("Button pressed\r\n");
          i++;
          /* Send something via the WLAN module: */
          //esp8266_UDPprintln("Button pressed %u times", i);
          palTogglePad(GPIOD, GPIOD_LED6); /* Blue.  */
      }
#else
      rpm_getKMperHour(&speed);
      esp8266_UDPprintln("Frequency %5u ms", speed);
#endif
      /* Wait some time, to make the scheduler running tasks with lower prio */
      chThdSleep(MS2ST(50));
  }
}

