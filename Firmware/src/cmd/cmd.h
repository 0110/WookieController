#ifndef _CMD_H_
#define _CMD_H_

#include "ch.h"

#ifdef __cplusplus
extern "C"
{
#endif

  void
  cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]);

  void
  cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]);

  void
  cmd_ifconfig(BaseSequentialStream *chp, int argc, char *argv[]);

  void
  cmd_cat(BaseSequentialStream *chp, int argc, char *argv[]);

  void
  cmd_flash(BaseSequentialStream *chp, int argc, char *argv[]);


  void cmd_ledctrl(BaseSequentialStream *chp, int argc, char *argv[]);

  void cmd_ws2812(BaseSequentialStream *chp, int argc, char *argv[]);

  /** @fn void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[])
   * @param[in] ch	shell to print on (optional, so it can be NULL)
   * @param[in] argc amount of arguments passed to the function	(optional, can be zero)
   * @param[in] argv arguments itself  (optional, can be NULL)
   */
  void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
