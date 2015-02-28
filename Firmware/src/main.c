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

#include "esp8266/esp8266.h"

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

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

void cmd_esp8266(BaseSequentialStream *chp, int argc, char *argv[])
{
  if (argc >= 3 && strcmp(argv[0], "connect") == 0)
  {
    if (esp8266_init(argv[1], argv[2]) == RET_OK)
    {
        chThdSleepMilliseconds(100);
        esp8266_printIP(chp);
    }
    else
    {
        chprintf(chp, "ERROR! Could not connect to WiFi module\r\n");
    }
  }
  else if (argc >= 1 && strcmp(argv[0], "reconnect") == 0)
  {
    if (esp8266_init(NULL, NULL) == RET_OK)
    {
      chThdSleepMilliseconds(100);
      esp8266_printIP(chp);
    }
    else
    {
        chprintf(chp, "ERROR! Could not connect to WiFi module\r\n");
    }
  }
  else if (argc >= 1 && strcmp(argv[0], "ip") == 0)
  {
    esp8266_printIP(chp);
  }
  else
  {
    chprintf(chp, "possible arguments are:\r\n"
        "- connect <ssid> <password>\tConnect to the given WiFi with its password\r\n"
        "- reconnect\t\t\tConnect to the last connected WiFi\r\n"
        "- ip\t\t\t\tprint the current IP address\r\n");
  }
}

static const ShellCommand commands[] = {
		{ "mem", cmd_mem },
		{ "threads", cmd_threads },
		{ "led" , cmd_led },
		{ "esp" , cmd_esp8266 },
		{ NULL, NULL } };

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

	chThdSleep(MS2ST(100));

	chThdCreateStatic(waThreadBlink, sizeof(waThreadBlink), NORMALPRIO,
			blinkerThread, NULL);

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
			palSetPad(GPIOD, GPIOD_LED5);	/* Red On*/
			usbcdc_print("Button pressed\r\n");
		}

		/* Wait some time, to make the scheduler running tasks with lower prio */
		chThdSleep(MS2ST(500));
	}
}

