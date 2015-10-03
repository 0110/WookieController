/** @file ssd1803a-spi.c
 * @author ollo
 * @date created: 2014-11-17
 *
 * @brief Module LCD with a SSD1803A controller
 */

#include "lcd/ea_dogl128x-6.h"
#include "lcd/spi-implement.h"

#include "ch.h"

#define SPI_TELEGRAM_LENGTH	1       /**< Amount of bytes for one package of information to the LCD */

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

static void
sendViaSPI(uint8_t data)
{
  uint8_t transferStore[SPI_TELEGRAM_LENGTH];
  uint8_t tmp;
  int bit;

  /*Set the first byte (lower data) */
  //transferStore[1] = 0x00;
  //transferStore[1] |= (data & 0x0F) << 4;
  //SWAP_NIPPLE(tmp, 7, 4, transferStore[1])

  /* Set the second byte (upper data) */
  //transferStore[2] = 0x00;
  //transferStore[2] |= (data & 0xF0);
  //SWAP_NIPPLE(tmp, 7, 4, transferStore[2])

  /* Simply copy the data to transfer into the temporary storage */
  transferStore[0] = data;
  SWAP_NIPPLE(tmp, 7, 0, transferStore[0]);

  spi_implement_send(SPI_TELEGRAM_LENGTH, transferStore);
  chThdSleep(MS2ST(5)); /* give the scheduler and the LCD some time */
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

void
eadogl_init(void)
{

  if (spi_implement_init() != SPI_IMPL_RET_OK)
    {
      gRunning = FALSE;
      return;
    }

  /** The init procedure */
  sendViaSPI(0x40); /* Display start line 0 */
  sendViaSPI(0xA1); /* ADC reverse */
  sendViaSPI(0xC0); /* Normal COM0~COM63 */
  sendViaSPI(0xA6); /* Display normal */
  sendViaSPI(0xA2); /* Set las 1/9 (Duty 1/65) */
  sendViaSPI(0x2F); /* Booster, Regulator and Follower on */
  sendViaSPI(0xF8); /* Set internal Booster to 4x */
  sendViaSPI(0x00); /* Set internal Booster to 4x (Part 2) */
  sendViaSPI(0x27); /* Contrast set (Part 1 / 3) */
  sendViaSPI(0x81); /* Contrast set (Part 23/ 3) */
  sendViaSPI(0x00); /* Contrast set (Part 3 / 3) */
  sendViaSPI(0xAC); /* No Indicator (Part 1 / 2) */
  sendViaSPI(0x00); /* No Indicator (Part 2 / 2) */
  sendViaSPI(0xAF); /* Display on */

  gRunning = TRUE;
}

EADOGL_RET
eadogl_sendText(char *s, int textLength)
{
  int i;

  if (gRunning != TRUE)
  {
    return EADOGL_RET_NOTINITIALIZED;
  }

  //TODO sendViaSPI(0x01); /* Clear Display */
  //TODO sendViaSPI(0x02); /* Return home */
  chThdSleep(MS2ST(5)); /* give the LCD some time */

  /* Converting the data according to ROM A */
  for (i = 0; i < textLength; i++)
  {
      if (s[i] >= 'A' && s[i] <= 'Z')
      {
          sendViaSPI(/*0, 1,*/ 65 + (s[i] - 'A'));
      }
      else if (s[i] >= 'a' && s[i] <= 'z')
      {
          sendViaSPI(/*0, 1,*/ 97 + (s[i] - 'a'));
      }
      else if (s[i] >= '%' && s[i] <= '?')
      {
          /* % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? */
          sendViaSPI(/*0, 1,*/ 37 + (s[i] - '%'));
      }
      else if (s[i] == 0xC3)
      {
          switch (s[i + 1])
          {
          case 0xA4: // ä
            sendViaSPI(/*0, 1,*/ 0xe4);
            break;
          case 0xB6: // ö
            sendViaSPI(/*0, 1,*/ 0xf6);
            break;
          case 0xBC: // ü
            sendViaSPI(/*0, 1,*/ 0xfc);
            break;
          case 0x84: // Ä
            sendViaSPI(/*0, 1,*/ 0xc4);
            break;
          case 0x96:  // Ö
            sendViaSPI(/*0, 1,*/ 0xd6);
            break;
          case 0x9C:  // Ü
            sendViaSPI(/*0, 1,*/ 0xdc);
            break;
          case 0x9F:  // ß
            sendViaSPI(/*0, 1,*/ 0xdf);
            break;
          default:
            sendViaSPI(/*0, 1,*/ 0x15); // Error Char
            break;
          }
          i++;
      }
      else if (s[i] == 0xC2)
      {
          switch (s[i + 1])
          {
          default:
            sendViaSPI(/*0, 1,*/ 0x15); // Error Char
            break;
          }
          i++;
      }
      else
      {
          /*FIXME simple copy the value, as we have no idea how to convert it (or currently not implemented) */
          sendViaSPI(/*0, 1,*/ s[i]);
      }
      chThdSleep(MS2ST(5)); /* give the LCD some time */
  }
  return EADOGL_RET_OK;
}
