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
 * SPI configuration structure.
 * Maximum speed (12MHz), CPHA=0, CPOL=0, 16bits frames, MSb transmitted first.
 * The slave select line is the pin GPIOA_SPI1NSS on the port GPIOA.
 */
static const SPIConfig spicfg = {
  spicb,
  /* HW dependent part.*/
  GPIOB,
  12,
  SPI_CR1_DFF
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

  /** The init procedure */
/* Command               RS      R/W     DB7     DB6     DB5     DB4     DB3     DB2     DB1     DB0     Hex     Remark
 * Function Set          0       0       0       0       1        1      1       0       1       0       $3A     8-Bit data length extension Bit RE=1; REV=0
 * Extended funcion set  0       0       0       0       0        0      1       0       0       1       $09     4 line display
 * Entry mode set        0       0       0       0       0        0      0       1       1       0       $06     bottom view
 * Bias setting          0       0       0       0       0        1      1       1       1       0       $1E     BS1=1
 * Function Set          0       0       0       0       1        1      1       0       0       1       $39     8-Bit data length extension Bit RE=0; IS=1
 * Internal OSC          0       0       0       0       0        1      1       0       1       1       $1B     BS0=1 -> Bias=1/6
 * Follower control      0       0       0       1       1        0      1       1       1       0       $6E     Devider on and set value
 * Power control         0       0       0       1       0        1      0       1       1       1       $57     Booster on and set contrast (DB1=C5, DB0=C4)
 * Contrast Set          0       0       0       1       1        1      0       0       1       0       $72     Set contrast (DB3-DB0=C3-C0)
 * Function Set          0       0       0       0       1        1      1       0       1       0       $38     8-Bit data length extension Bit RE=0; IS=0
 * Display On            0       0       0       0       0        0      1       1       1       1       $0F     Display on, cursor on, blink on */

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
   * PB12 - NSS.
   * PB13 - SCK.
   * PB14 - MISO.
   * PB15 - MOSI.
   */
  spiStart(&SPID2, &spicfg);
  palSetPad(GPIOB, 12);
  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
                           PAL_STM32_OSPEED_HIGHEST);           /* NSS.     */
  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              /* MISO.    */
  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */



  chThdCreateStatic(wa_ssd1803a, sizeof(wa_ssd1803a), NORMALPRIO - 1, ssd1803a_spi_thread, NULL);
}

