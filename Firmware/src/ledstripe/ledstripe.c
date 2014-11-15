/** @file ledstripe.c
 * @author ollo
 * @date created: 2014-11-08
 *
 * @brief Module (with an own thread) to control the WS2812 LED strips
 */

#include "ch.h"
#include "hal.h"
#include "ledstripe/ledstripe.h"

/**
 * Stack area for the led thread.
 */
WORKING_AREA(wa_ledstripe, LEDSTRIPE_THREAD_STACK_SIZE);

#define LENGTH_END_BUFFER	45      /**< Bits (zeroed), to simulate the end of a communication aka Reset */
#define BITS_IN_BYTE            8

uint8_t ledstripe_fb[LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED];

static uint8_t spi_buffer[LENGTH_LEDBITS]; /**< Converted Bits to be sent via SPI */
static uint8_t endbuffer[LENGTH_END_BUFFER]; /**< buffer containing zeros to simulate a reset signal */

static int mLedstripeIndex = 0;

#define CODE_BIT_0	0x03		/**< Bit representation on the SPI for a Logical ZERO */
#define CODE_BIT_1	0x0F		/**< Bit representation on the SPI for a Logical ONE  */

#define	 SPI_OUTPUT_PIN		15	/**< Output pin, used for the SPI */

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

/**
 * @fn static int updateBufferContent( void )
 *
 * Update the buffer, sent via SPI
 *
 * @return      TRUE    if there is still new data
 *              FALSE   the end was found
 */
static int updateBufferContent(void);

static void spicb(SPIDriver *spip);

/******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************/

/*
 * SPI2 configuration structure.
 * Speed 800kHz
 */
static const SPIConfig spi2cfg = { spicb,
/* HW dependent part.*/
0,
0,
SPI_CR1_BR_1
};

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

/*
 * SPI end transfer callback.
 */
static void spicb(SPIDriver *spip) {
	/* On transfer end just releases the slave select line.*/
	palSetPad(GPIOB, 12);
}

/**
 * DMX thread.
 */
__attribute__((noreturn))
  msg_t ledstripethread(void *arg) {
	(void) arg;
	chRegSetThreadName("ledstripe");

	palSetPadMode(GPIOB, 12, PAL_STM32_MODE_OUTPUT | PAL_STM32_OTYPE_PUSHPULL |PAL_STM32_PUDR_PULLUP);

	while ( TRUE ) {
		while (!updateBufferContent()) {

			palSetPad(GPIOB, 12);
			palSetPadMode(GPIOB, 12, PAL_STM32_MODE_INPUT | PAL_STM32_PUDR_FLOATING);

			spiStartSendI(&SPID2, LENGTH_LEDBITS, spi_buffer);

			/* Send Reset. */
			palSetPadMode(GPIOB, 12, PAL_STM32_MODE_OUTPUT | PAL_STM32_OTYPE_PUSHPULL |PAL_STM32_PUDR_PULLUP);
			palClearPad(GPIOB, 12);

			/* Give the Scheduler of Chibios some time */
			chThdSleep(US2ST(1));
		}

		chThdSleep(1); /* give the scheduler some time */
		/* End with an reset */
		spiStartSendI(&SPID2, LENGTH_END_BUFFER, endbuffer); /*TODO test here a long sleep ?!? */

		/* Wait some time, to make the scheduler running tasks with lower prio */
		chThdSleep(MS2ST(50));
		palTogglePad(GPIOD, GPIOD_LED5); /* Red */
	}
}

static int updateBufferContent() {
	int i, bitIndex;
	uint8_t mask;

	/*FIXME normally there is an index needed, where in the big LED array to look */

	/* Update the complete Buffer, that is sent via SPI */
	for (i = 0; i < LENGTH_LED_SEND * LEDSTRIPE_COLORS_PER_LED; i++) {
		for (bitIndex = 1; bitIndex <= BITS_IN_BYTE; bitIndex++) {
			mask = ~(1 << bitIndex);
			if (ledstripe_fb[mLedstripeIndex] & mask) {
				spi_buffer[(i * BITS_IN_BYTE) + bitIndex] = CODE_BIT_1;
			} else {
				spi_buffer[(i * BITS_IN_BYTE) + bitIndex] = CODE_BIT_0;
			}
		}

		mLedstripeIndex++;

		if (mLedstripeIndex > (LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED)) {
			mLedstripeIndex = 0;
			return TRUE;
		}

	}

	return FALSE;
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

void ledstripe_init(void) {
	int i;
	for (i = 0; i < LENGTH_LEDBITS; i++) {
		spi_buffer[i] = CODE_BIT_0;
	}

	/* Initialize the end array */
	for (i = 0; i < LENGTH_END_BUFFER; i++) {
		endbuffer[i] = 0x00;
	}

	/* Clear the LEDs */
	for (i = 0; i < LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED; i ++) {
		ledstripe_fb[i] = 0;
	}

	/*
	 * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
	 * PB15 - MOSI.
	 */
	spiStart(&SPID2, &spi2cfg);
	palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5)); /* MISO.    */
	palSetPadMode(GPIOB, SPI_OUTPUT_PIN, PAL_MODE_ALTERNATE(5) |
			PAL_STM32_OSPEED_HIGHEST); /* MOSI.    */


	chThdCreateStatic(wa_ledstripe, sizeof(wa_ledstripe), NORMALPRIO - 1,
			ledstripethread, NULL);
}

