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


//enter number of LEDs here!
#define LEDSTRIPE_FRAMEBUFFER_SIZE 240

//Size of Ring-Buffer
#define LEDSTRIPE_PWM_BUFFER_SIZE 192

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} ledstripe_color;

/** @addtogroup LEDSTRIPE */
/*@{*/

#ifdef __cplusplus
extern "C"
{
#endif

extern ledstripe_color ledstripe_framebuffer[LEDSTRIPE_FRAMEBUFFER_SIZE];

/** @fn void ledstripe_init(void)
 * @brief Initialization of the Timer/PWM and DMA
 * @param[in] port      The port to use, e.g.: GPIOB
 * @param[in] pin       The pin to use, e.g.:  GPIOB_PIN0
 * The PIN and the port number must match! This is NOT checked internally.
 */
void ledstripe_init(GPIO_TypeDef *port, int pin);

void usbcdc_putMemory(uint8_t *text, unsigned int length);

#ifdef __cplusplus
}
#endif

/*@}*/
#endif /*_LEDSTRIPE_H_*/
