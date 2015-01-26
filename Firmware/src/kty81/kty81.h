/*
 * @file kty81.h
 *
 *  @date Jan 26, 2015
 *  @author ollo
 *
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

/** @addtogroup DefaultGroup */

typedef enum {
	RET_OK,
	RET_ERROR
} kty81_ret_t;

/** @fn kty81_ret_t kty81_init(void)
 * Initialization of the module.
 * MUST be called once (normally at boot)
 * @return status
 */
kty81_ret_t kty81_init(void);

/** @fn kty81_ret_t kty81_read(volatile uint32_t *temperature)
 * Reads a temperature value.
 * @param[out] temperature      the read temperature in degree celsius
 * @return status
 */
kty81_ret_t kty81_read(volatile uint32_t *temperature);

/*@}*/

#endif /* KTY81_H_ */
