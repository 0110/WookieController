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

#define		BIT_LOW 	30	/*FIXME adapt values (in micro seconds) */
#define		BIT_HIGH 	50	/*FIXME adapt values (in micro seconds) */

#define		BIT_LENGTH	100	/* micro seconds */

#define TEST_PATTERN_LENGTH		8

#define	 set_pin_high()	palSetPad(GPIOD, GPIOD_LED5) 	/* Red.  */
#define set_pin_low()	palClearPad(GPIOD, GPIOD_LED5) 	/* Red.  */

static uint8_t testpattern[TEST_PATTERN_LENGTH] = { BIT_HIGH, BIT_HIGH, BIT_HIGH, BIT_HIGH, BIT_LOW, BIT_LOW, BIT_LOW, BIT_LOW };

static int byteOffset=0;	/* is used with the doulbed size of "testpattern" to simulate the value to fall */

static void gpt_adc_trigger(GPTDriver *gpt_ptr)
{
	/* Update next value */
	if ((byteOffset / 2) >= TEST_PATTERN_LENGTH)
	{
		/*FIXME normal, the next must be copied, now simply restart */
		byteOffset = 0;
	}
	else
	{
		byteOffset++;
	}

	if (byteOffset % 2 != 0)
	{
		set_pin_low();

		// restart timer
		gptStartOneShot(gpt_ptr, BIT_LENGTH - testpattern[byteOffset / 2] );
	}
	else	/* go to the next field in the array */
	{
		set_pin_high();
		gptStartOneShot(gpt_ptr, testpattern[byteOffset / 2] );
	}

}

/*
 * Configure a GPT object
 */
static GPTConfig gpt_adc_config =
{
     1000000,  // timer clock: 1Mhz
     gpt_adc_trigger  // Timer callback function
};

void ledstripe_init() {

	/* init the required hardware */

	/*
	  * Start the GPT timer
	  * Timer is clocked at 1Mhz (1us). Timer triggers at 76us and calls the callback function
	  */
	gptStart(&GPTD1, &gpt_adc_config);
	set_pin_high();
	gptStartOneShot(&GPTD1, testpattern[byteOffset] /* begin with the first time */);

}

