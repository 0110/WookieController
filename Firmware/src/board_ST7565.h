/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define ST7565_LCD_BIAS         ST7565_LCD_BIAS_7
#define ST7565_ADC              ST7565_ADC_NORMAL
#define ST7565_COM_SCAN         ST7565_COM_SCAN_INC
#define ST7565_PAGE_ORDER       0,1,2,3,4,5,6,7
/*
 * Custom page order for several LCD boards, e.g. HEM12864-99
 * #define ST7565_PAGE_ORDER       4,5,6,7,0,1,2,3
 */

#include "ch.h"
#include "hal.h"

#define SPI_DRIVER (&SPID2)
#define DNC_PORT    GPIOB
#define DNC_PAD    11        // control=0, data=1
/*
 * SPI end transfer callback.
 */
static void spicb(SPIDriver *spip)
{
  (void) spip;
}

/*
 * SPI configuration structure.
 * The slave select line is the pin GPIOA_SPI1NSS on the port GPIOA.
 */
static SPIConfig spicfg = {
  spicb,
  /* HW dependent part.*/
  GPIOB,
  12,
  SPI_CR1_BR_2
};

static GFXINLINE void init_board(GDisplay *g) {
	g->board = 0; 
 	// Maximum speed of SSD1306 is 10Mhz, so set SPI speed less or = to that.  
 	//
 	// STM32 specific setup
 	// STM32_PCLK1 is APB1 frequence in hertz.
 	// STM32_PCLK2 is APB2 frequence in hertz.
 	// See manual clock diagram to determine APB1 or APB2 for spi in use. 
 	// SPI2 uses APB1 clock on stm32151
 	// BR bits divide PCLK as follows
 	// 000  /2   =   16 MHz
 	// 001  /4   =   8 MHz
 	// 010  /8   =   4 MHz
 	// 011  /16  =   2 MHz
 	// 100  /32  =   1 MHz
 	// 101  /64  =   500 kHz
 	// 110  /128 =   250 kHz
 	// 111  /256 =   125 kHz
 	unsigned long spi_clk = STM32_PCLK1 / 2;
 	unsigned code = 0;
 	while (spi_clk > 10000000) {
 		code++;
 		spi_clk /= 2;
 	}
 	spicfg.cr1 |= (code << 3);


 	if (g->controllerdisplay == 0) {
	  /*
	   * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
	   * PB12 - NSS.
	   * PB13 - SCK.
	   * PB14 - MISO.
	   * PB15 - MOSI.
	   */
	  spiStart(SPI_DRIVER, &spicfg);
	  palSetPad(GPIOB, 12);
	  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
				   PAL_STM32_OSPEED_HIGHEST);           /* NSS.     */
	  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
				   PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
	  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5));              /* MISO.    */
	  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
				   PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */

	  palSetPadMode(DNC_PORT, DNC_PAD, PAL_MODE_OUTPUT_PUSHPULL);
	  palClearPad(DNC_PORT, DNC_PAD); 

       }
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
	/*FIXME palWritePad(RESET_PORT, RESET_PAD, !state); */
	/* Found here: https://bitbucket.org/Tectu/ugfx/pull-requests/15/some-improvements-to-the-ssd1306-driver/diff */
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
	spiAcquireBus(SPI_DRIVER);
	spiStart(SPI_DRIVER, &spicfg); 
	spiSelect(SPI_DRIVER);

}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	spiUnselect(SPI_DRIVER);
	spiStop(SPI_DRIVER);
	spiReleaseBus(SPI_DRIVER);
}

static GFXINLINE void write_cmd(GDisplay *g, uint8_t cmd) {
	(void) g;
	(void) cmd;
	static uint8_t buf;
	palClearPad(DNC_PORT, DNC_PAD);
	buf = cmd;
	spiSend(SPI_DRIVER, 1, &buf);

}

static GFXINLINE void write_data(GDisplay *g, uint8_t* data, uint16_t length) {
	(void) g;
	(void) data;
	(void) length;
	palSetPad(DNC_PORT, DNC_PAD);
	spiSend(&SPID2, length, data);
}

#endif /* _GDISP_LLD_BOARD_H */
