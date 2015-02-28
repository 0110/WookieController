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


/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define UART_PORT		(&SD6)

#define WLAN_UPRINT( ... ) chprintf((BaseSequentialStream *) UART_PORT, __VA_ARGS__); /**< UART print for WLAN module */

#define TEXTLINE_MAX_LENGTH     128


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
                     switch ((char)charbuf)
                     {
                       break;
                     default:
                       pText[read] = (char)charbuf;
                       break;
                     }
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

void esp8266_init(char *ssid, char *password)
{
	char textbuffer[TEXTLINE_MAX_LENGTH];
	int r=0;

	/* Set the baudrate to the default of 115200 */
	SerialConfig sc;
	/*FIXME: this should normally work: sc.sc_speed = 115200; */
	/* At the moment, we can skip this, as this is the same baudrate as the default one*/

	/*
	* Activates the serial driver 6
	* using default the configuration.
	* RX: PC7
	* TX: PC6
	*/
	sdStart(UART_PORT, /* FIXME &sc, hack: */ NULL);
	chThdSleepMilliseconds(500);

	WLAN_UPRINT("AT\r\n");
	usbcdc_print("Sending AT ...\r\n");
	r = readAll(textbuffer, TEXTLINE_MAX_LENGTH);
	usbcdc_print("Read %3d :  %s\r\n", r, textbuffer);

#if ESP8266_AUTO_RESET
	usbcdc_print("Reset board\r\n");
	/* Reset the WLAN board */
	WLAN_UPRINT("AT+RST \r\n");
	r = readAll(textbuffer, TEXTLINE_MAX_LENGTH);
	usbcdc_print("Read %3d :  %s\r\n", r, textbuffer);
#endif

	/* Set client mode: */
	WLAN_UPRINT("AT+CWMODE=1\r\n");
	r = readAll(textbuffer, TEXTLINE_MAX_LENGTH);
	usbcdc_print("Read %3d :  %s\r\n", r, textbuffer);

	/* Connect to WLAN */
	/*AT+CWJAP="SSID","secret" */
	WLAN_UPRINT("AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
        r = readAll(textbuffer, TEXTLINE_MAX_LENGTH);
        usbcdc_print("Read %3d :  %s\r\n", r, textbuffer);
}

void esp8266_printIP(BaseSequentialStream *chp)
{
	char textbuffer[TEXTLINE_MAX_LENGTH];
	int r=0;

	WLAN_UPRINT("AT+CIFSR\r\n");
	r = readAll(textbuffer, TEXTLINE_MAX_LENGTH);
	chprintf(chp, "Read %3d :  %s\r\n", r, textbuffer);
}
