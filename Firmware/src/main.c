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

#include "gfx.h"

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

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
		palSetPad(GPIOD, GPIOD_LED5);	/* Red ON*/
	}
	else if (argc >= 1 && strcmp(argv[0], "off") == 0)
	{
		palClearPad(GPIOD, GPIOD_LED5);	/* Red OFF */
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
	uint8_t charmap[8];
	uint8_t n;

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

	shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);


	halInit();
	chSysInit();
	tdispInit();

	/* reset cursor position and clear the screen */
 	tdispHome();
	tdispClear();

	/* draw a string to a given location */
	tdispSetCursor(0, 0);
	tdispDrawChar('C');
	chThdSleepMilliseconds(500);
	tdispDrawChar('h');
	chThdSleepMilliseconds(500);
	tdispDrawChar('i');
	chThdSleepMilliseconds(500);
	tdispDrawChar('b');
	chThdSleepMilliseconds(500);
	tdispDrawChar('i');
	chThdSleepMilliseconds(500);
	tdispDrawChar('O');
	chThdSleepMilliseconds(500);
	tdispDrawChar('S');
	chThdSleepMilliseconds(500);
	tdispDrawChar('/');
	chThdSleepMilliseconds(500);
	tdispDrawChar('G');
	chThdSleepMilliseconds(500);
	tdispDrawChar('F');
	chThdSleepMilliseconds(500);
	tdispDrawChar('X');
	chThdSleepMilliseconds(500);

	// After 2 seconds blank the screen and
	// print tekst on the next line en 
	// enable the display again
	chThdSleepMilliseconds(1000);
	tdispDisplayMode(displayOff);
	tdispSetCursor(1, 1);
	tdispDrawString("TDISP/HD44780");
	chThdSleepMilliseconds(1000);
	tdispDisplayMode(displayOn);

	/* create and display a custom made character after 1S*/
	chThdSleepMilliseconds(1000);
	charmap[0] = 0b00100;
	charmap[1] = 0b01110;
	charmap[2] = 0b01110;
	charmap[3] = 0b01110;
	charmap[4] = 0b11111;
	charmap[5] = 0b00000;
	charmap[6] = 0b00100;
	charmap[7] = 0b00000;
	tdispCreateChar(0, charmap);
	tdispSetCursor(0, 1);
	tdispDrawChar(0);
	

	// after 1 second turn to 50%
	chThdSleepMilliseconds(1000);
	tdispSetBacklight(50);


	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state, when the button is
	 * pressed the test procedure is launched with output on the serial
	 * driver 2.
	 */
	while (TRUE) {

		tdispSetCursor(0, 3);
	  tdispDrawString("Cursor   OFF: ");
	  tdispSetCursorShape(cursorOff);
	  chThdSleepMilliseconds(5000);

	  tdispSetCursor(0, 3);
	  tdispDrawString("Cursor    ON: ");
	  tdispSetCursorShape(cursorBlock);
	  chThdSleepMilliseconds(5000);

	 /* tdispSetCursor(0, 3);
	  tdispDrawString("Cursor BLINK: ");
	  tdispSetCursorShape(cursorBlinkingBlock);
	  chThdSleepMilliseconds(5000);
	 */
		usbcdc_process();

		if (palReadPad(GPIOA, GPIOA_BUTTON))
		{
			palSetPad(GPIOD, GPIOD_LED5);	/* Red On*/
			usbcdc_print("Button pressed\r\n");
		}

		/* Wait some time, to make the scheduler running tasks with lower prio */
		chThdSleep(MS2ST(500));
	}
}

