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
#include "cmd/cmd.h"
#include "ledstripe/ledstripe.h"

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define PRINT( ... )	chprintf((BaseSequentialStream *) &SD6, __VA_ARGS__);/**< Uart print */ \
                        usbcdc_print(__VA_ARGS__);

/*===========================================================================*/
/* Command line related. */
/*===========================================================================*/

static const ShellCommand commands[] =
  {
    { "led", cmd_ledctrl },
    { "w", cmd_ws2812 },
    { "mem", cmd_mem },
    { "threads", cmd_threads },
    { NULL, NULL } };

static const ShellConfig shell_cfg1 =
  { (BaseSequentialStream *) &SD6, commands };

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
int
main(void)
{
  int i, red, green, blue;
  int offset = 0;

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
   * Initialize USB serial console
   */
  usbcdc_init(commands);

  /*
   * Shell manager initialization.
   */
  shellInit();

  /*
   * Activates the serial driver 6 and SDC driver 1 using default
   * configuration.
   */
  sdStart(&SD6, NULL);

  chThdSleep(MS2ST(100));

  /*
   * Initialize LedDriver
   */
  ledstripe_init(GPIOB 	/*FIXME this parameter is the default port, and not interpreted. */,
		  GPIOB_PIN0 	/*FIXME this parameter is the default pin, and not interpreted.  */);

  PRINT("\x1b[1J\x1b[0;0HStarting ChibiOS\r\n");
  PRINT("Start blinker thread ...");
  chThdCreateStatic(waThreadBlink, sizeof(waThreadBlink), NORMALPRIO,
      blinkerThread, NULL);
  PRINT(" Done\r\n");

  shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched with output on the serial
   * driver 2.
   */
  while (TRUE)
    {
      usbcdc_process();

      if (palReadPad(GPIOA, GPIOA_BUTTON))
        {
          if (ledstripe_framebuffer[offset].red > 0
              && ledstripe_framebuffer[offset].green > 0
              && ledstripe_framebuffer[offset].blue > 0)
            {
              red = 255;
              green = 0;
              blue = 0;
            }
          else if (ledstripe_framebuffer[offset].red > 0)
            {
              red = 0;
              green = 255;
              blue = 0;
            }
          else if (ledstripe_framebuffer[offset].green > 0)
            {
              red = 0;
              green = 0;
              blue = 255;
            }
          else if (ledstripe_framebuffer[offset].blue > 0)
            {
              red = 0;
              green = 0;
              blue = 0;
            }
          else
            {
              red = green = blue = 255;
            }
          PRINT("Set %.2X%.2X%.2X (RRGGBB)\r\n", red, green, blue);

          /* Update the end of the stripe */
          for (i = offset; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
            {
              ledstripe_framebuffer[i].red = red;
              ledstripe_framebuffer[i].green = green;
              ledstripe_framebuffer[i].blue = blue;
            }
        }

      /* Wait some time, to make the scheduler running tasks with lower prio */
      chThdSleep(MS2ST(500));
    }
}

