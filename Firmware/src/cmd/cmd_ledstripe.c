/*
 * cmd_ledstripe.c
 *
 *  Created on: Apr 4, 2015
 *      Author: Ollo
 */

#include "cmd.h"
#include "ledstripe/ledstripe.h"
#include "usbcdc/usbcdc.h"

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

#define LED_END_POSTION		0

#define	STARTOFFSET				2
#define HEX_SIZE				2
#define TEXTLINE_MAX_LENGTH 	1024

/******************************************************************************
 * GLOBAL VARIABLES for this module
 ******************************************************************************/

/******************************************************************************
 * LOCAL VARIABLES for this module
 ******************************************************************************/
static WORKING_AREA(waLEDstripBlink, 2048);

/******************************************************************************
 * LOCAL FUNCTIONS for this module
 ******************************************************************************/

static msg_t ledThread(void *arg);

/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static msg_t
ledThread(void *arg)
{
  int pos_r = 0;
  int pos_g = 0;
  int pos_b = 0;

  (void) arg;

  while (TRUE)
    {
      //code for running light
      pos_r %= LEDSTRIPE_FRAMEBUFFER_SIZE;
      pos_g %= LEDSTRIPE_FRAMEBUFFER_SIZE;
      pos_b %= LEDSTRIPE_FRAMEBUFFER_SIZE;
      ledstripe_framebuffer[pos_r].red = 255;
      if (pos_r > 0)
        ledstripe_framebuffer[pos_r - 1].red = 64;
      if (pos_r > 1)
        ledstripe_framebuffer[pos_r - 2].red = 10;
      ledstripe_framebuffer[pos_g].green = 0;
      if (pos_g > 0)
        ledstripe_framebuffer[pos_g - 1].green = 0;
      if (pos_g > 1)
        ledstripe_framebuffer[pos_g - 2].green = 0;
      ledstripe_framebuffer[pos_b].blue = 255;
      ;
      if (pos_b > 0)
        ledstripe_framebuffer[pos_b - 1].blue = 64;
      if (pos_b > 1)
        ledstripe_framebuffer[pos_b - 2].blue = 10;
      chThdSleepMilliseconds(10);
      ledstripe_framebuffer[pos_r].red = 0;
      if (pos_r > 0)
        ledstripe_framebuffer[pos_r - 1].red = 0;
      if (pos_r > 1)
        ledstripe_framebuffer[pos_r - 2].red = 0;
      pos_r++;
      ledstripe_framebuffer[pos_g].green = 0;
      if (pos_g > 0)
        ledstripe_framebuffer[pos_g - 1].green = 0;
      if (pos_g > 1)
        ledstripe_framebuffer[pos_g - 2].green = 0;
      pos_g++;
      ledstripe_framebuffer[pos_b].blue = 0;
      if (pos_b > 0)
        ledstripe_framebuffer[pos_b - 1].blue = 0;
      if (pos_b > 1)
        ledstripe_framebuffer[pos_b - 2].blue = 0;
      pos_b++;
    }

  return RDY_OK;
}



static void readDirectWS2812cmd(BaseSequentialStream *chp)
{
	chprintf(chp, "Waiting vof WS12812 commands\r\n");
	int i, length, j = 0;
	char textbuffer[TEXTLINE_MAX_LENGTH];
	int r = usbcdc_readAll(textbuffer, TEXTLINE_MAX_LENGTH);
	if(r >= 2 && (textbuffer[0] == 'W' && textbuffer[1] == 'S'))
	{
		chprintf(chp, "Go one: '%s'\r\n", textbuffer);
		length = r - 2;
		for(i=0; i < length; i+=3){
			  ledstripe_framebuffer[j].red = 	(uint8_t) textbuffer[i+0];
			  ledstripe_framebuffer[j].green = 	(uint8_t) textbuffer[i+1];
			  ledstripe_framebuffer[j].blue = 	(uint8_t) textbuffer[i+2];
			  j++;
		}
	}
	else
	{
		chprintf(chp, "Found only '%s'\r\n", textbuffer);
	}
}

/******************************************************************************
 * GLOBAL FUNCTIONS for this module
 ******************************************************************************/

