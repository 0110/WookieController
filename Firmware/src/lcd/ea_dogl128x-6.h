/* @file ssd1803a-spi.h
 * @brief Interface to use LCD with a EA DOGL128x-6 controller
 * @author Ollo
 *
 * @defgroup ea_dogl128x-6
 */

#ifndef _EADOGL_H_
#define _EADOGL_H_

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/** @addtogroup ea_dogl128x-6 */
/*@{*/

typedef enum {  EADOGL_RET_OK,
  EADOGL_RET_NOTINITIALIZED,
  EADOGL_RET_IO_ERROR
} EADOGL_RET;

#ifdef __cplusplus
extern "C"
{
#endif

  void eadogl_init(void);

  EADOGL_RET eadogl_sendText(char *s, int textLength);

#ifdef __cplusplus
}
#endif

/*@}*/
#endif /*_EADOGL_H_*/
