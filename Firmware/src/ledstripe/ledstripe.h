/* @file ledstripe.h
 * @brief Interface to use ledstripe
 * @author Ollo
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

#define LEDSTRIPE_THREAD_STACK_SIZE	512
#define LEDSTRIPE_MAXIMUM			256

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
