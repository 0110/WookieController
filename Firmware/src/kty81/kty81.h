/*
 * @file kty81.h
 *
 *  @date Jan 26, 2015
 *  @author ollo
 *
 *  @brief Module to read temperature from a kty81 plugged to PC1 on the STM32 Discovery board.
 */

#ifndef KTY81_H_
#define KTY81_H_

#include "ch.h"

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

/**
 * @mainpage KTY81 KTY81 Documentation
 *
 * @section SWsetup Software Setup
 *
 * The Temperature measurement needs an configured ADC.
 * The one configured in the Module reads PC0 and PC1.
 *
 * @section HWsetup Hardware setup
 *
 * The following connection was used:
 *
 * @dot
 * digraph HardwareSetup {
 * rankdir=LR;
 * node [shape=record];
 * GND [ label="GND" ];
 * VCC [ label="5V" ];
 * PIN [ label="PC1" ];
 * CON;
 * GND -> CON [ label="kty81" arrowhead=none ];
 * CON -> PIN [ label="9.8 kOhm" arrowhead=none ];
 * CON -> VCC [ label="2.5 kOhm" arrowhead=none ];
 * }
 * @enddot
 */

/** @addtogroup kty81
 * @{ */

/** @enum kty81_ret_t
 * @brief Status
 * return value for all global functions in the kty81 module
 */
typedef enum {
	RET_OK, /**< OK, no Error occurred */
	RET_ERROR /**< Global not further specified error! */
} kty81_ret_t;

/** @fn kty81_ret_t kty81_init(void)
 * @brief Initialization of the module.
 * MUST be called once (normally at boot)
 * @return status
 */
kty81_ret_t kty81_init(void);

/** @fn kty81_ret_t kty81_read(volatile int32_t *temperature)
 * Reads a temperature value.
 * @param[out] temperature      the read temperature in degree celsius
 * @return status
 */
kty81_ret_t kty81_read(volatile int32_t *temperature);

/*@}*/

#endif /* KTY81_H_ */
