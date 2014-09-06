/** @file usbCD.h
 * @brief Module to provide a simple character device
 * @author Ollo
 *
 * @date 06.09.2014
 * @defgroup USB
 *
 */

#ifndef _USBCDC_H_
#define _USBCDC_H_

#include "ch.h"
#include "hal.h"

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)

#ifdef __cplusplus
extern "C"
{
#endif

/** @fn void usbCD_init(void)
 * @brief initialization of the USB stack. Must only executed once!
 */
void usbCD_init(void);

/** @fn void usbcdc_process(void)
 * @brief check for a established USB connection
 *
 * Must check must be done cyclic. In order to open a new shell, if the USB is connected.
 */
void usbCD_process(void);

#ifdef __cplusplus
}
#endif

#endif
