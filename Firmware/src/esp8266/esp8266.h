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
#include "ch.h"

#ifndef STM32_SERIAL_USE_USART6
#error "We need the second UART for the WLAN dongle"
#endif

/******************************************************************************
 * DEFINES
 ******************************************************************************/

/******************************************************************************
* TYPE DEFINITIONS
******************************************************************************/
/** @enum esp8266_ret_t
* @brief Status
* return value for all global functions in the esp8266 module
*/
typedef enum {
  RET_OK, /**< OK, no Error occurred */
  RET_ERROR, /**< Global not further specified error! */
  RET_COMMUNICATION_ERR /**< Unexpected response from ESP8266 module */
} esp8266_ret_t;

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

/** @fn void esp8266_init(char *ssid, char *password)
 * @param[in] ssid     the ssid of the network to connect to
 * @param[in] password the password of ssid
 * @brief initialization of the UART Wifi module.
 * If the two paramater ssid and password are not set, the last used WiFi is used again.
 */
esp8266_ret_t esp8266_init(char *ssid, char *password);

/** @fn void esp8266_printIP(BaseSequentialStream *chp)
 * @param chp	Stream to print the information on
 * @brief Print the information about the own IP address.
 */
void esp8266_printIP(BaseSequentialStream *chp);

/** @fn esp8266_ret_t esp8266_openTCPServer(int port)
 * @param port  The port number where the TCP socket should listen on
 * @brief Starts an TCP Server
 */
esp8266_ret_t esp8266_openTCPServer(int port);

/** @fn esp8266_ret_t esp8266_closeTCPServer()
 * @brief Close the TCP Server
 */
esp8266_ret_t esp8266_closeTCPServer(void);

/** @fn esp8266_ret_t esp8266_printTCP(const char *s, ...)
 * @brief prints the given string on the TCP socket
 * @param[in]   s       string to print including formating definition
 * @param[in]   ...     All values for the formating options
 */
esp8266_ret_t esp8266_printTCP(const char *s, ...);

/*@}*/

#endif /* ESP8266_H_ */
