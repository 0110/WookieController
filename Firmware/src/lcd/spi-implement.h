/*
 * @file spi-implement.h
 *
 *  @date Nov 22, 2014
 *  @author c3ma
 */

#ifndef SPI_IMPLEMENT_H_
#define SPI_IMPLEMENT_H_


#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

/** @addtogroup LCD */

typedef enum {
	RET_OK,
	RET_ERROR
} spi_implement_ret_t;


spi_implement_ret_t     spi_implement_init(void);

/*@}*/

#endif /* SPI_IMPLEMENT_H_ */
