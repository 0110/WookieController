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
#define TEXTLINE_MAX_LENGTH 	256

#define DEBUG_PRINT( ... )	chprintf((BaseSequentialStream *) &SD6, __VA_ARGS__);/**< Uart print */

/******************************************************************************
 * LOCAL VARIABLES for this module
 ******************************************************************************/
static WORKING_AREA(waBoblightThread, 4096);

static int channelSize = 0;

/******************************************************************************
 * LOCAL FUNCTIONS for this module
 ******************************************************************************/

static void readDirectWS2812cmd(char *textbuffer)
{
	int i=0,j = 0;
	int length = usbcdc_readAll(textbuffer, TEXTLINE_MAX_LENGTH);
	if(length >= 2 && (textbuffer[i] == 'A' && textbuffer[i+1] == 'd' && textbuffer[i+2] == 'a'))
	{
		channelSize = textbuffer[i+3] * 256 + textbuffer[i+4];
		/*DEBUG_PRINT("%3d [%2d chan. CRC: %2X]", i, channelSize, textbuffer[i+5]);*/

		for(i=6; i < length; i+=3)
		{
			if (textbuffer[i] == 'A' && textbuffer[i+1] == 'd' && textbuffer[i+2] == 'a')
			{
				channelSize = textbuffer[i+3] * 256 + textbuffer[i+4];
				/*DEBUG_PRINT("\r\n%3d [%2d chan. CRC: %2X]", i, channelSize, textbuffer[i+5]);*/
				i+=6;
				j=0;
			}

			ledstripe_framebuffer[j].red = 	(uint8_t) textbuffer[i+0];
			ledstripe_framebuffer[j].green = 	(uint8_t) textbuffer[i+1];
			ledstripe_framebuffer[j].blue = 	(uint8_t) textbuffer[i+2];
			j++;
			/*DEBUG_PRINT("%.2X%.2X%.2X ", textbuffer[i+0], textbuffer[i+1], textbuffer[i+2]); */
		}
		/* DEBUG_PRINT("\r\n"); */
	}
}


static msg_t
boblightThread(void *arg)
{
	(void) arg;
	systime_t time = chTimeNow();
	char textbuffer[TEXTLINE_MAX_LENGTH];

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
		  readDirectWS2812cmd(textbuffer);
		  chThdSleepMilliseconds(5);

			/* Send ACK to host each second */
		  if (time + MS2ST(1000) < chTimeNow())
		  {
			usbcdc_putMemory((uint8_t *) "Ada\n", 4);
			time = chTimeNow();
			DEBUG_PRINT("Still alive\r\n");
		  }
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