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
  SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
			  SCB_AIRCR_VECTRESET_Msk | SCB_AIRCR_SYSRESETREQ_Msk);
  while(1);
  /* Source: https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy.st.com%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fSystem%20software%20reset%2c%20effects%20and%20privileges&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=2290 */
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
