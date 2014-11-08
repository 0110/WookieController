#include "ch.h"
#include "hal.h"

#include "ledstripe/ledstripe.h"

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

static void _dma_rotate_callback( SPIDriver * driver , u32 flags );

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

static PWMConfig pwmcfg = {
10000, /* 10Khz PWM clock frequency */
1000, /* PWM period 1 second */
NULL, /* No callback */
/* Only channel 4 enabled */
{
{PWM_OUTPUT_DISABLED, NULL},
{PWM_OUTPUT_DISABLED, NULL},
{PWM_OUTPUT_DISABLED, NULL},
{PWM_OUTPUT_ACTIVE_HIGH, NULL},
},
0,
0
};

/**
 * @fn void ledstripe_init(void)
 * Using GPIOC_SD_D1 als Output-PIN.
 * This SHOULD be changed, as this one is potential used to access the SD-card!
 * FIXME another one!
 */
void ledstripe_init(void)
{

	/* Allocating two DMA2 streams for memory copy operations.*/
	if (dmaStreamAllocate(STM32_DMA1_STREAM2, 0, (stm32_dmaisr_t) _dma_rotate_callback, NULL))
		chSysHalt();

	//FIXME dmaStreamSetPeripheral(STM32_DMA2_STREAM6,


	/* Enables PWM output (of TIM4, channel 4) on blue LED connected to PD15 */
	palSetPadMode(GPIOD, GPIOD_LED6, PAL_MODE_ALTERNATE(2));

	pwmStart(&PWMD4, &pwmcfg);
	pwmEnableChannel(&PWMD4, 3, 500); /* 500 clock pulse number to change the LED (must fit to the 1000 see pwmconfig) */

	while (TRUE) {
		chThdSleepMilliseconds(5000);
	}

}

/******************************************************************************
 * LOCAL Functions
 ******************************************************************************/

/* SPI transfer callback, used when buffers need to be rotated */
static void _dma_rotate_callback( SPIDriver * driver , u32 flags )
{
	/*TODO do some magic: kommt später */
	(void) driver; /* not using it atm */
	(void) flags;  /* not using it atm */
}

