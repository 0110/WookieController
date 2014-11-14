/* @file ledstripe.h
 * @brief Interface to use ledstripe (WS2812)
 * @author Ollo
 *
 * @date 7.11.2014 – Created
 *
 * @defgroup LEDSTRIPE
 */

#ifndef _LEDSTRIPE_H_
#define _LEDSTRIPE_H_

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/** @addtogroup LEDSTRIPE */
/*@{*/

#define LEDSTRIPE_THREAD_STACK_SIZE	        1024
#define LEDSTRIPE_MAXIMUM					40      /**< Amount of supported LEDs (best a multiple of LENGTH_LED_SEND) */
#define LEDSTRIPE_COLORS_PER_LED           3       /**< Colors (each color uses one byte) per LED -> Memory usage is LEDSTRIPE_MAXIMUM times LEDSTRIPE_COLORS_PER_LED */
#define LENGTH_LEDBITS						480     /**< Space for 20 LEDs */
#define LENGTH_LED_SEND        				20      /**< Amount of LEDs that are sent at one block via SPI */

#ifndef LEDSTRIPE_THREAD_PRIORITY
#define LEDSTRIPE_THREAD_PRIORITY     (LOWPRIO + 3)
#endif


extern uint8_t ledstripe_fb[LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED];

extern WORKING_AREA(wa_ledstripe, LEDSTRIPE_THREAD_STACK_SIZE);

#ifdef __cplusplus
extern "C"
{
#endif

  void ledstripe_init(void);

#ifdef __cplusplus
}
#endif

/*@}*/
#endif /*_LEDSTRIPE_H_*/
