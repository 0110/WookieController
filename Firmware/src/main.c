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

void cmd_temploop(BaseSequentialStream *chp, int argc, char *argv[])
{
  int32_t temp=0;
  (void) argc;
  (void) argv;
  chprintf(chp, "Endless loop printing the temperature values:\r\n");
  chprintf(chp, "Pressing the Button will stop it.\r\n");
  chprintf(chp, "=============================================\r\n");
  while (TRUE) {
    kty81_read(&temp);
    chprintf(chp, "%3d\r\n", temp);
    chThdSleep(MS2ST(500));
    if (palReadPad(GPIOA, GPIOA_BUTTON))
    {
        chprintf(chp, "Endless loop found an end: the user\r\n");
        break;
    }
  }
}

static const ShellCommand commands[] = {
		{ "mem", cmd_mem },
		{ "threads", cmd_threads },
		{ "temploop" , cmd_temploop },
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

/*
 * Application entry point.
 */
int main(void)
{
    int32_t temp=0;
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
                    usbcdc_print("Temperature is %3d°C\r\n", temp);
            }

            /* Wait some time, to give the scheduler a chance to run tasks with lower prio */
            chThdSleep(MS2ST(200));
    }
}

