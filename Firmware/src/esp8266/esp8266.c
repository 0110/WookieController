/** @file esp8266.c
 * @brief Module to provide an WLAN connection via UART
 * @author Ollo
 *
 * @date 24.02.2015
 * @defgroup ESP6266
 *
 */

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "esp8266.h"

#include "ch.h"
#include "hal.h"

#include "usbcdc/usbcdc.h"

#include <string.h>

/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define UART_PORT               (&SD6)

#define WLAN_UPRINT( ... ) chprintf((BaseSequentialStream *) UART_PORT, __VA_ARGS__); /**< UART print for WLAN module */

#define TEXTLINE_MAX_LENGTH     128

#define CHECK_RESPONSE(RESPONSE)          r = readAll(textbuffer, TEXTLINE_MAX_LENGTH); \
                                if (strcmp(textbuffer, RESPONSE) != 0) \
                                { \
                                    int i; \
                                    usbcdc_print("Got '%s' instead of '%s' (%d Bytes read)\r\n", textbuffer, RESPONSE, r); \
                                    /* Print complete buffer as hex values */ \
                                    for(i=0; i<r;i++) \
                                    { \
                                        usbcdc_print("%2i %2X =? %2X\r\n", i, textbuffer[i], RESPONSE[i]); \
                                    } \
                                    return RET_COMMUNICATION_ERR; \
                                }

#define WLAN_CMD_GETIP  "AT+CIFSR\r\n"

/******************************************************************************
 * GLOBAL VARIABLES for this module
 ******************************************************************************/

/******************************************************************************
 * LOCAL VARIABLES for this module
 ******************************************************************************/

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

/** @fn static int readLine(char *pData)
 * @brief Reads a line
 * @param[in|out]       pText           space for the text, that is read (and the read result)
 * @param[in]           bufferLeng      The maximum of characters, that could be read.
 * @return amount of read characters (or -1 on errors)
 */
static int readAll(char *pText, int bufferLeng)
{
  EventListener elGPSdata;
  flagsmask_t flags;
  int i, read=0;
  int finishFlag=0;
  chEvtRegisterMask((EventSource *)chnGetEventSource(UART_PORT), &elGPSdata, EVENT_MASK(1));

  /* Check faulty input parameter */
   if (bufferLeng <= 0)
   {
     return -1;
   }

   for(i=0;i < bufferLeng && !finishFlag; i++)
   {
       /* Found serial reading here:
        * http://forum.chibios.org/phpbb/viewtopic.php?p=12262&sid=5f8c68257a2cd5be83790ce6f7e1282d#p12262 */
       chEvtWaitOneTimeout(EVENT_MASK(1), MS2ST(10));
       chSysLock();
       flags = chEvtGetAndClearFlags(&elGPSdata);
       chSysUnlock();

       if (flags & CHN_INPUT_AVAILABLE)
       {
           msg_t charbuf;
           do
           {
                 charbuf = chnGetTimeout(UART_PORT, TIME_IMMEDIATE);
                 if ( charbuf != Q_TIMEOUT )
                 {
                     pText[read] = (char)charbuf;
                     read++;
                 }
           }
           while (charbuf != Q_TIMEOUT);
       }
   }

   /* do not write after YOUR memory */
   if (read >= bufferLeng)
   {
           read = bufferLeng - 1;
   }

   chEvtUnregister((EventSource *)chnGetEventSource(UART_PORT), &elGPSdata);

   /* also make a zero to mark the end of the text */
   pText[read] = '\0';

   return read;
}

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

esp8266_ret_t esp8266_init()
{
  SerialConfig sc = {
          9600,
        0,
        USART_CR2_STOP1_BITS | USART_CR2_LINEN,
        0
      };

  /*
  * Activates the serial driver 6
  * using default the configuration.
  * RX: PC7
  * TX: PC6
  */
  sdStop(UART_PORT);
  sdStart(UART_PORT, &sc);

  return ESP8266_RET_OK;
}

esp8266_ret_t esp8266_UDPprintln(const char *text, ...)
{
  char textbuffer[TEXTLINE_MAX_LENGTH];
  int r=0;
  va_list ap;
  va_start(ap, text);
  WLAN_UPRINT("s:send(\"");
  usbcdc_chvprintf((BaseSequentialStream *) UART_PORT, text, ap);
  WLAN_UPRINT("\\n\")\r");
  va_end(ap);
  r = readAll(textbuffer, TEXTLINE_MAX_LENGTH);
  usbcdc_print("Read %3d :  %s\r\n", r, textbuffer);

  return ESP8266_RET_OK;
}

esp8266_ret_t esp8266_debugcmd(const char *s)
{
  char textbuffer[TEXTLINE_MAX_LENGTH];
  int r=0;

  usbcdc_print("Sending '%s'\r\n", s);
  WLAN_UPRINT(s);

  r = readAll(textbuffer, TEXTLINE_MAX_LENGTH);
  usbcdc_print("Read %3d :  %s\r\n", r, textbuffer);

  return ESP8266_RET_OK;
}
