#include "ch.h"
#include "hal.h"
#include "ledstripe/ledstripe.h"

/**
 * Stack area for the dmx thread.
 */
WORKING_AREA(wa_ledstripe, LEDSTRIPE_THREAD_STACK_SIZE);

static uint8_t ledstripe_buffer[LEDSTRIPE_MAXIMUM];

#define LENGTH_END_BUFFER	45

static uint8_t endbuffer[LENGTH_END_BUFFER];


#define CODE_BIT_0	0x07		/**< Bit representation on the SPI for a Logical ZERO / FALSE */
#define CODE_BIT_1	0x0F		/**< Bit representation on the SPI for a Logical ONE / TRUE */

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

static void spicb(SPIDriver *spip);

/*
 * SPI2 configuration structure.
 * Speed 800kHz
 * The slave select line is the pin 12 on the port GPIOA. (Not used)
 */
static const SPIConfig spi2cfg = {
  spicb,
  /* HW dependent part.*/
  0 /* No port for slave select */,
  0 /* no bit on port for slave select */,
  SPI_CR1_DFF | SPI_CR1_BR_1
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
	  /* First a reset */
	  spiStartSendI(&SPID2, LENGTH_END_BUFFER, endbuffer);
	  chThdSleep(2); /* give the scheduler some time */

	  spiStartSendI(&SPID2, 48, ledstripe_buffer);
	  chThdSleep(2); /* give the scheduler some time */

	  /* End with an reset */
	  spiStartSendI(&SPID2, LENGTH_END_BUFFER, endbuffer);

	  /* Wait some time, to make the scheduler running tasks with lower prio */
	  chThdSleep(MS2ST(1));
  }
}

void
ledstripe_init(void)
{
	int i;
	for(i=0; i < LEDSTRIPE_MAXIMUM; i++)
	{
		ledstripe_buffer[i] = CODE_BIT_1;
	}

	/* Initialize the end array */
	for(i=0; i < LENGTH_END_BUFFER; i++)
	{
		endbuffer[i] = 0x00;
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

