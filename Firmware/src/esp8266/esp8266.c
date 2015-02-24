/*
 * esp8266.c
 *
 *  Created on: Feb 24, 2015
 *      Author: c3ma
 */

#include "esp8266.h"

#include "ch.h"
#include "hal.h"

#define UART_PORT		(&SD6)

#define WLAN_UPRINT( ... ) chprintf((BaseSequentialStream *) UART_PORT, __VA_ARGS__); /**< UART print for WLAN module */

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
