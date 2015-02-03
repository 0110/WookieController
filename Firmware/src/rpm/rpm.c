/*
 * @file rpm.c
 *
 *  @date Feb 3, 2015
 *  @author c3ma
 */


#include "rpm/rpm.h"

#include "ch.h"
#include "hal.h"

#include "usbcdc/usbcdc.h"

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

/******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************/


/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/


rpm_ret_t rpm_init(void)
{
   while(TRUE)
   {
       if (palReadPad(GPIOA, GPIOA_BTNRPM1))
       {
               palSetPad(GPIOD, GPIOD_LED5);   /* Red On*/
               usbcdc_print("Peep\r\n");
       }
       else
       {
         palClearPad(GPIOD, GPIOD_LED5);   /* Red Off*/
       }
       /* Wait some time, to make the scheduler running tasks with lower prio */
       chThdSleep(MS2ST(50));
   }

   /*TODO fill in here the magic */
   return RET_ERROR;
}
