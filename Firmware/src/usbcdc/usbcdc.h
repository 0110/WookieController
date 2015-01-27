/** @file usbcdc.h
 * @brief Module to provide an USB CDC (USB serial interface)
 * @author Ollo
 *
 * @date 28.06.2014
 *
 * @defgroup USB Output library (USB UART)
 * @{
 */

#ifndef _USBCDC_H_
#define _USBCDC_H_

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)       /**< Memory for the SHELL connected via USB */

#ifdef __cplusplus
extern "C"
{
#endif

/** @fn void usbcdc_init(const ShellCommand * commands)
 * @param[in] commands     the available commands to execute.
 * @brief initialization of the USB stack. Must only executed once!
 */
void usbcdc_init(const ShellCommand * commands);

/** @fn void usbcdc_process(void)
 * @brief check for a established USB connection
 *
 * Must check must be done cyclic. In order to open a new shell, if the USB is connected.
 */
void usbcdc_process(void);

/** @fn void usbcdc_print(const char *s, ...)
 * @brief prints the given string on the USB CDC
 * @param[in]	s	string to print including formating definition
 * @param[in]   ...     All values for the formating options
 */
void usbcdc_print(const char *s, ...);

#ifdef __cplusplus
}
#endif

/*@}*/

#endif
