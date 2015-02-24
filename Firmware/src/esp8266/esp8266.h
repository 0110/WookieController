/*
 * esp8266.h
 *
 *  Created on: Feb 24, 2015
 *      Author: c3ma
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include "mcuconf.h"

#ifndef STM32_SERIAL_USE_USART6
#error "We need the second UART for the WLAN dongle"
#endif

/** @fn void esp8266_init(char *ssid, char *password)
 * @param[in] ssid     the ssid of the network to connect to
 * @param[in] password the password of ssid
 * @brief initialization of the UART Wifi module.
 */
void esp8266_init(char *ssid, char *password);

#endif /* ESP8266_H_ */
