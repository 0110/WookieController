/**
 * @file ledstrip.c
 * @author Manu
 * @author Ollo
 *
 * creation date 2014-11-08
 *
 * Inspired by: https://github.com/omriiluz/WS2812B-LED-Driver-ChibiOS/blob/master/LEDDriver.c
 *
 */

#include "ledstripe/ledstripe.h"

#define		BIT_LOW 	30	/*FIXME adapt values */
#define		BIT_HIGH 	50	/*FIXME adapt values */

#define TEST_PATTERN_LENGTH		8

static uint8_t testpattern[TEST_PATTERN_LENGTH] = { BIT_HIGH, BIT_HIGH, BIT_HIGH, BIT_HIGH, BIT_LOW, BIT_LOW, BIT_LOW, BIT_LOW };

static void gpt_adc_trigger(GPTDriver *gpt_ptr)
{
	(void) gpt_ptr;
    palTogglePad(GPIOB, GPIOD_LED4);
}

static uint8_t* dma_source;

/*
 * Configure a GPT object
 */
static GPTConfig gpt_adc_config =
{
     1000000,  // timer clock: 1Mhz
     gpt_adc_trigger  // Timer callback function
};

void ledstripe_init() {

	int i;
	/* alloc some memory and fill it with the test pattern */
	dma_source = chHeapAlloc(NULL, TEST_PATTERN_LENGTH);
	for(i=0; i< TEST_PATTERN_LENGTH; i++)
	{
		dma_source[i] = testpattern[i];
	}

	/* init the required hardware */

	 /*
	  * Start the GPT timer
	  * Timer is clocked at 1Mhz (1us). Timer triggers at 76us and calls the callback function
	  */
	gptStart(&GPTD2, &gpt_adc_config);
	gptStartContinuous(&GPTD3, 76);

}

