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

/******************************************************************************
 * DEFINES
 ******************************************************************************/

#define UART_PORT		(&SD6)

#define WLAN_UPRINT( ... ) chprintf((BaseSequentialStream *) UART_PORT, __VA_ARGS__); /**< UART print for WLAN module */

/******************************************************************************
 * GLOBAL VARIABLES for this module
 ******************************************************************************/

/******************************************************************************
 * LOCAL VARIABLES for this module
 ******************************************************************************/

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

void esp8266_init(char *ssid, char *password)
{
	/* Set the baudrate to the default of 115200 */
	SerialConfig sc;
	/*FIXME: this should normally work: sc.sc_speed = 115200; */
	/* At the moment, we can skip this, as this is the same baudrate as the default one*/

	/*
	* Activates the serial driver 2 and SDC driver 1 using default
	* configuration.
	* RX: PA3
	* TX: PA2
	*/
	sdStart(UART_PORT, /* FIXME &sc, hack: */ NULL);
	chThdSleepMilliseconds(50);

	WLAN_UPRINT("AT\r\n");
	WLAN_UPRINT("Hello World %d\r\n", "Arg1");

	/* Set client mode: */
	/*TODO AT+CWMODE=1*/

	/* Connect to WLAN */
	/*TODO AT+CWJAP="SSID","secret" */

}
