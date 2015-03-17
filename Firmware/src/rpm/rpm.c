/*
 * @file rpm.c
 *
 *  @date Feb 3, 2015
 *  @author c3ma
 */


#include "rpm/rpm.h"

#include "ch.h"
#include "hal.h"
#include "ext.h"
#include "chvt.h"

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

/**
 * @var gRoundPerMinutes
 * @brief the calculated round per minute
 */
static uint32_t gTicksBetweenTrigger = 0;

static volatile uint32_t startTime = 0; /**< Ticks of the clock, for the last input */

/* Triggered when the button is pressed or released. The LED5 is set to ON.*/
static void extcb1(EXTDriver *extp, expchannel_t channel) {

  (void)extp;
  (void)channel;

  palTogglePad(GPIOD, GPIOD_LED5); /* Red ? */
  //chSysLockFromIsr();

  if (startTime > 0)
  {
      gTicksBetweenTrigger = chTimeNow() - startTime;
  }

  startTime = chTimeNow();

  //chSysUnlockFromIsr();
}

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, extcb1},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/



/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/


rpm_ret_t rpm_init(void)
{
  /*
   * Activates the EXT driver 1.
   */
  extStart(&EXTD1, &extcfg);

  extChannelEnable(&EXTD1, 0);

  return RET_OK;
}

rpm_ret_t rpm_getKMperHour(uint32_t*    kmPerH)
{
  /*FIXME for now simply copy the ticks out */
  (*kmPerH) = gTicksBetweenTrigger;

  return RET_ERROR;
}
