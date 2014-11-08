#include "ch.h"
#include "hal.h"

#include "ledstripe/ledstripe.h"

/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/



void
ledstripe_init(void)
{
#if 0
	/* Allocating two DMA2 streams for memory copy operations.*/
	if (dmaStreamAllocate(STM32_DMA2_STREAM6, 0, (stm32_dmaisr_t) _dma_rotate_callback, NULL))
		chSysHalt();

	//FIXME dmaStreamSetPeripheral(STM32_DMA2_STREAM6,
#endif
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
