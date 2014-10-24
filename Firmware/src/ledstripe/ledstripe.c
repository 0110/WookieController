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
	/* Allocating two DMA2 streams for memory copy operations.*/
	if (dmaStreamAllocate(STM32_DMA2_STREAM6, 0, NULL, NULL))
		chSysHalt();

	dmaStreamSetPeripheral(STM32_DMA2_STREAM6,
}

