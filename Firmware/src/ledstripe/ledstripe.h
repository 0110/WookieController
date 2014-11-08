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

#include "hal.h"
#define sign(x) (( x > 0 ) - ( x < 0 ))

typedef struct Color Color;
	struct Color {
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

/** @addtogroup LEDSTRIPE */
/*@{*/

#ifdef __cplusplus
extern "C"
{
#endif

void ledDriverInit(int leds, GPIO_TypeDef *port, uint32_t mask, uint8_t **o_fb);
void setColorRGB(Color c, uint8_t *buf, uint32_t mask);
void testPatternFB(uint8_t *fb);
void ledDriverWaitCycle(void);


#ifdef __cplusplus
}
#endif

/*@}*/
#endif /*_LEDSTRIPE_H_*/
