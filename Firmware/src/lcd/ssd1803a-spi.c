 /** @file ssd1803a-spi.c
 * @author ollo
 * @date created: 2014-11-17
 *
 * @brief Module LCD with a SSD1803A controller
 */

#include "ch.h"
#include "hal.h"
#include "lcd/ssd1803a-spi.h"

/**
 * Stack area for the led thread.
 */
WORKING_AREA(wa_ssd1803a, SSD1803A_THREAD_STACK_SIZE);


/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/

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
}

/**
 * DMX thread.
 */
__attribute__((noreturn))
 msg_t
 ssd1803a_spi_thread(void *arg)
{
  (void) arg;
  chRegSetThreadName("lcd-ssd1803a");


      /*FIXME while (1) */
      {

            /*FIXME SPI usage example: spiStartSendI(&SPID2, LENGTH_LEDBITS, SSD1803A_buffer); */
            chThdSleep(2); /* give the scheduler some time */
      }
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

void
ssd1803a_spi_init(void)
{
  /*
   * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
   * PB15 - MOSI.
   */
  spiStart(&SPID2, &spi2cfg);
  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              /* MISO.    */
  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */

  chThdCreateStatic(wa_ssd1803a, sizeof(wa_ssd1803a), NORMALPRIO - 1, ssd1803a_spi_thread, NULL);
}

