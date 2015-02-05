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
/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(waThreadRpm, RPM_THREAD_WORKING_AREA_SIZE);

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

static msg_t rpmThread(void *arg) {

        (void) arg;
        chRegSetThreadName("rpm");
        while(TRUE)
        {
            if (palReadPad(GPIOA, GPIOA_BTNRPM1))
            {
               palSetPad(GPIOD, GPIOD_LED5);   /* Red On*/
            }
            else
            {
                palClearPad(GPIOD, GPIOD_LED5);   /* Red Off*/
            }
            /* Wait some time, to make the scheduler running tasks with lower prio */
            chThdSleep(MS2ST(5));
        }
        return RDY_OK;
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

rpm_ret_t rpm_init(void)
{
  chThdCreateStatic(waThreadRpm, sizeof(waThreadRpm), NORMALPRIO,
                          rpmThread, NULL);
  return RET_OK;
}
