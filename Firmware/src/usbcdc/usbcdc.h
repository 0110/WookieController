#ifndef _USBCDC_H_
#define _USBCDC_H_

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)

#ifdef __cplusplus
extern "C"
{
#endif

void usbcdc_init(const ShellCommand * commands);

void usbcdc_process(void);

#ifdef __cplusplus
}
#endif

#endif
