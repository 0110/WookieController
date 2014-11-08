#include "ch.h"
#include "hal.h"

#include "ledstripe/ledstripe.h"

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/


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
0
};

void
ledstripe_init(void)
{

#if 0
	/* Allocating two DMA2 streams for memory copy operations.*/
	if (dmaStreamAllocate(STM32_DMA2_STREAM6, 0, (stm32_dmaisr_t) _dma_rotate_callback, NULL))
		chSysHalt();

	//FIXME dmaStreamSetPeripheral(STM32_DMA2_STREAM6,
#endif

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

#if 0

/* SPI transfer callback, used when buffers need to be rotated */
static void _dma_rotate_callback( SPIDriver * driver , u32 flags )
{
	/*TODO do some magic */
}
#endif
