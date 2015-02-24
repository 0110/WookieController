/** @file esp8266.h
 * @brief Module to provide an WLAN connection via UART
 * @page ESP8266 UART WLAN
 * @author Ollo
 * @date 24.02.2015
 *
 * Source of commands: http://defcon-cc.dyndns.org/wiki/ESP8266#Software
 *
 * @defgroup ESP6266 Output library (USB UART)
 * @{
 *
 */

#ifndef ESP8266_H_
#define ESP8266_H_

/******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include "mcuconf.h"

#ifndef STM32_SERIAL_USE_USART6
#error "We need the second UART for the WLAN dongle"
#endif

/******************************************************************************
 * DEFINES
 ******************************************************************************/

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

/** @fn void esp8266_init(char *ssid, char *password)
 * @param[in] ssid     the ssid of the network to connect to
 * @param[in] password the password of ssid
 * @brief initialization of the UART Wifi module.
 */
void esp8266_init(char *ssid, char *password);

/*@}*/

#endif /* ESP8266_H_ */
