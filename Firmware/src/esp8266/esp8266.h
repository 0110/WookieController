/** @file esp8266.h
 * @brief Module to provide an WLAN connection via UART
 * @page ESP8266 UART WLAN
 * @author Ollo
 * @date 24.02.2015
 *
 * Using the following custom firmware: https://github.com/nodemcu/nodemcu-firmware
 * at this Version:  nodemcu_20150212.bin
 *
 * At startup, the following script is executed:
 * @code{.unparsed}
 * --init.lua will be excuted on each reboot
 * file.open("init.lua","w")
 * file.writeline([[wifi.setmode(wifi.SOFTAP)]])
 * file.writeline([[wifi.ap.config({ssid="mobileAP",pwd="myLAN4Bike"})]])
 * file.writeline([[ip=wifi.ap.getip()]])
 * file.writeline([[print("Access Point started with")]])
 * file.writeline([[print(ip)]])
 * file.writeline([[s=net.createServer(net.UDP)]])
 * file.writeline([[s:on("receive",function(s,c) print(c) end)]])
 * file.writeline([[s:listen(1234)]])
 * file.writeline([[print("TCP Server started on port 1234")]])
 * file.close()
 * @endcode
 *
 * @defgroup ESP6266 Output library (WLAN UART)
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
  ESP8266_RET_OK, /**< OK, no Error occurred */
  ESP8266_RET_ERROR, /**< Global not further specified error! */
  ESP8266_RET_COMMUNICATION_ERR /**< Unexpected response from ESP8266 module */
} esp8266_ret_t;

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

/** @fn void esp8266_init(void)
 * @brief initialization of the UART Wifi module.
 * If the two paramater ssid and password are not set, the last used WiFi is used again.
 */
esp8266_ret_t esp8266_init(void);

/** @fn esp8266_ret_t esp8266_printTCP(const char *s, ...)
 * @brief prints the given string on the TCP socket
 * @param[in]   s       string to print including formating definition
 * @param[in]   ...     All values for the formating options
 */
esp8266_ret_t esp8266_printUDP(const char *s, ...);

/** @fn esp8266_ret_t esp8266_debugcmd(const char *s)
 * @brief Send a Command directly to the WLAN Uart
 * @param[in]   s       string with the command
 * The output will be printet to the USB UART
 *
 * @return success or failure
 */
esp8266_ret_t esp8266_debugcmd(const char *s);

/*@}*/

#endif /* ESP8266_H_ */
