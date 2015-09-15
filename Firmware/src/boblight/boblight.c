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

#define BOBLIGHT_MAILBOX_SIZE	1000
#define BOBLIGHT_MAILBOX_DECR	50		/**< Amount of entries that are removed by one step */
#define BOBLIGHT_MAILBOX_DECR_BORDER 100 /**< When Mailbox increased this amount,  BOBLIGHT_MAILBOX_DECR is used for decrementing */

#define LOGGING_FACTOR		10

/******************************************************************************
 * LOCAL VARIABLES for this module
 ******************************************************************************/
static WORKING_AREA(waBoblightThread, 4096);

static int channelSize = 0;
static int ledOffset = 0;
static int startFound = FALSE;

static int colorPosition=0;

uint32_t*	gBoblightMailboxBuffer = NULL;
Mailbox *	gBoblightMailbox = NULL;

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
		palClearPad(GPIOD, GPIOD_LED6); /* Turn off Blue, nothing new found */
		return FALSE; /* Nothing found */
	}
}

/******************************************************************************
 * GLOBAL FUNCTIONS for this module
 ******************************************************************************/

static msg_t
boblightThread(void *arg)
{
	int loggingOutput=LOGGING_FACTOR;
	(void) arg;
	systime_t time = chTimeNow();
	char textbuffer[TEXTLINE_MAX_LENGTH];
	chRegSetThreadName("boblight");

	/* Initialize the Message queue */
	 gBoblightMailboxBuffer = chHeapAlloc(NULL,
	      sizeof(uint32_t) * BOBLIGHT_MAILBOX_SIZE);
	  MAILBOX_DECL(boblightMailbox, gBoblightMailboxBuffer, BOBLIGHT_MAILBOX_SIZE);
	  gBoblightMailbox = &boblightMailbox;

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

	  /* check if we can update the "I'm alive mailbox", or if it is full */
	  if (chMBGetUsedCountI(gBoblightMailbox) < BOBLIGHT_MAILBOX_SIZE)
	  {
		  /* Put something into the Mailbox to say, that we are still alive */
		  chSysLock();
		  chMBPostI(gBoblightMailbox, (uint32_t) 1);
		  chSysUnlock();
	  }

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

	  /* Send ACK to host each second */
	  if (time + MS2ST(1000) < chTimeNow())
	  {
		usbcdc_putMemory((uint8_t *) "Ada\n", 4);

		loggingOutput--;
		if (loggingOutput <= 0)
		{
			usbcdc_print("%s at %s Version:%s\tAlive-Mailbox: %2d\r\n", __DATE__, __TIME__,BOBLIGHT_VERSION, chCoreStatus(), chMBGetUsedCountI(gBoblightMailbox));
			loggingOutput=LOGGING_FACTOR;
		}

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

int boblight_alive(void)
{
	msg_t msg1;
	int newMessages, i;

	/* Use nonblocking function to count incoming messages */
	newMessages = chMBGetUsedCountI(gBoblightMailbox);

	if (newMessages > 0)
	{
		/* Remove only one message, it will take some time, until the dead thread is recognized */
	    chMBFetch(gBoblightMailbox, &msg1, TIME_INFINITE);
	    if (newMessages > BOBLIGHT_MAILBOX_DECR_BORDER)
	    {
	    	for(i=0; i < BOBLIGHT_MAILBOX_DECR; i++)
	    	{
	    		chMBFetch(gBoblightMailbox, &msg1, TIME_INFINITE);
	    	}
	    }
		return 1;
	}
	else

	{
		return 0;
	}
}
