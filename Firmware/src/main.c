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
#include "lcd/ssd1803a-spi.h"

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define UPRINT( ... )	chprintf((BaseSequentialStream *) &SD6, __VA_ARGS__); /**< Uart print */

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

void cmd_lcd(BaseSequentialStream *chp, int argc, char *argv[])
{
  int index, strLength;
  int i=0;
  if (argc < 1)
  {
    chprintf(chp, "Usage <input>\r\n");
    return;
  }

  strLength = 0;
  /* Count length of all parameter */
  for(index=0; index < argc; index++)
  {
	  strLength += strlen(argv[index]) + 1 /* for the space between*/;
  }

  strLength --;

  for(i=0;i<strLength;i++)
  {
	  /* Kaepten mag die Loop nicht */
	  if (argv[0][i] == '\0')
	  {
		  argv[0][i] = ' ';
	  } /* But it is necessary! */
  }

  /* Debug output */
 for(i=0; i < strLength; i++)
 {
   chprintf(chp, "%2X (%c)\r\n", (int) argv[0][i], argv[0][i]);
 }


  if (ssd1803a_spi_sendText(argv[0], strLength) != SSD1803A_RET_OK)
  {
      chprintf(chp, "Could not update LCD\r\n");
  }
  else
  {
    chprintf(chp, "Wrote %d characters on the screen\r\n", strLength);
  }

}

static const ShellCommand commands[] = {
		{ "mem", cmd_mem },
		{ "threads", cmd_threads },
		{ "lcd", cmd_lcd },
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
static msg_t blinkerThread(void *arg) {

	(void) arg;
	chRegSetThreadName("blinker");
	while (TRUE) {
		palSetPad(GPIOD, GPIOD_LED4); /* Green.  */
		palSetPad(GPIOA, GPIOA_LEDEXT); /* external LED */
		chThdSleepMilliseconds(500); palClearPad(GPIOD, GPIOD_LED4); /* Green.  */
		palClearPad(GPIOA, GPIOA_LEDEXT); /* external LED */
		chThdSleepMilliseconds(500);
	}
	return RDY_OK;
}

/*
 * Application entry point.
 */
int main(void) {

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

	ssd1803a_spi_init();

	/*
	 * Activates the serial driver 6 and SDC driver 1 using default
	 * configuration.
	 */
	sdStart(&SD6, NULL);

	chThdSleep(MS2ST(100));

	UPRINT("\x1b[1J\x1b[0;0HStarting ChibiOS\r\n");
	UPRINT("Start blinker thread ...");
	chThdCreateStatic(waThreadBlink, sizeof(waThreadBlink), NORMALPRIO,
			blinkerThread, NULL);
	UPRINT( " Done\r\n");

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
			palTogglePad(GPIOD, GPIOD_LED5);	/* Red On*/
			usbcdc_print("Button pressed\r\n");
		}

		/* Wait some time, to make the scheduler running tasks with lower prio */
		chThdSleep(MS2ST(500));
	}
}

