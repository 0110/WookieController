/* @file ledstripe.h
 * @brief Interface to use ledstripe
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

#define LEDSTRIPE_THREAD_STACK_SIZE	        512
#define LEDSTRIPE_MAXIMUM					18      /**< Amount of supported LEDs */
#define LEDSTRIPE_COLORS_PER_LED           3       /**< Colors (each color uses one byte) per LED -> Memory usage is LEDSTRIPE_MAXIMUM times LEDSTRIPE_COLORS_PER_LED */
#define LENGTH_LEDBITS						216     /**< Space for 9 LEDs */
#define LENGTH_BYTE_SEND        			9       /**< Amount of LEDs that are sent at one block via SPI */

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
