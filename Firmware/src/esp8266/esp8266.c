/*
 * esp8266.c
 *
 *  Created on: Feb 24, 2015
 *      Author: c3ma
 */

#ifndef ESP8266_C_
#define ESP8266_C_

#include "esp8266.h"

#include "ch.h"
#include "hal.h"

#define WLAN_UPRINT( ... ) chprintf((BaseSequentialStream *) &SD2, __VA_ARGS__); /**< Uart print */

void esp8266_init(char *ssid, char *password)
{
	/* Set the baudrate to the default of 115200 */
	SerialConfig sc;
	/*FIXME: this should normally work: sc.sc_speed = 115200; */
	/* At the moment, we can skip this, as this is the same baudrate as the default one*/

	/*
	* Activates the serial driver 2 and SDC driver 1 using default
	* configuration.
	* RX: PD6
	* TX: PD5
	*/
	sdStart(&SD2, /* FIXME &sc, hack: */ NULL);
	WLAN_UPRINT("AT\r\n");
}

#endif /* ESP8266_C_ */
