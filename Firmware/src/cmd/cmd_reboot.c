/*
 * cmd_reboot.c
 *
 *  Created on: May 1, 2015
 *      Author: c3ma
 */

#include "ch.h"
#include "chprintf.h"
#include "hal.h"

#define CHPRINT_SAFE(x)	if (chp != NULL){	chprintf(chp, x); }

void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void) argv;
  if (argc > 0)
  {
	  CHPRINT_SAFE("Usage: reboot\r\n");
      return;
  }

  CHPRINT_SAFE("Rebooting the board...\r\n");

  /* Restart the board */
  chSysLock();
	  /*SCB->AIRCR = SCB_AIRCR_SYSRESETREQ_Msk; */
	  /*0x05fa0000 | SCB_AIRCR_VECTKEY_Pos*/;
	  SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
			  SCB_AIRCR_VECTRESET_Msk | SCB_AIRCR_SYSRESETREQ_Msk);
	  while(1);
}

void cmd_poweroff(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void) argv;
  if (argc > 0)
  {
	  CHPRINT_SAFE("Usage: poweroff\r\n");
      return;
  }

  CHPRINT_SAFE("Power down the board...\r\n");
  chSysHalt();
}
