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

#include "kty81/kty81.h"

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define BRANCH_NAME  "KTY81"

#define UPRINT( ... )	chprintf((BaseSequentialStream *) &SD6, __VA_ARGS__); /**< Uart print */

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

void cmd_led(BaseSequentialStream *chp, int argc, char *argv[])
{
	if (argc >= 1 && strcmp(argv[0], "test") == 0)
	{
		chprintf(chp, "LED test\r\n");
	}
	else if (argc >= 1 && strcmp(argv[0], "on") == 0)
	{
		palSetPad(GPIOA, GPIOA_LEDEXT);	/* External LED on: PA8 ON*/
	}
	else if (argc >= 1 && strcmp(argv[0], "off") == 0)
	{
		palClearPad(GPIOA, GPIOA_LEDEXT); /* External LED on: PA8 OFF */
	}
	else	/* Usage */
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
		chThdSleepMilliseconds(500);
		palClearPad(GPIOD, GPIOD_LED4); /* Green.  */
		chThdSleepMilliseconds(500);
	}
	return RDY_OK;
}

volatile int32_t temp=0;

/*
 * Application entry point.
 */
int main(void)
{
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

	UPRINT("\x1b[1J\x1b[0;0HStarting ChibiOS\r\n");
	UPRINT("Start blinker thread ...");
	chThdCreateStatic(waThreadBlink, sizeof(waThreadBlink), NORMALPRIO,
			blinkerThread, NULL);
	UPRINT( " Done\r\n");

	kty81_init();

	shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state, when the button is
	 * pressed the test procedure is launched with output on the serial
	 * driver 2.
	 */
	while (TRUE) {
		usbcdc_process();

		if (palReadPad(GPIOA, GPIOA_BUTTON))
		{
			usbcdc_print("Button pressed (Branch is " BRANCH_NAME " )\r\n");
			kty81_read(&temp);
			chThdSleep(MS2ST(10));
			usbcdc_print("Value are %5d\r\n", temp);
		}

		/* Wait some time, to make the scheduler running tasks with lower prio */
		chThdSleep(MS2ST(200));
	}
}

