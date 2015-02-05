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
* DEFINITIONS
******************************************************************************/

#define SLEEP_TIME        5     /**< Time to wait between to checks if the PIN is high */
#define REFERENCE_TIME    100   /**< Reference time to reach */
#define TICKS_REFTIME2RPM 600   /**< Convention factor between the reference time to RPM */

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

/**
 * @var gRoundPerMinutes
 * @brief the calculated round per minute
 */
static uint32_t gRoundPerMinutes = 0;

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

static msg_t rpmThread(void *arg)
{
    uint32_t trigger = 0U;
    uint32_t i = 0U;
    (void) arg;
    chRegSetThreadName("rpm");

    while(TRUE)
    {
        trigger = 0U;
        for(i=0U; i < REFERENCE_TIME; i+= SLEEP_TIME)
        {
          if (palReadPad(GPIOA, GPIOA_BTNRPM1))
          {
              trigger++;
          }
          chThdSleep(MS2ST(SLEEP_TIME));
        }
        gRoundPerMinutes = trigger * TICKS_REFTIME2RPM;
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

rpm_ret_t rpm_getKMperHour(uint32_t*    kmPerH)
{
  /*FIXME for now simply copy the round per minutes */
  (*kmPerH) = gRoundPerMinutes;

  return RET_ERROR;
}
