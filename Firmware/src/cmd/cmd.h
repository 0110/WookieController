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

#ifdef __cplusplus
}
#endif

#endif
