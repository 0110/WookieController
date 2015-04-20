/*
 * boblight.c
 *
 *  Created on: Apr 4, 2015
 *      Author: c3ma
 */

#include "boblight.h"

#include "usbcdc/usbcdc.h"
#include "ledstripe/ledstripe.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"

/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define	STARTOFFSET				2
#define HEX_SIZE				2
#define TEXTLINE_MAX_LENGTH 	1024

#define COLOR_RESET				0
#define COLOR_RED				1
#define COLOR_GREEN				2
#define COLOR_BLUE				3

#define DEBUG_PRINT( ... )	chprintf((BaseSequentialStream *) &SD6, __VA_ARGS__);/**< Uart print */

/******************************************************************************
 * LOCAL VARIABLES for this module
 ******************************************************************************/
static WORKING_AREA(waBoblightThread, 4096);

static int channelSize = 0;
static int ledOffset = 0;
static int startFound = FALSE;

static int colorPosition=0;

/******************************************************************************
 * LOCAL FUNCTIONS for this module
 ******************************************************************************/

static int readDirectWS2812cmd(char *textbuffer)
{
	int i=0;
	int length = usbcdc_read(textbuffer, TEXTLINE_MAX_LENGTH);
	if(length > 0)
	{
		for(i=0; i < length; i++)
		{
			/** Handle Frame beginning */
			if (textbuffer[i] == 'A' && textbuffer[i+1] == 'd' && textbuffer[i+2] == 'a')
			{
				channelSize = textbuffer[i+3] * 256 + textbuffer[i+4];
				i=i+5;
				ledOffset=0;
				colorPosition=COLOR_RED;
				startFound=TRUE;
			}
			else if (startFound == TRUE)
			{
				/* Update the LED information */
				if (ledOffset < LEDSTRIPE_FRAMEBUFFER_SIZE)
				{
					switch(colorPosition)
					{
					case COLOR_RED:
						ledstripe_framebuffer[ledOffset].red = 		(uint8_t) textbuffer[i];
						break;
					case COLOR_GREEN:
						ledstripe_framebuffer[ledOffset].green = 	(uint8_t) textbuffer[i];
						break;
					case COLOR_BLUE:
						ledstripe_framebuffer[ledOffset].blue = 	(uint8_t) textbuffer[i];
						/* Reset for the next LED */
						colorPosition = COLOR_RESET;
						ledOffset++;
						break;
					}
					colorPosition++;
				}
			}
		}
		palTogglePad(GPIOD, GPIOD_LED6); /* Blue.  */
		return TRUE;
	}
	else
	{
		return FALSE; /* Nothing found */
	}
}

static void helper_watchdog(void *p) {

  (void)p;
  chSysHalt();
}

/******************************************************************************
 * GLOBAL FUNCTIONS for this module
 ******************************************************************************/

static msg_t
boblightThread(void *arg)
{
	(void) arg;
	systime_t time = chTimeNow();
	char textbuffer[TEXTLINE_MAX_LENGTH];
	VirtualTimer vt;
	chRegSetThreadName("boblight");

	/*
	 * Initialize USB serial console
	 */
	usbcdc_init(NULL);

	/* Say hello to the host */
	usbcdc_print("Ada\n");
	time = chTimeNow();

	DEBUG_PRINT("Start listening...\r\n");
	while (1)
	{
	  usbcdc_process();
	  memset(textbuffer, 0, TEXTLINE_MAX_LENGTH);

	  /* Starts a VT working as watchdog to catch a malfunction in the USB reading.*/
	  chSysLock();
	  chVTSetI(&vt, MS2ST(2500), helper_watchdog, NULL);
	  chSysUnlock();

	  if (readDirectWS2812cmd(textbuffer) == TRUE)
	  {
		  /* Turn off the LED, as now, we found someone */
		  palClearPad(GPIOD, GPIOD_LED5); /* Red.  */
	  }
	  else
	  {
		  /* Indicate, that no LEDs were communicated */
		  palTogglePad(GPIOD, GPIOD_LED5); /* Red.  */
	  }

	  /* Stops the watchdog.*/
	  chSysLock();
	  if (chVTIsArmedI(&vt))
		chVTResetI(&vt);
	  chSysUnlock();

	  /* Send ACK to host each second */
	  if (time + MS2ST(1000) < chTimeNow())
	  {
		usbcdc_putMemory((uint8_t *) "Ada\n", 4);
		time = chTimeNow();
		DEBUG_PRINT("========== Still alive ===========\r\n")
		DEBUG_PRINT("channel size %4d\tLEDs %4d\r\n", channelSize, ledOffset);

	  }
	  chThdSleepMilliseconds(50);
	}

	  return RDY_OK;
}

void boblight_init(void)
{
	DEBUG_PRINT("Start boblight thread ...");
	  chThdCreateStatic(waBoblightThread, sizeof(waBoblightThread), NORMALPRIO,
	      boblightThread, NULL);
	DEBUG_PRINT(" Done\r\n");
}
