/**
 * @file kty81.h
 *
 *  @date Jan 26, 2015
 *  @author ollo
 *
 *  @brief Module to read temperature from a kty81 plugged to PC1 on the STM32 Discovery board.
 *
 *  @defgroup kty81      Temperature measurement based on KTY81
 */

#ifndef KTY81_H_
#define KTY81_H_

#include "ch.h"

#ifndef TRUE
#define TRUE	1       /**< define TRUE as one (if not already present) */
#endif

#ifndef FALSE
#define FALSE	0       /**< define FALSE as zero (if not already present) */
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
 * <table>
 * <TR><TH>Ohm</TH><TH>°C</TH></TR>
 * <TR><TD>567</TD><TD>-40</TD></TR>
 * <TR><TD>624</TD><TD>-30</TD></TR>
 * <TR><TD>684</TD><TD>-20</TD></TR>
 * <TR><TD>747</TD><TD>-10</TD></TR>
 * <TR><TD>815</TD><TD>0</TD></TR>
 * <TR><TD>886</TD><TD>10</TD></TR>
 * <TR><TD>961</TD><TD>20</TD></TR>
 * <TR><TD>1000</TD><TD>25</TD></TR>
 * <TR><TD>1040</TD><TD>30</TD></TR>
 * <TR><TD>1122</TD><TD>40</TD></TR>
 * <TR><TD>1209</TD><TD>50</TD></TR>
 * <TR><TD>1299</TD><TD>60</TD></TR>
 * <TR><TD>1392</TD><TD>70</TD></TR>
 * <TR><TD>1490</TD><TD>80</TD></TR>
 * <TR><TD>1591</TD><TD>90</TD></TR>
 * <TR><TD>1696</TD><TD>100</TD></TR>
 * <TR><TD>1805</TD><TD>110</TD></TR>
 * </table>
 * This values results in the following polynom (according to http://www.xuru.org/rt/PR.asp#Manually):<br/>
 * <code>
 * y = 1.77136749·10^-8 x^3 - 9.521069433·10^-5 x^2 + 2.654177488·10^-1 x - 162.8266092
 * </code><br/>
 *
 * In order to calculate the resistance of the KTY81, the following formula should be used:<br/>
 * <code>
 * R_KTY81 = V_PC1 * 2500 / (5.0 - V_PC1 )
 * </code><br/>
 * The voltage (V_PC1) is calculated based on the ADC values.
 */

/** @addtogroup kty81
 * @{ */

/******************************************************************************
 * DEFINITIONS
 ******************************************************************************/

#define KTY81_TEMP_FACTOR_X3       0.000000018     /**< Factor in front of for x³ in the formula x³ + x² + x + offset, necessary for the temperature calculation */
#define KTY81_TEMP_FACTOR_X2       -0.000095211    /**< Factor in front of for x² in the formula x³ + x² + x + offset, necessary for the temperature calculation */
#define KTY81_TEMP_FACTOR_X1       0.265417749     /**< Factor in front of for x in the formula x³ + x² + x + offset, necessary for the temperature calculation */
#define KTY81_TEMP_OFFSET_X0       -162.8266092    /**< Offset at the end of the formula x³ + x² + x + offset*/

#define KTY81_ADC2V_FACTOR            0.00075f           /**< Factor to calculate between ADC and the volt value received on the PIN; Used in the formula: V= FACTOR * (uC-ADC - OFFSET) + OFFSET */
#define KTY81_ADC2V_OFFSET            0.0f               /**< Offset between ADC and the volt value received on the PIN; Used in the formula: V= FACTOR * (uC-ADC - OFFSET) + OFFSET  */

#define KTY81_VCC                     5                  /**< Used VCC for the temperature measurement circuit */
#define KTY81_PULLUP_RESISTOR         2500               /**< Pull Up resistor between VCC and the measure point */

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

/** @fn kty81_ret_t kty81_read(int32_t *temperature)
 * Reads a temperature value.
 * @param[out] temperature      the read temperature in degree celsius
 * @return status               RET_OK  on success
 */
kty81_ret_t kty81_read(int32_t *temperature);

/*@}*/

#endif /* KTY81_H_ */
