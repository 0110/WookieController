 /** @file ledstripe.c
 * @author ollo
 * @date created: 2014-11-08
 *
 * @brief Module (with an own thread) to control the WSxyz?!? LED strips
 */

#include "ch.h"
#include "hal.h"
#include "ledstripe/ledstripe.h"

/**
 * Stack area for the led thread.
 */
WORKING_AREA(wa_ledstripe, LEDSTRIPE_THREAD_STACK_SIZE);

#define LENGTH_LEDBITS		240	/**< Space for 10 LEDs */
#define LENGTH_LED_SEND         10      /**< Amount of LEDs that are sent at one block via SPI */
#define LENGTH_END_BUFFER	45      /**< Bits (zeroed), to simulate the end of a communication aka Reset */
#define BITS_IN_BYTE            8

static uint8_t ledStates[LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED];

static uint8_t  ledstripe_buffer[LENGTH_LEDBITS];       /**< Converted Bits to be sent via SPI */
static uint8_t  endbuffer[LENGTH_END_BUFFER];           /**< buffer containing zeros to simulate a reset signal */

#define CODE_BIT_0	0x03		/**< Bit representation on the SPI for a Logical ZERO */
#define CODE_BIT_1	0x0F		/**< Bit representation on the SPI for a Logical ONE  */

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

static void updateBufferContent( void );

static void spicb(SPIDriver *spip);

/******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************/

/*
 * SPI2 configuration structure.
 * Speed 800kHz
 */
static const SPIConfig spi2cfg = {
  spicb,
  /* HW dependent part.*/
  0 /* No port for slave select */,
  0 /* no bit on port for slave select */,
  SPI_CR1_BR_1
};

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

/*
 * SPI end transfer callback.
 */
static void spicb(SPIDriver *spip)
{
  (void) spip;
  /*FIXME get new data to display */
  palClearPad(GPIOB, 15);

  palTogglePad(GPIOD, GPIOD_LED5); /* Red */
}

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
	  /* No data transmission, only keep the signal down to zero */
	  spiStartSendI(&SPID2, LENGTH_LEDBITS, ledstripe_buffer);
	  chThdSleep(2); /* give the scheduler some time */

	  /* End with an reset */
	  spiStartSendI(&SPID2, LENGTH_END_BUFFER, endbuffer); /*TODO test here a long sleep ?!? */

	  /* Wait some time, to make the scheduler running tasks with lower prio */
	  palClearPad(GPIOB, 15);
	  chThdSleep(MS2ST(50));

	  updateBufferContent();
  }
}

static void updateBufferContent()
{
	int i, bitIndex;
	uint8_t mask;

	/*FIXME normally there is an index needed, where in the big LED array to look */

	/* Update the complete Buffer, that is sent via SPI */
	for(i=0; i < LENGTH_LED_SEND; i++)
        {
	    for(bitIndex = 1; bitIndex <= BITS_IN_BYTE; bitIndex++)
            {
	        mask = ~(1 << bitIndex);
	        if (ledStates[i] & mask)
                {
	            ledstripe_buffer[(i * BITS_IN_BYTE) + bitIndex] = CODE_BIT_1;
                }
	        else
                {
	            ledstripe_buffer[(i * BITS_IN_BYTE) + bitIndex] = CODE_BIT_0;
                }
            }
        }
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

void
ledstripe_init(void)
{
	int i;
	for(i=0; i < LENGTH_LEDBITS; i++)
	{
		ledstripe_buffer[i] = CODE_BIT_0;
	}

	/* Initialize the end array */
	for(i=0; i < LENGTH_END_BUFFER; i++)
	{
		endbuffer[i] = 0x00;
	}

	/* Tiny example with Blue LEDs */
	for(i=2; i < LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED; i+=3)
	{
	    ledStates[i] = ((i / 3) * 10 % 255);
	}

	/*
         * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
         * PB15 - MOSI.
         */
        spiStart(&SPID2, &spi2cfg);
        palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              /* MISO.    */
        palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
                                 PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */

      chThdCreateStatic(wa_ledstripe, sizeof(wa_ledstripe), NORMALPRIO - 1, ledstripethread, NULL);
}

