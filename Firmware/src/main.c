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

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define UPRINT( ... )	chprintf((BaseSequentialStream *) &SD6, __VA_ARGS__); /**< Uart print */

static int gEventMaskInited=0;  /**< Flag, if the Serial reading logic is already initialized */

#define ENDLESS_UART_LOOP       EventListener elGPSdata; \
      flagsmask_t flags; \
      if (gEventMaskInited == 0) \
      { \
        chEvtRegisterMask((EventSource *)chnGetEventSource(&SD6), &elGPSdata, EVENT_MASK(1)); \
        gEventMaskInited = 1; \
      } \
 \
      chprintf(chp, "Mirroring...\r\n(Press button to quit function)\r\n"); \
      while (!palReadPad(GPIOA, GPIOA_BUTTON)) \
      { \
          /* Found serial reading here:
           * http://forum.chibios.org/phpbb/viewtopic.php?p=12262&sid=5f8c68257a2cd5be83790ce6f7e1282d#p12262 */ \
          chEvtWaitOneTimeout(EVENT_MASK(1), MS2ST(10)); \
          chSysLock(); \
          flags = chEvtGetAndClearFlags(&elGPSdata); \
          chSysUnlock(); \
          \
          if (flags & CHN_INPUT_AVAILABLE) \
          { \
             msg_t charbuf; \
             do \
             { \
                charbuf = chnGetTimeout(&SD6, TIME_IMMEDIATE); \
                if ( charbuf != Q_TIMEOUT ) \
                { \
                   switch ((char)charbuf) \
                   { \
                   case '\n': \
                     chprintf(chp, "\r\n"); \
                     break; \
                   case '\r': \
                      chprintf(chp, "\r\n"); \
                      break; \
                   default: \
                     chprintf(chp, "%c", (char)charbuf); \
                     break; \
                   } \
                } \
             } \
             while (charbuf != Q_TIMEOUT); \
          } \
      }

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


}

void cmd_mirror(BaseSequentialStream *chp, int argc, char *argv[])
{
  if (argc < 1)
  {
    chprintf(chp, "Usage <options>\r\n"
        "The following options are present\r\n"
        "- serial\tAll text from SD6 mirrored to this shell\r\n"
        "- 2serial <arg>\tArguments are written to the serial console\r\n"
        "- 2serial2 <arg>\tArguments are written to the serial console(and listens afterwards)\r\n");
    return;
  }
  else if (strcmp("serial", argv[0]) == 0)
  {
      ENDLESS_UART_LOOP
  }
  else if (strcmp("2serial", argv[0]) == 0)
  {
      if (argc >= 1)
      {
          UPRINT("%s\r\n", argv[1]);
      }
      else
      {
          chprintf(chp, "No Command found\r\n");
      }
  }
  else if (strcmp("2serial2", argv[0]) == 0)
  {
      if (argc >= 1)
      {
          UPRINT("%s\n", argv[1]);
          chThdSleepMilliseconds(50);
          chprintf(chp, "Finshed sending %s\r\n", argv[1]);
          ENDLESS_UART_LOOP
      }
      else
      {
          chprintf(chp, "No Command found\r\n");
      }
  }
  else
  {
      chprintf(chp, "Unkown command\r\n"
          "Use 'mirror' without arguments for help\r\n");
  }
}

static const ShellCommand commands[] = {
		{ "mem", cmd_mem },
		{ "threads", cmd_threads },
		{ "lcd", cmd_lcd },
		{ "mirror", cmd_mirror },
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

	/*
	 * Activates the serial driver 6 and SDC driver 1 using default
	 * configuration.
	 * RX: PC7
	 * TX: PC6
	 */
	sdStart(&SD6, NULL);

	chThdSleep(MS2ST(100));

	UPRINT("Starting ChibiOS\r\n");
	UPRINT("Start blinker thread ...");
	chThdCreateStatic(waThreadBlink, sizeof(waThreadBlink), NORMALPRIO,
			blinkerThread, NULL);
	UPRINT( " Done\r\n");

	UPRINT("Init LCD library ...");
        //FIXME here the logic must be added here
	UPRINT( " Done\r\n");

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

			if (palReadPad(GPIOD, GPIOD_LED5))
                        {
                      	   /*FIXME add here something to be displayed on the screen */   
                        }
			else
                        {
			    /*FIXME Clear screen */
                        }
		}

		/* Wait some time, to make the scheduler running tasks with lower prio */
		chThdSleep(MS2ST(500));
	}
}

