/* @file ssd1803a-spi.h
 * @brief Interface to use LCD with a SSD1803A controller
 * @author Ollo
 *
 * @defgroup SSD1803A
 */

#ifndef _SSD1803A_H_
#define _SSD1803A_H_

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/** @addtogroup SSD1803A */
/*@{*/

#define SSD1803A_THREAD_STACK_SIZE	        512

#ifndef SSD1803A_THREAD_PRIORITY
#define SSD1803A_THREAD_PRIORITY     (LOWPRIO + 3)
#endif

extern WORKING_AREA(wa_ssd1803a, SSD1803A_THREAD_STACK_SIZE);

#ifdef __cplusplus
extern "C"
{
#endif

  void ssd1803a_spi_init(void);

#ifdef __cplusplus
}
#endif

/*@}*/
#endif /*_SSD1803A_H_*/
