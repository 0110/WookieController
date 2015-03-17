/**
 * @file rpm.h
 * @date Feb 3, 2015
 * @author c3ma
 *
 * @defgroup rpm
 */
#ifndef RPM_H_
#define RPM_H_

#include "stdint.h"

/** @addtogroup rpm     Speed based on Rounds per Minute
 * @{
 */

/******************************************************************************
* DEFINITIONS
******************************************************************************/

#ifndef TRUE
#define TRUE	1	 /**< define TRUE as one (if not already present) */
#endif

#ifndef FALSE
#define FALSE	0	/**< define FALSE as zero (if not already present) */
#endif

#define RPM_THREAD_WORKING_AREA_SIZE    64      /**< Amount of bytes for the RPM thread */

/******************************************************************************
* TYPE DEFINITIONS
******************************************************************************/

/**
 * @enum rpm_ret_t
 * @brief Status
 * Return value for all global functions of rpm
 */
typedef enum {
	RET_OK,
	RET_ERROR,
        RET_ERR_NOT_INITED
} rpm_ret_t;


/******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************/

/** @fn rpm_ret_t rpm_init(void)
 * @brief Initialization function
 * @return status RET_OK on success
 */
rpm_ret_t rpm_init(void);

/** @fn rpm_ret_t rpm_getKMperHour(uint32_t*    kmPerH)
 * @brief Get actual speed
 * @param[out]  kmPerH  Actual speed in kilometer per hour
 *
 * @return status RET_OK on success
 */
rpm_ret_t rpm_getKMperHour(uint32_t*    kmPerH);

/*@}*/

#endif /* RPM_H_ */
