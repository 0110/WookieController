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

#define DEBUG_PRINT( ... )	chprintf((BaseSequentialStream *) &SD6, __VA_ARGS__);/**< Uart print */

/******************************************************************************
 * LOCAL VARIABLES for this module
 ******************************************************************************/
static WORKING_AREA(waBoblightThread, 4096);

static int channelSize = 0;
static int startChannel = 0;

/******************************************************************************
 * LOCAL FUNCTIONS for this module
 ******************************************************************************/

static void readDirectWS2812cmd(void)
{
	int offset=0,i, j = 0;
	char textbuffer[TEXTLINE_MAX_LENGTH];
	int length = usbcdc_readAll(textbuffer, TEXTLINE_MAX_LENGTH);
	if(length >= 2 && (textbuffer[0] == 0x81 && textbuffer[1] == 0x02))
	{
		startChannel =textbuffer[2];
		channelSize = textbuffer[3];
		DEBUG_PRINT("%3d [%2d chan.]", offset, channelSize);

		for(i=4; i < length; i+=3){
			  /* Check for the beginning */
			  if (textbuffer[0] == 0x55 && textbuffer[1] == 0xAA && textbuffer[2] == 0x00)
			  {
				  offset++;
				  DEBUG_PRINT("\r\n%3d ", offset);
				  i+=2;
				  j=0;
			  }

			  ledstripe_framebuffer[j].red = 	(uint8_t) textbuffer[i+0];
			  ledstripe_framebuffer[j].green = 	(uint8_t) textbuffer[i+1];
			  ledstripe_framebuffer[j].blue = 	(uint8_t) textbuffer[i+2];
			  j++;
			  DEBUG_PRINT("%.2X%.2X%.2X ", textbuffer[i+0], textbuffer[i+1], textbuffer[i+2]);
		}
		DEBUG_PRINT("\r\n");
	}
	else if (length > 0)
	{
		DEBUG_PRINT("Received only: '%s'\r\n", textbuffer);
	}
}


static msg_t
boblightThread(void *arg)
{
	(void) arg;
	systime_t time = chTimeNow();
	int length, i;
	char textbuffer[TEXTLINE_MAX_LENGTH];

	chRegSetThreadName("boblight");

	/*
	 * Initialize USB serial console
	 */
	usbcdc_init(NULL);

	/* Say hello to the host */
	usbcdc_print("WS\n");

	DEBUG_PRINT("Search start...\r\n");
	while (channelSize <= 0)
	{
		length = usbcdc_readAll(textbuffer, TEXTLINE_MAX_LENGTH);
		DEBUG_PRINT("Got %3d \r\n", length);
		for(i=0; i < length - 4; i++)
		{
			DEBUG_PRINT("%2X ", textbuffer[i]);
			if(textbuffer[i] == 0x81 && textbuffer[i+1] == 0x02)
			{
				startChannel =textbuffer[i+2];
				channelSize = textbuffer[i+3];
				DEBUG_PRINT("%3d start channel is %2d; amount is %3d chan.\r\n", i, startChannel, channelSize);
			}
			else if (textbuffer[i] == 0x55 && textbuffer[i+1] == 0xAA)
			{
				uint8_t prefix[2] = {0x55, 0xAA};
				usbcdc_putMemory(prefix, 2);
				DEBUG_PRINT("Prefix\r\n");

				/* Send the WS2812 Channel maximums */
				uint8_t offsetAndMax[2] = {0x00, LEDSTRIPE_FRAMEBUFFER_SIZE};
				usbcdc_putMemory(offsetAndMax, 2);
				DEBUG_PRINT("WS2812 environment\r\n");
				usbcdc_putMemory((uint8_t *) ledstripe_framebuffer, LEDSTRIPE_FRAMEBUFFER_SIZE);
				DEBUG_PRINT("default data\r\n");
			}

		}
		DEBUG_PRINT("\r\n");
		chThdSleepMilliseconds(10);
	}


	DEBUG_PRINT("Start listening...\r\n");
	  while (1)
	  {
		  usbcdc_process();
		  readDirectWS2812cmd();
		  chThdSleepMilliseconds(5);

			/* Send ACK to host each second */
		  if (time + MS2ST(1000) < chTimeNow())
		  {
			uint8_t prefix[2] = {0x55, 0xAA};
			usbcdc_putMemory(prefix, 2);
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
