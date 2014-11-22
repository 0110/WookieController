/* @file ssd1803a-spi.h
 * @brief Interface to use LCD with a SSD1803A controller
 * @author Ollo
 *
 * Datasheets:
 * - https://cdn-reichelt.de/documents/datenblatt/A500/DS_DOGM204_UK.pdf
 * - http://www.lcd-module.de/fileadmin/eng/pdf/zubehoer/ssd1803a_2_0.pdf
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

typedef enum {  RET_OK,
                RET_NOTINITIALIZED,
                RET_IO_ERROR
} SSD1803A_RET;

#ifdef __cplusplus
extern "C"
{
#endif

  void ssd1803a_spi_init(void);

  SSD1803A_RET ssd1803a_spi_sendText(char *s, int textLength);

#ifdef __cplusplus
}
#endif

/*@}*/
#endif /*_SSD1803A_H_*/