void cmd_ledctrl(BaseSequentialStream *chp, int argc, char *argv[])
{
  int i;
  if (argc >= 1 && strcmp(argv[0], "test1") == 0)
    {
      chprintf(chp, "Red ...\r\n");
      for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          ledstripe_framebuffer[i].red = 255;
          ledstripe_framebuffer[i].green = 0;
          ledstripe_framebuffer[i].blue = 0;
        }
      chThdSleepMilliseconds(5000);
      chprintf(chp, "Green ...\r\n");
      for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          ledstripe_framebuffer[i].green = 255;
          ledstripe_framebuffer[i].red = 0;
          ledstripe_framebuffer[i].blue = 0;
        }
      chThdSleepMilliseconds(5000);
      chprintf(chp, "Blue ...\r\n");
      for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          ledstripe_framebuffer[i].blue = 255;
          ledstripe_framebuffer[i].red = 0;
          ledstripe_framebuffer[i].green = 0;
        }
      chThdSleepMilliseconds(5000);
    }
  else if (argc >= 1 && strcmp(argv[0], "on") == 0)
    {
      for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          ledstripe_framebuffer[i].red = 255;
          ledstripe_framebuffer[i].green = 255;
          ledstripe_framebuffer[i].blue = 255;
        }
    }
  else if (argc >= 1 && strcmp(argv[0], "off") == 0)
    {
      for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          ledstripe_framebuffer[i].red = 0;
          ledstripe_framebuffer[i].green = 0;
          ledstripe_framebuffer[i].blue = 0;
        }
    }
  else if (argc >= 4 && strcmp(argv[0], "end") == 0)
    {
      int red = atoi(argv[1]);
      int green = atoi(argv[2]);
      int blue = atoi(argv[3]);
      for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          ledstripe_framebuffer[i].red = 0;
          ledstripe_framebuffer[i].green = 0;
          ledstripe_framebuffer[i].blue = 0;
        }
      for (i = LED_END_POSTION; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          ledstripe_framebuffer[i].red = red;
          ledstripe_framebuffer[i].green = green;
          ledstripe_framebuffer[i].blue = blue;
        }
    }
  else if (argc >= 1 && strcmp(argv[0], "start") == 0)
    {
      chprintf(chp, "Start led thread ...");
      chThdCreateStatic(waLEDstripBlink, sizeof(waLEDstripBlink), NORMALPRIO,
          ledThread, NULL);
      chprintf(chp, " Done\r\n");
    }
  else if (argc >= 1 && strcmp(argv[0], "show") == 0)
    {
      chprintf(chp, "\r\n");
      for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++)
        {
          chprintf(chp, "%.2X%.2X%.2X ",
              ledstripe_framebuffer[i].red,
              ledstripe_framebuffer[i].green,
              ledstripe_framebuffer[i].blue);
        }
        chprintf(chp, "\r\n");
    }
  else if (argc >= 1 && strcmp(argv[0], "read") == 0)
    {

      readDirectWS2812cmd(chp);

    }
  else if (argc >= 1)  /* Update the LEDs directly */
  {
	  int i,j= 0, color = 0;
	  long int number = 0;
	  int length = strlen(argv[0]);
	  char pEnd[2] = { '0', '0' };
	  for(i=0; i < length; i+=2){
		  memcpy(pEnd, argv[0] +i, 2);
		  number = strtol(pEnd, NULL, 16);
		  chprintf(chp, "%d %d\r\n", j, number);
		  switch(color)
		  {
		  case 0:
			  ledstripe_framebuffer[j].red = number;
			  break;
		  case 1:
			  ledstripe_framebuffer[j].green = number;
			  break;
		  case 2:
			  ledstripe_framebuffer[j].blue = number;
			  j++;
			  color=-1;
			  break;
		  }
		  color++;
	  }

  }
  else /* Usage */
    {
      chprintf(chp, "possible arguments are:\r\n"
          "-test1\r\n"
          "-start\r\n"
          "-end (red) (green) (blue)\tSet the last box\r\n"
          "-on\r\n"
          "-off\r\n"
          "-read\r\n"
          "-show\r\n");
    }
}
