/*
 * cmd_reboot.c
 *
 *  Created on: May 1, 2015
 *      Author: c3ma
 */

#include "ch.h"
#include "chprintf.h"

#define CHPRINT_SAFE(x)	if (chp != NULL){	chprintf(chp, x); }

void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void) argv;
  if (argc > 0)
  {
	  CHPRINT_SAFE("Usage: reboot\r\n");
      return;
  }


}
