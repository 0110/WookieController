/**
 * @file kty81.h
 *
 *  @date Jan 26, 2015
 *  @author ollo
 *
 *  @brief Module to read temperature from a kty81 plugged to PC1 on the STM32 Discovery board.
 *
 *  @defgroup kty81
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
 * @mainpage KTY81 Documentation
 *
 * @section SWsetup Software Setup
 *
 * The Temperature measurement needs an configured ADC.
 * The one configured in the Module reads PC1.
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
 *
 * The datasheet can be found at http://www.nxp.com/documents/data_sheet/KTY81_SER.pdf
 *
 * The KTY81-100 will have a resistance between 733 (-10 °C) and 1136 (40 °C) ohm.
 *
 * 20 °C is 961 ohm, so 1.37V will be measured at the PIN.
 *
 *
 * Conversion of resistance values:
 * Ohm  °C
 * 567  -40
 * 624     -30
 * 684     -20
 * 747     -10
 * 815     0
 * 886     10
 * 961     20
 * 1000    25
 * 1040    30
 * 1122    40
 * 1209    50
 * 1299    60
 * 1392    70
 * 1490    80
 * 1591    90
 * 1696    100
 * 1805    110
 * results in the following polynom (according to http://www.xuru.org/rt/PR.asp#Manually)
 * y = 1.77136749·10^-8 x^3 - 9.521069433·10^-5 x^2 + 2.654177488·10^-1 x - 162.8266092
 *
 *
 * In order to calculate the resistance of the KTY81, the following formula should be used:
 * R_KTY81 = V_PC1 * 2500 / (5.0 - V_PC1 )
 *
 * The voltage (V_PC1) is calculated based on the ADC values.
 */

/** @addtogroup kty81 Temperature measurement based on KTY81
 * @{ */

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/
#define FACTOR_X3       0.000000018
#define FACTOR_X2       -0.000095211
#define FACTOR_X1       0.265417749
#define OFFSET_X0       -162.8266092
/******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

/** @enum kty81_ret_t
 * @brief Status
 * return value for all global functions in the kty81 module
 */
typedef enum {
	RET_OK, /**< OK, no Error occurred */
	RET_ERROR /**< Global not further specified error! */
} kty81_ret_t;

/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/

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
kty81_ret_t kty81_read(int32_t *temperature);

/*@}*/

#endif /* KTY81_H_ */
