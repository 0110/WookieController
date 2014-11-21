 /** @file ssd1803a-spi.c
 * @author ollo
 * @date created: 2014-11-17
 *
 * @brief Module LCD with a SSD1803A controller
 */

#include "ch.h"
#include "hal.h"
#include "lcd/ssd1803a-spi.h"

#define SPI_TELEGRAM_LENGTH     3       /**< Amount of bytes for one package of information to the LCD */

#define SWAP_NIPPLE(TMP,START, STOP, DATA)		TMP = 0; \
  for(bit=START; bit >= STOP; bit--) \
  { \
    if (DATA & (1 << bit)) { TMP |= (1 << (START - bit + STOP)); } \
  } \
  DATA = TMP;

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
 * The slave select line is the pin GPIOA_SPI1NSS on the port GPIOA.
 */
static const SPIConfig spicfg = {
  spicb,
  /* HW dependent part.*/
  GPIOB,
  12,
  SPI_CR1_BR_2
};

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

static void sendViaSPI(int RW, int RS, uint8_t data)
{
  uint8_t transferStore[SPI_TELEGRAM_LENGTH];
  int bit;
  uint8_t tmp;

  /* Fill the start byte */
  transferStore[0] = 0xF8; /* Set the first 5 bits */
  transferStore[0] |= RW ? 0x04 : 0x00; /* the 6th bit defines R/W */
  transferStore[0] |= RS ? 0x02 : 0x00; /* the 7th bit defines RS */
  /* The 8th bit is always zero */

  /*Set the first byte (lower data) */
  transferStore[1] = 0x00;
  transferStore[1] |= (data & 0x0F) << 4;
  SWAP_NIPPLE(tmp, 7, 4, transferStore[1])

  /* Set the second byte (upper data) */
  transferStore[2] = 0x00;
  transferStore[2] |= (data & 0xF0);
  SWAP_NIPPLE(tmp, 7, 4, transferStore[2])

  spiStartSendI(&SPID2, SPI_TELEGRAM_LENGTH, transferStore);
  chThdSleep(MS2ST(50)); /* give the scheduler some time */
}

/*
 * SPI end transfer callback.
 */
static void spicb(SPIDriver *spip)
{
  (void) spip;
}

/** @fn msg_t ssd1803a_spi_thread(void *arg)
 * SPI update thread.
 */
msg_t ssd1803a_spi_thread(void *arg)
{
  (void) arg;
  chRegSetThreadName("lcd-ssd1803a");

  /** The init procedure */
/* Command               RS      R/W     DB7     DB6     DB5     DB4     DB3     DB2     DB1     DB0     Hex     Remark
 * Function Set          0       0       0       0       1        1      1       0       1       0       $3A     8-Bit data length extension Bit RE=1; REV=0*/
 sendViaSPI(0,0,0x3A);
 /* Extended funcion set  0       0       0       0       0        0      1       0       0       1       $09     4 line display*/
 sendViaSPI(0,0,0x09);
 /* Entry mode set        0       0       0       0       0        0      0       1       1       0       $06     bottom view*/
 sendViaSPI(0,0,0x06);
 /* Bias setting          0       0       0       0       0        1      1       1       1       0       $1E     BS1=1*/
 sendViaSPI(0,0,0x1E);
 /* Function Set          0       0       0       0       1        1      1       0       0       1       $39     8-Bit data length extension Bit RE=0; IS=1*/
 sendViaSPI(0,0,0x39);
 /* Internal OSC          0       0       0       0       0        1      1       0       1       1       $1B     BS0=1 -> Bias=1/6*/
 sendViaSPI(0,0,0x1B);
 /* Follower control      0       0       0       1       1        0      1       1       1       0       $6E     Devider on and set value*/
 sendViaSPI(0,0,0x6E);
 /* Power control         0       0       0       1       0        1      0       1       1       1       $57     Booster on and set contrast (DB1=C5, DB0=C4)*/
 sendViaSPI(0,0,0x57);
 /* Contrast Set          0       0       0       1       1        1      0       0       1       0       $72     Set contrast (DB3-DB0=C3-C0)*/
 sendViaSPI(0,0,0x72);
 /* Function Set          0       0       0       0       1        1      1       0       1       0       $38     8-Bit data length extension Bit RE=0; IS=0*/
 sendViaSPI(0,0,0x38);
 /* Display On            0       0       0       0       0        0      1       1       1       1       $0F     Display on, cursor on, blink on */
 sendViaSPI(0,0,0x0F);

 chThdSleep(MS2ST(50));
 sendViaSPI(0,0,0x01); /* Clear Display */
 sendViaSPI(0,0,0x02); /* Return home */
 sendViaSPI(0,1,'C');
 sendViaSPI(0,1,'3');
 sendViaSPI(0,1,'M');
 sendViaSPI(0,1,'A');
 sendViaSPI(0,1,'\r');
 sendViaSPI(0,1,'\n');
 sendViaSPI(0,1,'H');
 sendViaSPI(0,1,'e');

#if 0
 /* Set Character table */
 sendViaSPI(0, 0, 0x3A);
 sendViaSPI(0, 1, 0x72);
 sendViaSPI(0, 1, 0x00);
 sendViaSPI(0, 0, 0x38);

 sendViaSPI(0, 0, 'H');
 sendViaSPI(0, 0, 'e');
 sendViaSPI(0, 0, 'l');
 sendViaSPI(0, 0, 'l');
 sendViaSPI(0, 0, 'o');
 sendViaSPI(0, 0, '\0');
 /* A tiny test */
 sendViaSPI(0,0, '\r');
 sendViaSPI(0,0, '\n');
#endif

 while ( TRUE )
 {

   /*FIXME SPI usage example: spiStartSendI(&SPID2, LENGTH_LEDBITS, SSD1803A_buffer); */
	 chThdSleep(MS2ST(100)); /* give the scheduler some time */
 }
 return RDY_OK;
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

