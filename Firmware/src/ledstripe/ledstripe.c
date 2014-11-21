/**
 * @file ledstrip.c
 * @author Manu
 * @author Ollo
 *
 * creation date 2014-11-08
 *
 * Inspired by: https://github.com/omriiluz/WS2812B-LED-Driver-ChibiOS/blob/master/LEDDriver.c
 *
 */

#include "ledstripe/ledstripe.h"

ledstripe_color ledstripe_framebuffer[LEDSTRIPE_FRAMEBUFFER_SIZE];
static uint16_t ledstripe_pwm_buffer[LEDSTRIPE_PWM_BUFFER_SIZE];
static uint16_t frame_pos = 0;

// writes the pwm values of one byte into the array which will be used by the dma
static inline void color2pwm(uint16_t ** dest, uint8_t color) {
	uint8_t mask = 0x80;
	do {
		if (color & mask) {
			**dest = 49;
		} else {
			**dest = 20;
		}
		*dest += 1;
		mask >>= 1;
	} while (mask != 0);
}

static void Update_Buffer(uint16_t* buffer) {
	static int incomplete_return = 0;
	ledstripe_color *framebufferp;
	uint32_t i, j;
	uint16_t * bufp;

	for (i = 0; i < (LEDSTRIPE_PWM_BUFFER_SIZE / 2) / 24; i++) {
		if (incomplete_return) {
			incomplete_return = 0;
			for (j = 0; j < 24; j++) {
				buffer[i * 24 + j] = 0;
			}
		} else {
			if (frame_pos == LEDSTRIPE_FRAMEBUFFER_SIZE) {
				incomplete_return = 1;
				frame_pos = 0;
				for (j = 0; j < 24; j++) {
					buffer[i * 24 + j] = 0;
				}
			} else {
				framebufferp = &(ledstripe_framebuffer[frame_pos++]);
				bufp = buffer + (i * 24);
// edit here to change order of colors in "ws2812_framebuffer" (0x00RRGGBB, 0x00GGBBRR, etc)
// the chip needs G R B
				color2pwm(&bufp, framebufferp->green); // green
				color2pwm(&bufp, framebufferp->red); // red
				color2pwm(&bufp, framebufferp->blue); // blue
			}
		}
	}
}

static void ledstripe_irq_handler(void* data, uint32_t flags) {
	(void) data;

	// Half-Transfer completed
	// HTIF2 im DMA_LISR reg
	if ( flags & (1<<20)) {
		Update_Buffer(ledstripe_pwm_buffer);
	}

	// Transfer completed
	// TCIF2 im DMA_LISR reg
	if (flags & (1<<21)) {
		Update_Buffer(ledstripe_pwm_buffer + (LEDSTRIPE_PWM_BUFFER_SIZE / 2));
	}
}

void ledstripe_init(void) {
	int i;

	// Init buffers
	for (i = 0; i < LEDSTRIPE_PWM_BUFFER_SIZE; i++) {
		ledstripe_pwm_buffer[i] = 0;
	}
	for (i = 0; i < LEDSTRIPE_FRAMEBUFFER_SIZE; i++) {
		ledstripe_framebuffer[i].red = 0;
		ledstripe_framebuffer[i].green = 0;
		ledstripe_framebuffer[i].blue = 0;
	}

	/*  GPIO config done in board.h
	 *  AF TIM3; Push
	 */

	dmaStreamAllocate(STM32_DMA1_STREAM2, 0, ledstripe_irq_handler, NULL);

}

