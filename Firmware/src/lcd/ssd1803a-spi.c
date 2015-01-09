 /** @file ssd1803a-spi.c
 * @author ollo
 * @date created: 2014-11-17
 *
 * @brief Module LCD with a SSD1803A controller
 */

#include "lcd/ssd1803a-spi.h"
#include "lcd/spi-implement.h"

#include "ch.h"

#define SPI_TELEGRAM_LENGTH     3       /**< Amount of bytes for one package of information to the LCD */

#define SWAP_NIPPLE(TMP,START, STOP, DATA)		TMP = 0; \
  for(bit=START; bit >= STOP; bit--) \
  { \
    if (DATA & (1 << bit)) { TMP |= (1 << (START - bit + STOP)); } \
  } \
  DATA = TMP;


/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/



/******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************/
static int gRunning = FALSE;

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

  spi_implement_send(SPI_TELEGRAM_LENGTH, transferStore);
  chThdSleep(MS2ST(2)); /* give the scheduler some time */
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

void
ssd1803a_spi_init(void)
{

  if (spi_implement_init() != SPI_IMPL_RET_OK)
  {
      gRunning = FALSE;
      return;
  }

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
 /* Display On            0       0       0       0       0        0      1       1       0       0       $0F     Display on, cursor off, blink off */
 sendViaSPI(0,0,0x0C);


  gRunning = TRUE;
}

SSD1803A_RET ssd1803a_spi_sendText(char *s, int textLength)
{
  int i;
  char tempBuffer[textLength];

  if (gRunning != TRUE)
  {
    return SSD1803A_RET_NOTINITIALIZED;
  }

  /* Converting the data according to ROM A */
  for(i=0; i < textLength; i++)
  {
      if (s[i] >= 'A' && s[i] <= 'Z')
      {
        tempBuffer[i] = 65 + (s[i] - 'A');
      }
      else if (s[i] >= 'a' && s[i] <= 'z')
      {
        tempBuffer[i] = 97 + (s[i] - 'a');
      }
      else if (s[i] >= '%' && s[i] <= '?')
      {
        /* % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? */
        tempBuffer[i] = 37 + (s[i] - '%');
      }
      else
      {
        /*FIXME simple copy the value, as we have no idea how to convert it (or currently not implemented) */
        tempBuffer[i] = s[i];
      }
  }

  sendViaSPI(0,0,0x01); /* Clear Display */
  sendViaSPI(0,0,0x02); /* Return home */
  for(i=0; i < textLength; i++)
  {
       sendViaSPI(0,1,tempBuffer[i]);
  }
  return SSD1803A_RET_OK;
}
