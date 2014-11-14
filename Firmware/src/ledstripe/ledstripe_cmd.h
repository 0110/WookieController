/* @file ledstripe_cmd.h
 * @brief Debuging tool, to modify the shared memory for LEDSTRIPE.
 * @author Ollo
 *
 * @date 14.11.2014 – Created
 * @defgroup LEDSTRIPE
 *
 */
/** @addtogroup LEDSTRIPE */
/*@{*/

#ifndef _LEDSTRIPE_CMD_H_
#define _LEDSTRIPE_CMD_H_

#include <stdio.h>

#include "ch.h"
#include "chprintf.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

  /* @brief Hook for Chibios to the commandline logic
   * @param chp	input and output stream for the Chibios
   * @param argc	amount of arguments
   * @param argv	the arguments themself
   * @return nothing, even on problems and errors
   */
  void
  cmd_ledstripe_modify(BaseSequentialStream *chp, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

/*@}*/
#endif /* _LEDSTRIPE_CMD_H_ */
