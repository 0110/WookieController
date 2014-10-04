#include "ch.h"
#include "hal.h"
#include "ledstripe/ledstripe.h"

/**
 * Stack area for the dmx thread.
 */
WORKING_AREA(wa_ledstripe, LEDSTRIPE_THREAD_STACK_SIZE);

static uint8_t ledstripe_buffer[LEDSTRIPE_MAXIMUM];

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

static void spicb(SPIDriver *spip);

/*
 * SPI2 configuration structure.
 * Speed 21MHz, CPHA=0, CPOL=0, 16bits frames, MSb transmitted first.
 * The slave select line is the pin 12 on the port GPIOA.
 */
static const SPIConfig spi2cfg = {
  spicb,
  /* HW dependent part.*/
  GPIOB,
  12,
  SPI_CR1_DFF
};

/*
 * SPI end transfer callback.
 */
static void spicb(SPIDriver *spip) {

  /* On transfer end just releases the slave select line.*/
  chSysLockFromIsr();
  spiUnselectI(spip);
  chSysUnlockFromIsr();
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/


/**
 * DMX thread.
 */
__attribute__((noreturn))
 msg_t
 ledstripethread(void *arg)
{
  (void) arg;
  chRegSetThreadName("ledstripe");

  while (1)
  {
	  /* SPI slave selection and transmission start.*/
	      spiSelectI(&SPID2);
	      spiStartSendI(&SPID2, 1, ledstripe_buffer);

  }
}

void
LEDSTRIPEInit(void)
{
	int i;
	for(i=0; i < LEDSTRIPE_MAXIMUM; i++)
	{
		ledstripe_buffer[i] = 0;
	}

	/*
	   * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
	   * PB12 - NSS.
	   * PB13 - SCK.
	   * PB14 - MISO.
	   * PB15 - MOSI.
	   */
	  spiStart(&SPID2, &spi2cfg);
	  palSetPad(GPIOB, 12);
	  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
	                           PAL_STM32_OSPEED_HIGHEST);           /* NSS.     */
	  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
	                           PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
	  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              /* MISO.    */
	  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
	                           PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */


      chThdCreateStatic(wa_ledstripe, sizeof(wa_ledstripe), NORMALPRIO - 1, ledstripethread, NULL);
}

